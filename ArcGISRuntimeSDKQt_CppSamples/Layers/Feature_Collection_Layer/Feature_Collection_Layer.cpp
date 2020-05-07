// [WriteFile Name=Feature_Collection_Layer, Category=Layers]
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

#include "Feature_Collection_Layer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Viewpoint.h"
#include "Envelope.h"
#include "Basemap.h"
#include "Feature.h"
#include "FeatureCollection.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollectionTable.h"
#include "Field.h"
#include "Point.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "SpatialReference.h"
#include "SimpleRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"

using namespace Esri::ArcGISRuntime;

Feature_Collection_Layer::Feature_Collection_Layer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Feature_Collection_Layer::~Feature_Collection_Layer() = default;

void Feature_Collection_Layer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Feature_Collection_Layer>("Esri.Samples", 1, 0, "Feature_Collection_LayerSample");
}

void Feature_Collection_Layer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::oceans(this), this);
  m_map->setInitialViewpoint(Viewpoint(Envelope(-8917856.590171767, 903277.583136797, -8800611.655131537, 1100327.8941287803, SpatialReference(102100))));

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create FeatureCollection and FeatureCollectionLayer
  m_featureCollection = new FeatureCollection(this);
  m_featureCollectionLayer = new FeatureCollectionLayer(m_featureCollection, this);

  // Add the Layer to the Map
  m_map->operationalLayers()->append(m_featureCollectionLayer);

  // Create the FeatureCollectionTables
  createPointTable();
  createPolylineTable();
  createPolygonTable();
}

void Feature_Collection_Layer::createPointTable()
{
  // Define the schema
  QList<Field> pointFields;
  Field placeField = Field::createText("Place", "Place Name", 50);
  pointFields.append(placeField);

  // Create the table
  FeatureCollectionTable* pointsTable =  new FeatureCollectionTable(pointFields, GeometryType::Point, SpatialReference(4326), this);

  // Define the renderer
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 18.0, this);
  SimpleRenderer* renderer = new SimpleRenderer(sms, this);
  pointsTable->setRenderer(renderer);

  // Add the table to the collection
  m_featureCollection->tables()->append(pointsTable);

  // Add features to the table
  Point pt1(-79.497238, 8.849289, SpatialReference::wgs84());
  addPointToTable("Place", "Current location", pt1, pointsTable);
}

void Feature_Collection_Layer::createPolylineTable()
{
  // Define the schema
  QList<Field> lineFields;
  Field boundaryField = Field::createText("Boundary", "Boundary Name", 50);
  lineFields.append(boundaryField);

  // Create the table
  FeatureCollectionTable* linesTable =  new FeatureCollectionTable(lineFields, GeometryType::Polyline, SpatialReference(4326), this);

  // Define the renderer
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("green"), 3.0, this);
  SimpleRenderer* renderer = new SimpleRenderer(sls, this);
  linesTable->setRenderer(renderer);

  // Add the table to the collection
  m_featureCollection->tables()->append(linesTable);

  // Add features to the table
  Point pt1(-79.497238, 8.849289, SpatialReference::wgs84());
  Point pt2(-80.035568, 9.432302, SpatialReference::wgs84());
  QList<Point> points { pt1, pt2 };
  addPolylineToTable("Boundary", "AManAPlanACanalPanama", points, linesTable);
}

void Feature_Collection_Layer::createPolygonTable()
{
  // Define the schema
  QList<Field> polyFields;
  Field areaField = Field::createText("Area", "Area Name", 50);
  polyFields.append(areaField);

  // Create the table
  FeatureCollectionTable* polysTable =  new FeatureCollectionTable(polyFields, GeometryType::Polygon, SpatialReference(4326), this);

  // Define the renderer
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("blue"), 2.0, this);
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::DiagonalCross, QColor("cyan"), sls, this);
  SimpleRenderer* renderer = new SimpleRenderer(sfs, this);
  polysTable->setRenderer(renderer);

  // Add the table to the collection
  m_featureCollection->tables()->append(polysTable);

  // Add features to the table
  const Point pt1(-79.497238, 8.849289, SpatialReference::wgs84());
  const Point pt2(-79.337936, 8.638903, SpatialReference::wgs84());
  const Point pt3(-79.11409, 8.895422, SpatialReference::wgs84());
  const QList<Point> points{ pt1, pt2, pt3 };
  addPolygonToTable("Area", "Restricted area", points, polysTable);
}

void Feature_Collection_Layer::addPointToTable(QString attrName, QString attrValue, Point point, FeatureCollectionTable* table)
{
  // Create the new feature
  Feature* feature = table->createFeature(this);
  feature->attributes()->replaceAttribute(attrName, attrValue);
  feature->setGeometry(point);

  // Add the feature to the table
  table->addFeature(feature);
}

void Feature_Collection_Layer::addPolylineToTable(QString attrName, QString attrValue, QList<Point> points, FeatureCollectionTable* table)
{
  // Create the new feature
  Feature* feature = table->createFeature(this);
  feature->attributes()->replaceAttribute(attrName, attrValue);
  PolylineBuilder* builder = new PolylineBuilder(SpatialReference(4326), this);
  for (const Point& pt : points)
  {
    builder->addPoint(pt);
  }
  feature->setGeometry(builder->toGeometry());

  // Add the feature to the table
  table->addFeature(feature);
}

void Feature_Collection_Layer::addPolygonToTable(QString attrName, QString attrValue, QList<Point> points, FeatureCollectionTable* table)
{
  // Create the new feature
  Feature* feature = table->createFeature(this);
  feature->attributes()->replaceAttribute(attrName, attrValue);
  PolygonBuilder* builder = new PolygonBuilder(SpatialReference(4326), this);
  for (const Point& pt : points)
  {
    builder->addPoint(pt);
  }
  feature->setGeometry(builder->toGeometry());

  // Add the feature to the table
  table->addFeature(feature);
}
