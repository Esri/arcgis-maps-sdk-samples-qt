// [WriteFile Name=AnalyzeTerrainSuitabilityWithSlopeAndAspect, Category=Analysis]
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

// sample headers
#include "AnalyzeTerrainSuitabilityWithSlopeAndAspect.h"

// ArcGIS Maps SDK headers
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "BooleanFieldFunction.h"
#include "Colormap.h"
#include "ColormapRenderer.h"
#include "ContinuousField.h"
#include "ContinuousFieldFunction.h"
#include "DiscreteFieldFunction.h"
#include "Envelope.h"
#include "FieldAnalysis.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SpatialReference.h"

// Qt headers
#include <QColor>
#include <QFileInfo>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }
} // namespace

AnalyzeTerrainSuitabilityWithSlopeAndAspect::AnalyzeTerrainSuitabilityWithSlopeAndAspect(QObject* parent /* = nullptr */) :
  QObject(parent)
{
  connect(&m_createContinuousFieldWatcher, &QFutureWatcher<ContinuousField*>::finished, this, [this]()
  {
    ContinuousField* continuousField = m_createContinuousFieldWatcher.result();
    if (!continuousField)
    {
      return;
    }

    // Create the analysis overlay once the raster field is available.
    if (!m_analysisOverlay && m_mapView)
    {
      m_analysisOverlay = new AnalysisOverlay(this);
      m_mapView->analysisOverlays()->append(m_analysisOverlay);
    }

    // Derive slope and aspect from the elevation field
    m_elevationFieldFunction = ContinuousFieldFunction::create(continuousField, this);
    m_slopeFunction = m_elevationFieldFunction->slope();
    m_aspectFunction = m_elevationFieldFunction->aspect();
    m_aboveSeaLevelSelection = m_elevationFieldFunction->isGreaterThanOrEqualTo(0.0F);

    // Build both scenarios up front and toggle visibility when the user changes selection.
    buildAnalysisForScenario(GentleSouthFacingSlopes);
    buildAnalysisForScenario(SteepWestAndNorthFacingSlopes);
    applyFieldAnalysisVisibility();

    if (m_mapView)
    {
      m_mapView->setViewpointCenterAsync(continuousField->extent().center(), 200000);
    }
  });
}

AnalyzeTerrainSuitabilityWithSlopeAndAspect::~AnalyzeTerrainSuitabilityWithSlopeAndAspect() = default;

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AnalyzeTerrainSuitabilityWithSlopeAndAspect>("Esri.Samples", 1, 0, "AnalyzeTerrainSuitabilityWithSlopeAndAspectSample");
}

MapQuickView* AnalyzeTerrainSuitabilityWithSlopeAndAspect::mapView() const
{
  return m_mapView;
}

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;

  m_map = new Map(SpatialReference(32630), this);
  loadElevationField();

  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::loadElevationField()
{
  const QString rasterPath = defaultDataPath() + QStringLiteral("/ArcGIS/Runtime/Data/raster/arran.tif");
  if (!QFileInfo::exists(rasterPath))
  {
    return;
  }

  // Load the raster into a continuous field that can be queried for slope and aspect.
  const SpatialReference spatialReference(32630);
  m_createContinuousFieldWatcher.setFuture(ContinuousField::createFromFilesAsync({rasterPath}, 0, spatialReference, this));
}

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::buildAnalysisForScenario(SiteScenario scenario)
{
  switch (scenario)
  {
    case GentleSouthFacingSlopes:
      if (m_gentleSouthFacingSlopesAnalysis)
      {
        return;
      }

      // Green highlights sheltered, lowland south-facing areas.
      m_gentleSouthFacingSlopesAnalysis = createScenarioAnalysis(0.0F, 20.0F, 112.5F, 247.5F, 0.0F, 300.0F, QColor(Qt::green));
      m_analysisOverlay->analyses()->append(m_gentleSouthFacingSlopesAnalysis);
      break;

    case SteepWestAndNorthFacingSlopes:
      if (m_steepWestAndNorthFacingSlopesAnalysis)
      {
        return;
      }

      // Purple highlights steeper upland terrain facing west through north.
      m_steepWestAndNorthFacingSlopesAnalysis = createScenarioAnalysis(20.0F, 80.0F, 202.5F, 67.5F, 300.0F, 850.0F, QColor("purple"));
      m_analysisOverlay->analyses()->append(m_steepWestAndNorthFacingSlopesAnalysis);
      break;
  }
}

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::applyFieldAnalysisVisibility()
{
  buildAnalysisForScenario(GentleSouthFacingSlopes);
  buildAnalysisForScenario(SteepWestAndNorthFacingSlopes);

  if (m_gentleSouthFacingSlopesAnalysis)
  {
    m_gentleSouthFacingSlopesAnalysis->setVisible(m_selectedScenario == GentleSouthFacingSlopes);
  }

  if (m_steepWestAndNorthFacingSlopesAnalysis)
  {
    m_steepWestAndNorthFacingSlopesAnalysis->setVisible(m_selectedScenario == SteepWestAndNorthFacingSlopes);
  }
}

FieldAnalysis* AnalyzeTerrainSuitabilityWithSlopeAndAspect::createScenarioAnalysis(float slopeMin,
                                                                                   float slopeMax,
                                                                                   float aspectStart,
                                                                                   float aspectEnd,
                                                                                   float elevationMin,
                                                                                   float elevationMax,
                                                                                   const QColor& color)
{
  // Build the scenario mask first, then convert it into a field analysis rendered with two colors.
  BooleanFieldFunction* scenarioFieldFunction = createScenarioFieldFunction(slopeMin, slopeMax, aspectStart, aspectEnd, elevationMin, elevationMax);

  const QList<QColor> colors{QColor(Qt::white), color};
  ColormapRenderer* renderer = new ColormapRenderer(Colormap::create(colors, this), this);

  FieldAnalysis* analysis = FieldAnalysis::create(scenarioFieldFunction->toDiscreteFieldFunction(), renderer, this);
  analysis->setVisible(false);
  return analysis;
}

BooleanFieldFunction* AnalyzeTerrainSuitabilityWithSlopeAndAspect::createScenarioFieldFunction(float slopeMin,
                                                                                               float slopeMax,
                                                                                               float aspectStart,
                                                                                               float aspectEnd,
                                                                                               float elevationMin,
                                                                                               float elevationMax)
{
  // Each mask represents a terrain condition that must be satisfied.
  BooleanFieldFunction* slopeRangeMask =
    m_slopeFunction->isGreaterThanOrEqualTo(slopeMin)->logicalAnd(m_slopeFunction->isLessThanOrEqualTo(slopeMax));

  BooleanFieldFunction* aspectRangeMask = nullptr;
  if (aspectStart <= aspectEnd)
  {
    // Normal aspect range, such as west through north.
    aspectRangeMask = m_aspectFunction->isGreaterThanOrEqualTo(aspectStart)->logicalAnd(m_aspectFunction->isLessThanOrEqualTo(aspectEnd));
  }
  else
  {
    // Wrapped aspect range, such as south-facing terrain spanning 0 degrees.
    BooleanFieldFunction* aspectFromStartToNorth =
      m_aspectFunction->isGreaterThanOrEqualTo(aspectStart)->logicalAnd(m_aspectFunction->isLessThan(360.0F));
    BooleanFieldFunction* aspectFromZeroToEnd =
      m_aspectFunction->isGreaterThanOrEqualTo(0.0F)->logicalAnd(m_aspectFunction->isLessThanOrEqualTo(aspectEnd));
    aspectRangeMask = aspectFromStartToNorth->logicalOr(aspectFromZeroToEnd);
  }

  // Elevation keeps the analysis within the intended lowland/upland bands.
  BooleanFieldFunction* elevationRangeMask =
    m_elevationFieldFunction->isGreaterThanOrEqualTo(elevationMin)->logicalAnd(m_elevationFieldFunction->isLessThanOrEqualTo(elevationMax));

  return slopeRangeMask->logicalAnd(aspectRangeMask)->logicalAnd(elevationRangeMask)->logicalAnd(m_aboveSeaLevelSelection);
}

void AnalyzeTerrainSuitabilityWithSlopeAndAspect::setSelectedScenario(SiteScenario scenario)
{
  if (m_selectedScenario == scenario)
  {
    return;
  }

  m_selectedScenario = scenario;
  if (m_analysisOverlay)
  {
    applyFieldAnalysisVisibility();
  }

  emit selectedScenarioChanged();
}
