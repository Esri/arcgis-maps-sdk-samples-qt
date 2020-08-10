// [WriteFile Name=EditFeaturesWithFeatureLinkedAnnotation, Category=EditData]
// [Legal]
// Copyright 2020 Esri.

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

#include "EditFeaturesWithFeatureLinkedAnnotation.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Geodatabase.h"
#include "AnnotationLayer.h"
#include "FeatureLayer.h"
#include "AnnotationLayer.h"
#include "GeodatabaseFeatureTable.h"

// Qt headers
#include <QString>
#include <QFile>
#include <QtCore/qglobal.h>
#include <QTimer>

#include <memory>

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


using namespace Esri::ArcGISRuntime;

EditFeaturesWithFeatureLinkedAnnotation::EditFeaturesWithFeatureLinkedAnnotation(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapType::LightGrayCanvasVector, 39.0204, -77.4159, 18, this))
{
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/loudoun_anno.geodatabase";
  qDebug() << dataPath;
  m_geodatabase = new Geodatabase(dataPath, this);
  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

//    qDebug() << "done loading";

    GeodatabaseFeatureTable* pointFeatureTable = m_geodatabase->geodatabaseFeatureTable("Loudoun_Address_Points_1");
    GeodatabaseFeatureTable* parcelLinesFeatureTable = m_geodatabase->geodatabaseFeatureTable("ParcelLines_1");

    GeodatabaseFeatureTable* addressPointsAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("Loudoun_Address_PointsAnno_1");
    GeodatabaseFeatureTable* parcelLinesAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("ParcelLinesAnno_1");

    m_pointFeatureLayer = new FeatureLayer(pointFeatureTable, this);
    m_parcelLinesFeatureLayer = new FeatureLayer(parcelLinesFeatureTable, this);

    m_addressPointsAnnotationLayer = new AnnotationLayer(addressPointsAnnotationTable, this);
    m_parcelLinesAnnotationLayer = new AnnotationLayer(parcelLinesAnnotationTable, this);

    connect(m_pointFeatureLayer, &FeatureLayer::doneLoading, this, [] (Error e)
    {
      if (!e.isEmpty())
      {
        qDebug() << "m_pointFeatureLayer error msg: "<< e.message();
        return;
      }

      qDebug() << "m_pointFeatureLayer loaded";
    });
    connect(m_parcelLinesFeatureLayer, &FeatureLayer::doneLoading, this, [] (Error e)
    {
      if (!e.isEmpty())
      {
        qDebug() << "m_parcelLinesFeatureLayer error msg: "<< e.message();
        return;
      }

      qDebug() << "m_parcelLinesFeatureLayer loaded";
    });
    connect(m_addressPointsAnnotationLayer, &FeatureLayer::doneLoading, this, [] (Error e)
    {
      if (!e.isEmpty())
      {
        qDebug() << "m_addressPointsAnnotationLayer error msg: "<< e.message();
        return;
      }

      qDebug() << "m_addressPointsAnnotationLayer loaded";
    });
    connect(m_parcelLinesAnnotationLayer, &FeatureLayer::doneLoading, this, [] (Error e)
    {
      if (!e.isEmpty())
      {
        qDebug() << "m_parcelLinesAnnotationLayer error msg: "<< e.message();
        return;
      }

      qDebug() << "m_parcelLinesAnnotationLayer loaded";
    });

    m_map->operationalLayers()->append(m_pointFeatureLayer);
    m_map->operationalLayers()->append(m_parcelLinesFeatureLayer);

    m_map->operationalLayers()->append(m_addressPointsAnnotationLayer);
    m_map->operationalLayers()->append(m_parcelLinesAnnotationLayer);
  });

  m_geodatabase->load();
}

EditFeaturesWithFeatureLinkedAnnotation::~EditFeaturesWithFeatureLinkedAnnotation() = default;

void EditFeaturesWithFeatureLinkedAnnotation::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<EditFeaturesWithFeatureLinkedAnnotation>("Esri.Samples", 1, 0, "EditFeaturesWithFeatureLinkedAnnotationSample");
}

MapQuickView* EditFeaturesWithFeatureLinkedAnnotation::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void EditFeaturesWithFeatureLinkedAnnotation::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent mouseEvent)
  {
    clearSelection();
    // call identify on the map view
//    m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
    m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 10, false);
  });

  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, [this](QUuid, QList<IdentifyLayerResult*> identifyResults)
  {
    if(identifyResults.isEmpty())
      return;

    for (auto identifyResult : identifyResults)
    {


      if (!identifyResult->geoElements().empty())
      {
        // select the item in the result
//        qDebug() << identifyResult->layerContent()->name();

        auto testCast = static_cast<FeatureLayer*>(identifyResult->layerContent());


        if (testCast)
        {
          qDebug() << testCast->name() << "Clearing selection";

          testCast->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
        } else {
          qDebug() << "Failed cast";
        }
//        m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
        //          emit  featureSelected();

        //          // Update the parent so the featureLayer is not deleted when the identifyResult is deleted.
        //          m_featureLayer->setParent(this);

        //          // obtain the selected feature with attributes
//        QueryParameters queryParams;
//        QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
//        queryParams.setWhereClause(whereClause);

        //          auto featTable = m_serviceGeodatabase->connectedTables()[0];
        //          featTable->queryFeatures(queryParams);
        //          m_featureTable = m_serviceGeodatabase->connectedTables()[0];
//        m_featureTable->queryFeatures(queryParams);
      }
    }
  });

  emit mapViewChanged();
}

void EditFeaturesWithFeatureLinkedAnnotation::clearSelection()
{
  for (auto layer : *m_map->operationalLayers())
  {
    auto featureLayer = static_cast<FeatureLayer*>(layer);
    featureLayer->clearSelection();
  }
}
