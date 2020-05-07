// [WriteFile Name=FeatureLayerGeodatabase, Category=Features]
// [Legal]
// Copyright 2016 Esri.

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

#include "FeatureLayerGeodatabase.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "GeodatabaseFeatureTable.h"
#include "Geodatabase.h"
#include "ArcGISVectorTiledLayer.h"
#include "SpatialReference.h"
#include "Point.h"
#include "Viewpoint.h"

#include <QDir>
#include <QtCore/qglobal.h>
#include <QUrl>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
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
} // namespace

FeatureLayerGeodatabase::FeatureLayerGeodatabase(QQuickItem* parent) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data/")
{
}

FeatureLayerGeodatabase::~FeatureLayerGeodatabase() = default;

void FeatureLayerGeodatabase::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerGeodatabase>("Esri.Samples", 1, 0, "FeatureLayerGeodatabaseSample");
}

void FeatureLayerGeodatabase::logError(Esri::ArcGISRuntime::Error error)
{
  setErrorMessage(QString("%1: %2").arg(error.message(), error.additionalMessage()));
}

QString FeatureLayerGeodatabase::errorMessage() const
{
  return m_errorMsg;
}

void FeatureLayerGeodatabase::setErrorMessage(const QString& msg)
{
  m_errorMsg = msg;
  qDebug() << m_errorMsg;
  emit errorMessageChanged();
}

void FeatureLayerGeodatabase::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  //! [FeatureLayer Geodatabase add basemap]
  // Create a map using a local vector tile package
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl::fromLocalFile(m_dataPath + "vtpk/LosAngeles.vtpk"), this);
  Basemap* basemap = new Basemap(vectorTiledLayer, this);
  m_map = new Map(basemap, this);
  //! [FeatureLayer Geodatabase add basemap]

  connect(m_map, &Map::errorOccurred, this, &FeatureLayerGeodatabase::logError);
  connect(m_mapView, &MapQuickView::errorOccurred, this, &FeatureLayerGeodatabase::logError);

  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(Point(-13214155, 4040194, SpatialReference(3857)), 35e4));

  // Set map to map view
  m_mapView->setMap(m_map);

  //! [FeatureLayer Geodatabase create gdb]
  // create the geodatabase using a local file path
  m_geodatabase = new Geodatabase(m_dataPath + "geodatabase/LA_Trails.geodatabase", this);
  //! [FeatureLayer Geodatabase create gdb]

  connect(m_geodatabase, &Geodatabase::errorOccurred, this, &FeatureLayerGeodatabase::logError);

  // connect to doneLoading signal of geodatabase to access feature tables
  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this](Error error)
  {
    if (error.isEmpty())
    {
      //! [FeatureLayer Geodatabase create feature layer]
      // access the feature table by name
      GeodatabaseFeatureTable* featureTable = m_geodatabase->geodatabaseFeatureTable("Trailheads");

      // create a feature layer from the feature table
      FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
      //! [FeatureLayer Geodatabase create feature layer]

      //! [FeatureLayer Geodatabase add to map]
      // add the feature layer to the map
      m_map->operationalLayers()->append(featureLayer);
      //! [FeatureLayer Geodatabase add to map]
    }
  });

  // load the geodatabase
  m_geodatabase->load();
}
