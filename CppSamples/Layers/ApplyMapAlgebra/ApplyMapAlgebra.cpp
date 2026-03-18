// [WriteFile Name=ApplyMapAlgebra, Category=Layers]
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
#include "ApplyMapAlgebra.h"

// ArcGIS Maps SDK headers
#include "BooleanFieldFunction.h"
#include "ColorRamp.h"
#include "Colormap.h"
#include "ColormapRenderer.h"
#include "ContinuousField.h"
#include "ContinuousFieldFunction.h"
#include "DiscreteField.h"
#include "DiscreteFieldFunction.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MinMaxStretchParameters.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "SpatialReference.h"
#include "StretchRenderer.h"
#include "Viewpoint.h"

// Qt headers
#include <QDir>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

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

    return dataPath + "/ArcGIS/Runtime/Data/raster";
  }
} // namespace

ApplyMapAlgebra::ApplyMapAlgebra(QQuickItem* parent /* = nullptr */) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath())
{
  connect(&m_createElevationFieldWatcher, &QFutureWatcher<ContinuousField*>::finished, this, [this]()
  {
    m_elevationField = m_createElevationFieldWatcher.result();
    if (!m_elevationField)
    {
      setBusy(false);
      setErrorMessage(QStringLiteral("Failed to create a continuous field from the elevation raster."));
      return;
    }

    setDataAvailable(true);
    setErrorMessage(QString());
    setBusy(false);
  });

  connect(&m_createGeomorphicFieldWatcher, &QFutureWatcher<DiscreteField*>::finished, this, [this]()
  {
    m_geomorphicField = m_createGeomorphicFieldWatcher.result();
    if (!m_geomorphicField)
    {
      setBusy(false);
      setErrorMessage(QStringLiteral("Failed to evaluate the geomorphic map algebra field."));
      return;
    }

    exportGeomorphicField();
  });

  connect(&m_exportGeomorphicFieldWatcher, &QFutureWatcher<QStringList>::finished, this, [this]()
  {
    const QStringList exportedFiles = m_exportGeomorphicFieldWatcher.result();
    if (exportedFiles.isEmpty())
    {
      setBusy(false);
      setErrorMessage(QStringLiteral("The geomorphic results could not be exported to a raster file."));
      return;
    }

    const QString& rasterFilePath = exportedFiles.constFirst();
    if (!QFileInfo::exists(rasterFilePath))
    {
      setBusy(false);
      setErrorMessage(QStringLiteral("The exported geomorphic raster file was not found."));
      return;
    }

    displayGeomorphicRaster(rasterFilePath);
    setResultsAvailable(true);
    setShowGeomorphicResults(true);
    setErrorMessage(QString());
    setBusy(false);
  });
}

ApplyMapAlgebra::~ApplyMapAlgebra() = default;

void ApplyMapAlgebra::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ApplyMapAlgebra>("Esri.Samples", 1, 0, "ApplyMapAlgebraSample");
}

void ApplyMapAlgebra::componentComplete()
{
  QQuickItem::componentComplete();

  m_mapView = findChild<MapQuickView*>("mapView");
  if (!m_mapView)
  {
    setErrorMessage(QStringLiteral("MapView was not found in the QML component tree."));
    return;
  }

  setupMap();
}

void ApplyMapAlgebra::categorize()
{
  if (m_busy || !m_elevationField)
  {
    return;
  }

  setBusy(true);
  setErrorMessage(QString());
  createGeomorphicField();
}

bool ApplyMapAlgebra::busy() const
{
  return m_busy;
}

bool ApplyMapAlgebra::dataAvailable() const
{
  return m_dataAvailable;
}

bool ApplyMapAlgebra::resultsAvailable() const
{
  return m_resultsAvailable;
}

bool ApplyMapAlgebra::showGeomorphicResults() const
{
  return m_showGeomorphicResults;
}

QString ApplyMapAlgebra::errorMessage() const
{
  return m_errorMessage;
}

// Sample workflow
void ApplyMapAlgebra::setupMap()
{
  m_map = new Map(BasemapStyle::ArcGISHillshadeDark, this);
  m_map->setInitialViewpoint(Viewpoint(55.584612, -5.234218, 300000));
  m_mapView->setMap(m_map);

  displayElevationRaster();
  createElevationField();
}

void ApplyMapAlgebra::createElevationField()
{
  const QString rasterPath = elevationRasterPath();
  if (!QFileInfo::exists(rasterPath))
  {
    setErrorMessage(QStringLiteral("The sample data was not found at %1.").arg(rasterPath));
    return;
  }

  setBusy(true);
  setErrorMessage(QString());
  m_createElevationFieldWatcher.setFuture(ContinuousField::createFromFilesAsync({rasterPath}, 0, SpatialReference::wgs84(), this));
}

void ApplyMapAlgebra::createGeomorphicField()
{
  ContinuousFieldFunction* continuousFieldFunction = ContinuousFieldFunction::create(m_elevationField, this);
  ContinuousFieldFunction* elevationFieldFunction = continuousFieldFunction->mask(continuousFieldFunction->isGreaterThanOrEqualTo(0.0F));

  DiscreteFieldFunction* tenMeterBinField = elevationFieldFunction->divide(10)->floor()->multiply(10)->toDiscreteFieldFunction();

  BooleanFieldFunction* isRaisedShoreline = tenMeterBinField->isGreaterThanOrEqualTo(0)->logicalAnd(tenMeterBinField->isLessThan(10));

  BooleanFieldFunction* isIceCovered = tenMeterBinField->isGreaterThanOrEqualTo(10)->logicalAnd(tenMeterBinField->isLessThan(600));

  BooleanFieldFunction* isIceFreeHighGround = tenMeterBinField->isGreaterThanOrEqualTo(600);

  DiscreteFieldFunction* geomorphicFieldFunction =
    tenMeterBinField->replaceIf(isRaisedShoreline, 1)->replaceIf(isIceCovered, 2)->replaceIf(isIceFreeHighGround, 3);

  m_createGeomorphicFieldWatcher.setFuture(geomorphicFieldFunction->evaluateAsync(this));
}

void ApplyMapAlgebra::exportGeomorphicField()
{
  // Raster layers display files, so the evaluated field is written to a temporary raster first.
  const QString outputDirectory = tempOutputDirectory();
  clearPreviousExportedFiles(outputDirectory);
  m_exportGeomorphicFieldWatcher.setFuture(m_geomorphicField->exportToFilesAsync(outputDirectory, QStringLiteral("geomorphicCategorization")));
}

// Display helpers
void ApplyMapAlgebra::displayElevationRaster()
{
  const QString rasterPath = elevationRasterPath();
  if (!QFileInfo::exists(rasterPath))
  {
    setErrorMessage(QStringLiteral("The sample data was not found at %1.").arg(rasterPath));
    return;
  }

  Raster* raster = new Raster(rasterPath, this);
  m_elevationRasterLayer = new RasterLayer(raster, this);

  const MinMaxStretchParameters stretchParameters({0.0}, {874.0});
  StretchRenderer* stretchRenderer =
    new StretchRenderer(stretchParameters, {1.0}, false, ColorRamp::create(PresetColorRampType::Surface, 256, this), this);

  m_elevationRasterLayer->setRenderer(stretchRenderer);
  m_elevationRasterLayer->setOpacity(0.5F);
  m_map->operationalLayers()->append(m_elevationRasterLayer);
}

void ApplyMapAlgebra::displayGeomorphicRaster(const QString& rasterFilePath)
{
  if (m_geomorphicRasterLayer)
  {
    m_map->operationalLayers()->removeOne(m_geomorphicRasterLayer);
    m_geomorphicRasterLayer->deleteLater();
    m_geomorphicRasterLayer = nullptr;
  }

  Raster* raster = new Raster(rasterFilePath, this);
  m_geomorphicRasterLayer = new RasterLayer(raster, this);

  const QMap<int, QColor> colors{
    {1, QColor(QStringLiteral("#2f6db3"))},
    {2, QColor(QStringLiteral("#73c7c7"))},
    {3, QColor(QStringLiteral("#7d4a2d"))},
  };

  ColormapRenderer* colormapRenderer = new ColormapRenderer(Colormap::create(colors, this), this);
  m_geomorphicRasterLayer->setRenderer(colormapRenderer);
  m_geomorphicRasterLayer->setOpacity(0.5F);
  m_map->operationalLayers()->append(m_geomorphicRasterLayer);
  updateVisibleRasterLayer();
}

void ApplyMapAlgebra::updateVisibleRasterLayer()
{
  if (m_elevationRasterLayer)
  {
    m_elevationRasterLayer->setVisible(!m_showGeomorphicResults);
  }

  if (m_geomorphicRasterLayer)
  {
    m_geomorphicRasterLayer->setVisible(m_showGeomorphicResults);
  }
}

// File system helpers
QString ApplyMapAlgebra::elevationRasterPath() const
{
  return m_dataPath + QStringLiteral("/arran.tif");
}

QString ApplyMapAlgebra::tempOutputDirectory() const
{
  const QString basePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  QDir outputDirectory(basePath + QStringLiteral("/ApplyMapAlgebra"));
  outputDirectory.mkpath(QStringLiteral("."));
  return outputDirectory.path();
}

void ApplyMapAlgebra::clearPreviousExportedFiles(const QString& outputDirectory) const
{
  QDir directory(outputDirectory);
  const QStringList existingFiles = directory.entryList({QStringLiteral("geomorphicCategorization*")}, QDir::Files);
  for (const QString& fileName : existingFiles)
  {
    directory.remove(fileName);
  }
}

// Internal state
void ApplyMapAlgebra::setBusy(bool busy)
{
  if (m_busy == busy)
  {
    return;
  }

  m_busy = busy;
  emit busyChanged();
}

void ApplyMapAlgebra::setDataAvailable(bool dataAvailable)
{
  if (m_dataAvailable == dataAvailable)
  {
    return;
  }

  m_dataAvailable = dataAvailable;
  emit dataAvailableChanged();
}

void ApplyMapAlgebra::setResultsAvailable(bool resultsAvailable)
{
  if (m_resultsAvailable == resultsAvailable)
  {
    return;
  }

  m_resultsAvailable = resultsAvailable;
  emit resultsAvailableChanged();
}

void ApplyMapAlgebra::setShowGeomorphicResults(bool showGeomorphicResults)
{
  if (m_showGeomorphicResults == showGeomorphicResults)
  {
    return;
  }

  m_showGeomorphicResults = showGeomorphicResults;
  updateVisibleRasterLayer();
  emit showGeomorphicResultsChanged();
}

void ApplyMapAlgebra::setErrorMessage(const QString& errorMessage)
{
  if (m_errorMessage == errorMessage)
  {
    return;
  }

  m_errorMessage = errorMessage;
  emit errorMessageChanged();
}
