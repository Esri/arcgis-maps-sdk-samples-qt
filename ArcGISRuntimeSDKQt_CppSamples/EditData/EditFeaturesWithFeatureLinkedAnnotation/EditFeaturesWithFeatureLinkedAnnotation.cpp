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

const QString EditFeaturesWithFeatureLinkedAnnotation::s_ad_address = QStringLiteral("AD_ADDRESS");
const QString EditFeaturesWithFeatureLinkedAnnotation::s_st_str_nam = QStringLiteral("ST_STR_NAM");

EditFeaturesWithFeatureLinkedAnnotation::EditFeaturesWithFeatureLinkedAnnotation(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapType::LightGrayCanvasVector, 39.0204, -77.4159, 18, this))
{
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/loudoun_anno.geodatabase";

  m_geodatabase = new Geodatabase(dataPath, this);
  connect(m_geodatabase, &Geodatabase::doneLoading, this, &EditFeaturesWithFeatureLinkedAnnotation::onGeodatabaseDoneLoading);

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

  connect(m_mapView, &MapQuickView::mouseClicked, this, &EditFeaturesWithFeatureLinkedAnnotation::onMouseClicked);
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, &EditFeaturesWithFeatureLinkedAnnotation::onIdentifyLayersCompleted);

  emit mapViewChanged();
}

void EditFeaturesWithFeatureLinkedAnnotation::onGeodatabaseDoneLoading(Error error)
{
  if (!error.isEmpty())
    return;

  GeodatabaseFeatureTable* pointFeatureTable = m_geodatabase->geodatabaseFeatureTable("Loudoun_Address_Points_1");
  GeodatabaseFeatureTable* parcelLinesFeatureTable = m_geodatabase->geodatabaseFeatureTable("ParcelLines_1");

  GeodatabaseFeatureTable* addressPointsAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("Loudoun_Address_PointsAnno_1");
  GeodatabaseFeatureTable* parcelLinesAnnotationTable = m_geodatabase->geodatabaseAnnotationTable("ParcelLinesAnno_1");

  // create feature layers from tables in the geodatabase
  m_pointFeatureLayer = new FeatureLayer(pointFeatureTable, this);
  m_parcelLinesFeatureLayer = new FeatureLayer(parcelLinesFeatureTable, this);

  // create annotation layers from tables in the geodatabase
  m_addressPointsAnnotationLayer = new AnnotationLayer(addressPointsAnnotationTable, this);
  m_parcelLinesAnnotationLayer = new AnnotationLayer(parcelLinesAnnotationTable, this);

  // add feature layers to map
  m_map->operationalLayers()->append(m_pointFeatureLayer);
  m_map->operationalLayers()->append(m_parcelLinesFeatureLayer);

  // add annotation layers to map
  m_map->operationalLayers()->append(m_addressPointsAnnotationLayer);
  m_map->operationalLayers()->append(m_parcelLinesAnnotationLayer);
}

void EditFeaturesWithFeatureLinkedAnnotation::onMouseClicked(QMouseEvent mouseEvent)
{
  clearSelection();

  if (m_selectedFeature)
  {
    // move feature to clicked locaiton if already selected
    const Point clickedPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    moveFeature(clickedPoint);
  }
  else
  {
    // identify and select feature
    m_mapView->identifyLayers(mouseEvent.x(), mouseEvent.y(), 10, false);
  }
}

void EditFeaturesWithFeatureLinkedAnnotation::onIdentifyLayersCompleted(QUuid, const QList<IdentifyLayerResult*>& identifyResults)
{
  for (IdentifyLayerResult* identifyResult : identifyResults)
  {
    if (!identifyResult->geoElements().isEmpty())
    {
      // only perform selections on Feature Layers
      FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(identifyResult->layerContent());

      // if cast was succesful select features.
      if (featureLayer)
      {
        m_selectedFeature = static_cast<Feature*>(identifyResult->geoElements().at(0));

        const GeometryType selectedFeatureGeomType = m_selectedFeature->geometry().geometryType();

        if (selectedFeatureGeomType == GeometryType::Polyline)
        {
          const Geometry geom = m_selectedFeature->geometry();
          const PolylineBuilder* polylineBuilder = new PolylineBuilder(geom, this);

          // if the selected feature is a polyline with any part containing more than one segment
          // (i.e. a curve) do not select it
          if (polylineBuilder->toPolyline().parts().part(0).segmentCount() > 1)
          {
            clearSelection();
            delete m_selectedFeature;
            m_selectedFeature = nullptr;
            return;
          }
        }
        else if (selectedFeatureGeomType == GeometryType::Point)
        {
          // clear string list with attribute values.
          m_addressAndStreetText.clear();

          // update QML text fields with the attributes for the selected point feature
          const QString addressText = m_selectedFeature->attributes()->attributeValue(s_ad_address).toString();
          m_addressAndStreetText.append(addressText);
          const QString streetNameText = m_selectedFeature->attributes()->attributeValue(s_st_str_nam).toString();
          m_addressAndStreetText.append(streetNameText);

          emit addressAndStreetTextChanged();
        }
        featureLayer->selectFeature(m_selectedFeature);
      }
    }
  }
}

void EditFeaturesWithFeatureLinkedAnnotation::clearSelection()
{
  for (Layer* layer : *m_map->operationalLayers())
  {
    FeatureLayer* featureLayer = dynamic_cast<FeatureLayer*>(layer);
    if (featureLayer)
      featureLayer->clearSelection();
  }
}

void EditFeaturesWithFeatureLinkedAnnotation::moveFeature(Point mapPoint)
{
  const Geometry geom = m_selectedFeature->geometry();

  const Point projectedMapPoint = GeometryEngine::project(mapPoint, geom.spatialReference());
  if (geom.geometryType() == GeometryType::Polyline)
  {
    // get nearest vertex to the map point on the selected polyline
    const ProximityResult nearestVertex = GeometryEngine::nearestVertex(geom, projectedMapPoint);
    PolylineBuilder* polylineBuilder = new PolylineBuilder(geom, this);

    // get part of polyline nearest to map point
    Part* part = polylineBuilder->parts()->part(nearestVertex.partIndex());

    // remove nearest point to map point
    part->removePoint(nearestVertex.pointIndex());

    // add the map point as a new point
    part->addPoint(projectedMapPoint);

    // update the selected feature with the new geometry
    m_selectedFeature->setGeometry(polylineBuilder->toGeometry());
    m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);

    clearSelection();
    delete m_selectedFeature;
    m_selectedFeature = nullptr;
  }
  else if (geom.geometryType() == GeometryType::Point)
  {
    // if the selected feature is a point, change the geometry with the map point
    m_selectedFeature->setGeometry(mapPoint);

    // update the selected feature with the new geometry
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

  // update the two attirbutes with the inputed text.
  m_selectedFeature->attributes()->replaceAttribute(s_ad_address, address);
  m_selectedFeature->attributes()->replaceAttribute(s_st_str_nam, streetName);
  m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
}
