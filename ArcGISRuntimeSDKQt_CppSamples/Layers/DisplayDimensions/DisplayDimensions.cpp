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

  // Make connections between the mmpk's doneLoading and errorOccurred signal and the addMapToMapView and handleError methods respectively.
  connect(m_mmpk, &MobileMapPackage::doneLoading, this, &DisplayDimensions::addMapToMapView);
  connect(m_mmpk, &MobileMapPackage::errorOccurred, this, &DisplayDimensions::handleError);

  // Load the mmpk.
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
  // If no errors have occurred, the mmpk is loaded, and there is only one map in the mmpk, the map
  // in the mmpk can be assigned to the MapView. Any errors will be managed by the handleError method.
  if (error.isEmpty() && m_mmpk->loadStatus() == LoadStatus::Loaded && m_mmpk->maps().count() > 0)
  {
    // Enable the checkboxes.
    setdimensionsAvailable(true);

    // Assign the map in the mmpk to m_map.
    m_map = m_mmpk->maps().at(0);

    // Set the minimum scale to prevent zooming out too far.
    m_map->setMinScale(35000);

    // Check to ensure that m_mapView has been initialised properly.
    if (m_mapView != nullptr)
    {
      // Set m_map as the MapView's map.
      m_mapView->setMap(m_map);
    }

    // From the map's layers, find the Dimension Layer.
    findDimensionLayer();
  }
  else
  {
    // If the map hasn't loaded or an error has occurred, disable the checkboxes in the UI.
    setdimensionsAvailable(false);
  }
}

void DisplayDimensions::findDimensionLayer()
{
  LayerListModel* layers = m_mapView->map()->operationalLayers();

  for (Layer* layer : *layers)
  {
    if (layer->layerType() == LayerType::DimensionLayer)
    {
      // The current layer, which is the DimensionLayer, has type Layer*. Ths needs to be converted to a DimensionLayer*.
      m_dimensionLayer = static_cast<DimensionLayer*>(layer);
      // Use the name of the Dimension Layer to define the title of the UI element.
      setDimensionLayerName(m_dimensionLayer->name());
      // There is only one Dimension Layer, so we can break out of the loop.
      break;
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

  if (m_map)
    m_mapView->setMap(m_map);

  emit mapViewChanged();
}

QString DisplayDimensions::errorMessage()
{
  return m_errorMessage;
}

void DisplayDimensions::setErrorMessage(const QString& message)
{
  m_errorMessage = message;
  emit errorMessageChanged();
}

QString DisplayDimensions::dimensionLayerName() const
{
  return m_dimensionLayerName;
}

void DisplayDimensions::setDimensionLayerName(const QString& name)
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

bool DisplayDimensions::dimensionsAvailable()
{
  return m_dimensionsAvailable;
}

void DisplayDimensions::setdimensionsAvailable(const bool status)
{
  m_dimensionsAvailable = status;
  emit dimensionsAvailableChanged();
}
