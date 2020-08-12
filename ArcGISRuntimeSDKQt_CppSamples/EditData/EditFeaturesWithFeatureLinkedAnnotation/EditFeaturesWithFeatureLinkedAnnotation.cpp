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
#include "GeometryEngine.h"
#include "PolylineBuilder.h"
#include "PartCollection.h"

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

  m_geodatabase = new Geodatabase(dataPath, this);
  connect(m_geodatabase, &Geodatabase::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    GeodatabaseFeatureTable* pointFeatureTable = m_geodatabase->geodatabaseFeatureTable("Loudoun_Address_Points_1");
    GeodatabaseFeatureTable* parcelLinesFeatureTable = m_geodatabase->geodatabaseFeatureTable("ParcelLines_1");

    GeodatabaseFeatureTable* addressPointsAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("Loudoun_Address_PointsAnno_1");
    GeodatabaseFeatureTable* parcelLinesAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("ParcelLinesAnno_1");

    m_pointFeatureLayer = new FeatureLayer(pointFeatureTable, this);
    m_parcelLinesFeatureLayer = new FeatureLayer(parcelLinesFeatureTable, this);

    m_addressPointsAnnotationLayer = new AnnotationLayer(addressPointsAnnotationTable, this);
    m_parcelLinesAnnotationLayer = new AnnotationLayer(parcelLinesAnnotationTable, this);

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

    if (m_selectedFeature) {
      const Point clickedPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

      movePolylineVertex(clickedPoint);
    }
    else
    {
      m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 10, false);
    }
  });

  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, [this](QUuid, QList<IdentifyLayerResult*> identifyResults)
  {
    if(identifyResults.isEmpty())
      return;

    for (auto identifyResult : identifyResults)
    {
      if (!identifyResult->geoElements().empty())
      {
        FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(identifyResult->layerContent());

        if (featureLayer)
        {
          m_selectedFeature = dynamic_cast<Feature*>(identifyResult->geoElements().at(0));

          // Prevent the feature from being deleted along with the identifyResult.
          m_selectedFeature->setParent(this);

          const GeometryType selectedFeatureGeomType = m_selectedFeature->geometry().geometryType();

          if (selectedFeatureGeomType == GeometryType::Polyline)
          {
            const Geometry geom = m_selectedFeature->geometry();
            const PolylineBuilder* polylineBuilder = new PolylineBuilder(geom, this);

            if (polylineBuilder->toPolyline().parts().part(0).segmentCount() > 1)
            {
              qDebug() << "Only Straight lines";
              clearSelection();
              delete m_selectedFeature;
              m_selectedFeature = nullptr;
              return;
            }
          }
          else if (selectedFeatureGeomType == GeometryType::Point)
          {
            m_addressAndStreetText.clear();

            const QString addressText = m_selectedFeature->attributes()->attributeValue("AD_ADDRESS").toString();
            m_addressAndStreetText.append(addressText);
            const QString streetNameText = m_selectedFeature->attributes()->attributeValue("ST_STR_NAM").toString();
            m_addressAndStreetText.append(streetNameText);

            emit addressAndStreetTextChanged();
          }
          else
          {
            qDebug() << "Unexpected geometry type selected";
          }

          featureLayer->selectFeature(m_selectedFeature);
        }
        else
        {
          qDebug() << "Failed cast";
        }
      }
    }
  });

  connect(m_map, &Map::doneLoading, this, [this] (Error e)
  {
    qDebug() << m_map->initialViewpoint().toJson();
  });

  emit mapViewChanged();
}

void EditFeaturesWithFeatureLinkedAnnotation::clearSelection()
{
  for (auto layer : *m_map->operationalLayers())
  {
    FeatureLayer* featureLayer = static_cast<FeatureLayer*>(layer);
    featureLayer->clearSelection();
  }
}

void EditFeaturesWithFeatureLinkedAnnotation::movePolylineVertex(Point mapPoint)
{
  const Polyline geom = m_selectedFeature->geometry();
  const Point workingPoint = GeometryEngine::project(mapPoint, geom.spatialReference());
  if (geom.geometryType() == GeometryType::Polyline)
  {
    const ProximityResult nearestVertex = GeometryEngine::nearestVertex(geom, workingPoint);

    PolylineBuilder* polylineBuilder = new PolylineBuilder(geom, this);

    Part* part = polylineBuilder->parts()->part(nearestVertex.partIndex());

    part->removePoint(nearestVertex.pointIndex());
    part->addPoint(workingPoint);
    m_selectedFeature->setGeometry(polylineBuilder->toGeometry());
    m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);

    clearSelection();
    delete m_selectedFeature;
    m_selectedFeature = nullptr;
  }
  else
  {
    m_selectedFeature->setGeometry(mapPoint);
    m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
    clearSelection();
    delete m_selectedFeature;
    m_selectedFeature = nullptr;
  }
}

void EditFeaturesWithFeatureLinkedAnnotation::updateSelectedFeature(const QString& address, const QString& streetName)
{
  if (!m_selectedFeature)
    return;

  m_selectedFeature->attributes()->replaceAttribute("AD_ADDRESS", address);
  m_selectedFeature->attributes()->replaceAttribute("ST_STR_NAM", streetName);
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
}
