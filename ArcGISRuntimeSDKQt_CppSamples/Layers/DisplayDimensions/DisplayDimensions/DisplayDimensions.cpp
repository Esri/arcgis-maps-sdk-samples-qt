// [WriteFile Name=DisplayDimensions, Category=Layers]
// [Legal]
// Copyright 2021 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "DisplayDimensions.h"

#include "DimensionLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

namespace
{
  // Helper method to get cross platform data path.
  QString defaultDataPath()
  {
    QString dataPath;

    #ifdef Q_OS_ANDROID
      dataPath = "/sdcard";
    #elif defined Q_OS_IOS
      dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    #else
      dataPath = QDir::homePath();
    #endif

    return dataPath;
  }

  // Sample MMPK location.
  const QString edinburghPylonFilePath {"/ArcGIS/Runtime/Data/mmpk/Edinburgh_Pylon_Dimensions.mmpk"};
}

DisplayDimensions::DisplayDimensions(QObject* parent /* = nullptr */):
  QObject(parent)
{
  QString mapPackagePath = defaultDataPath() + edinburghPylonFilePath;
  m_mmpk = new MobileMapPackage(mapPackagePath, this);

  connect(m_mmpk, &MobileMapPackage::doneLoading, this, &DisplayDimensions::addMapToMapView);
  connect(m_mmpk, &MobileMapPackage::errorOccurred, this, &DisplayDimensions::handleError);
  m_mmpk->load();
}

DisplayDimensions::~DisplayDimensions() = default;

void DisplayDimensions::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayDimensions>("Esri.Samples", 1, 0, "DisplayDimensionsSample");
}

void DisplayDimensions::addMapToMapView(const Error& error)
{
  // Any errors will be managed by the handleError method.
  if (error.isEmpty() && m_mmpk->loadStatus() == LoadStatus::Loaded && m_mmpk->maps().count() > 0)
  {
    setMMPKLoadStatus(true);

    m_map = m_mmpk->maps().at(0);
    m_map->setInitialViewpoint(Viewpoint(Point(-3.3098678, 55.9074044, SpatialReference::wgs84()), 30000));
    m_mapView->setMap(m_map);

    findDimensionLayer();
  }
  else
    setMMPKLoadStatus(false);
}

void DisplayDimensions::findDimensionLayer()
{
  LayerListModel* layers = m_mapView->map()->operationalLayers();

  for (Layer* layer : *layers)
  {
    if (layer->layerType() == LayerType::DimensionLayer)
    {
      m_dimensionLayer = dynamic_cast<DimensionLayer*>(layer);
      setDimensionLayerName(m_dimensionLayer->name());
    }
  }
}

void DisplayDimensions::handleError(const Error& error)
{
  if (error.additionalMessage().isEmpty())
    setErrorMessage(error.message());
  else
    setErrorMessage(error.message() + "\n" + error.additionalMessage());
}

MapQuickView* DisplayDimensions::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void DisplayDimensions::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

QString DisplayDimensions::errorMessage()
{
  return m_errorMessage;
}

void DisplayDimensions::setErrorMessage(const QString message)
{
  m_errorMessage = message;
  emit errorMessageChanged();
}

QString DisplayDimensions::dimensionLayerName() const
{
  return m_dimensionLayerName;
}

void DisplayDimensions::setDimensionLayerName(const QString name)
{
  m_dimensionLayerName = name;
  emit dimensionLayerNameChanged();
}

void DisplayDimensions::setDimensionLayerVisibility(const bool visibility)
{
  m_dimensionLayer->setVisible(visibility);
}

void DisplayDimensions::applyDefinitionExpression(const bool applied)
{
  if (applied)
    m_dimensionLayer->setDefinitionExpression("DIMLENGTH >= 450");
  else
    m_dimensionLayer->setDefinitionExpression("");
}

bool DisplayDimensions::mmpkLoaded()
{
  return m_mmpkLoaded;
}

void DisplayDimensions::setMMPKLoadStatus(const bool& status)
{
  m_mmpkLoaded = status;
  emit mmpkLoadStatusChanged();
}
