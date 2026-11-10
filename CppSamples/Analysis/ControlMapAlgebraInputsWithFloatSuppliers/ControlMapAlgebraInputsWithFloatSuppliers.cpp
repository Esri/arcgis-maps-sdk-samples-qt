// [WriteFile Name=ControlMapAlgebraInputsWithFloatSuppliers, Category=Analysis]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ControlMapAlgebraInputsWithFloatSuppliers.h"

#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "AnalysisListModel.h"
#include "AnalysisViewState.h"
#include "BooleanFieldFunction.h"
#include "ColorRamp.h"
#include "ContinuousField.h"
#include "ContinuousFieldFunction.h"
#include "ContinuousFieldSupplier.h"
#include "Envelope.h"
#include "Error.h"
#include "FieldAnalysis.h"
#include "FloatSupplier.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapViewTypes.h"
#include "MinMaxStretchParameters.h"
#include "Point.h"
#include "RasterTypes.h"
#include "SpatialReference.h"
#include "StretchRenderer.h"

#include <QFileInfo>
#include <QFuture>
#include <QList>
#include <QStandardPaths>
#include <QQmlEngine>

using namespace Esri::ArcGISRuntime;

namespace
{
  const SpatialReference utm32NSpatialReference(32632);

  QString elevationRasterPath()
  {
    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    return dataPath + QStringLiteral("/ArcGIS/Runtime/Data/raster/Meribel_32N.tif");
  }
} // namespace

ControlMapAlgebraInputsWithFloatSuppliers::ControlMapAlgebraInputsWithFloatSuppliers(QObject* parent) :
  QObject(parent)
{
}

ControlMapAlgebraInputsWithFloatSuppliers::~ControlMapAlgebraInputsWithFloatSuppliers() = default;

void ControlMapAlgebraInputsWithFloatSuppliers::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ControlMapAlgebraInputsWithFloatSuppliers>("Esri.Samples", 1, 0, "ControlMapAlgebraInputsWithFloatSuppliersSample");
}

MapQuickView* ControlMapAlgebraInputsWithFloatSuppliers::mapView() const
{
  return m_mapView;
}

void ControlMapAlgebraInputsWithFloatSuppliers::setMapView(MapQuickView* mapView)
{
  if (!mapView || m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_map = new Map(utm32NSpatialReference, this);
  m_mapView->setMap(m_map);
  loadElevationField();
  emit mapViewChanged();
}

// 5. Slider changes update the suppliers used by the existing analysis function.
void ControlMapAlgebraInputsWithFloatSuppliers::setSlopeRange(float minimum, float maximum)
{
  if (minimum == m_minimumSlope && maximum == m_maximumSlope)
  {
    return;
  }

  const float previousMinimum = m_minimumSlope;
  const float previousMaximum = m_maximumSlope;
  m_minimumSlope = minimum;
  m_maximumSlope = maximum;
  if (m_minimumSlopeSupplier && previousMinimum != minimum)
  {
    m_minimumSlopeSupplier->setValue(std::make_optional<float>(minimum));
  }
  if (m_maximumSlopeSupplier && previousMaximum != maximum)
  {
    m_maximumSlopeSupplier->setValue(std::make_optional<float>(maximum));
  }
  emit slopeRangeChanged();
}

void ControlMapAlgebraInputsWithFloatSuppliers::loadElevationField()
{
  constexpr int band = 0;
  const QString path = elevationRasterPath();
  if (!QFileInfo::exists(path))
  {
    setErrorMessage(QStringLiteral("Elevation data is missing. Expected file: %1").arg(path));
    return;
  }

  // 1. Create a continuous field from the raster, then wrap it in a supplier after loading.
  ContinuousField::createFromFilesAsync({path}, band, utm32NSpatialReference, this)
    .then(this,
          [this](ContinuousField* continuousField)
  {
    onContinuousFieldCreated(continuousField);
  })
    .onFailed(this, [this]()
  {
    setErrorMessage(QStringLiteral("Failed to load the elevation raster. Verify that Meribel_32N.tif is readable and valid."));
  });
}

void ControlMapAlgebraInputsWithFloatSuppliers::onContinuousFieldCreated(ContinuousField* continuousField)
{
  if (!continuousField)
  {
    setErrorMessage(QStringLiteral("Failed to create a continuous field from the elevation raster."));
    return;
  }

  m_elevationFunction = new ContinuousFieldSupplier(continuousField, this);
  // 2. Derive slope and create suppliers for its minimum and maximum values.
  m_slopeFunction = m_elevationFunction->slope();
  m_minimumSlopeSupplier = new FloatSupplier(std::make_optional<float>(m_minimumSlope), this);
  m_maximumSlopeSupplier = new FloatSupplier(std::make_optional<float>(m_maximumSlope), this);

  // 3. Compare the slope to both suppliers, combine the conditions, and mask the slope field.
  BooleanFieldFunction* aboveMinimum = m_slopeFunction->isGreaterThanOrEqualTo(m_minimumSlopeSupplier);
  BooleanFieldFunction* belowMaximum = m_slopeFunction->isLessThanOrEqualTo(m_maximumSlopeSupplier);
  BooleanFieldFunction* inRange = aboveMinimum->logicalAnd(belowMaximum);
  ContinuousFieldFunction* selectedSlopes = m_slopeFunction->mask(inRange);

  constexpr int colorRampSize = 256;
  const QList<double> gammaValues{1.0};
  constexpr bool estimateStats = false;
  const QList<double> elevationMinimumValues{1199.0};
  const QList<double> elevationMaximumValues{2740.0};
  const MinMaxStretchParameters elevationStretchParameters(elevationMinimumValues, elevationMaximumValues);
  ColorRamp* elevationColorRamp = ColorRamp::create(PresetColorRampType::Surface, colorRampSize, this);

  StretchRenderer* elevationRenderer = new StretchRenderer(elevationStretchParameters, gammaValues, estimateStats, elevationColorRamp, this);

  const QList<double> slopeMinimumValues{0.0};
  const QList<double> slopeMaximumValues{85.0};
  const MinMaxStretchParameters slopeStretchParameters(slopeMinimumValues, slopeMaximumValues);
  ColorRamp* slopeColorRamp = ColorRamp::create(PresetColorRampType::Viridis, colorRampSize, this);

  StretchRenderer* slopeRenderer = new StretchRenderer(slopeStretchParameters, gammaValues, estimateStats, slopeColorRamp, this);

  // 4. Display the elevation and masked slope functions as analyses in an overlay.
  m_analysisOverlay = new AnalysisOverlay(this);
  m_mapView->analysisOverlays()->append(m_analysisOverlay);

  FieldAnalysis* elevationAnalysis = FieldAnalysis::create(m_elevationFunction, elevationRenderer, this);
  if (elevationAnalysis)
  {
    elevationAnalysis->setOpacity(0.6F);
    m_analysisOverlay->analyses()->append(elevationAnalysis);
  }

  m_slopeAnalysis = FieldAnalysis::create(selectedSlopes, slopeRenderer, this);

  connect(m_mapView, &MapQuickView::analysisViewStateChanged, this, &ControlMapAlgebraInputsWithFloatSuppliers::onAnalysisViewStateChanged);

  m_analysisOverlay->analyses()->append(m_slopeAnalysis);
  constexpr double initialViewpointScale = 10000.0;
  m_mapView->setViewpointCenterAsync(continuousField->extent().center(), initialViewpointScale);
  setErrorMessage(QString());
}

void ControlMapAlgebraInputsWithFloatSuppliers::onAnalysisViewStateChanged(Analysis* analysis, const AnalysisViewState& viewState)
{
  if (analysis != m_slopeAnalysis)
  {
    return;
  }

  const AnalysisViewStatus status = viewState.status();
  switch (status)
  {
    case AnalysisViewStatus::Updating:
      setUpdating(true);
      break;
    case AnalysisViewStatus::Error:
    {
      setUpdating(false);
      const Error error = viewState.error();
      setErrorMessage(error.additionalMessage().isEmpty() ? error.message() : error.message() + QStringLiteral("\n") + error.additionalMessage());
      break;
    }
    case AnalysisViewStatus::UpToDate:
      setUpdating(false);
      setErrorMessage(QString());
      break;
  }
}

void ControlMapAlgebraInputsWithFloatSuppliers::setUpdating(bool updating)
{
  if (m_updating == updating)
  {
    return;
  }

  m_updating = updating;
  emit updatingChanged();
}

void ControlMapAlgebraInputsWithFloatSuppliers::setErrorMessage(const QString& errorMessage)
{
  if (m_errorMessage == errorMessage)
  {
    return;
  }

  m_errorMessage = errorMessage;
  emit errorMessageChanged();
}
