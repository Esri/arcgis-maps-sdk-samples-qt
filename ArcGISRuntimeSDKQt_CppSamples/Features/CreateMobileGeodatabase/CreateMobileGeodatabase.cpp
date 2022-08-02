// [WriteFile Name=CreateMobileGeodatabase, Category=Features]
// [Legal]
// Copyright 2022 Esri.

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

#include "CreateMobileGeodatabase.h"

#include "FeatureListModel.h"

#include "FeatureLayer.h"
#include "FieldDescription.h"
#include "FieldDescriptionListModel.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "Map.h"
#include "MapQuickView.h"
#include "TableDescription.h"

using namespace Esri::ArcGISRuntime;

CreateMobileGeodatabase::CreateMobileGeodatabase(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
}

CreateMobileGeodatabase::~CreateMobileGeodatabase() = default;

void CreateMobileGeodatabase::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateMobileGeodatabase>("Esri.Samples", 1, 0, "CreateMobileGeodatabaseSample");
}

MapQuickView* CreateMobileGeodatabase::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateMobileGeodatabase::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setViewpoint(Viewpoint(39.3238, -77.7332, 10'000));

  connect(m_mapView, &MapQuickView::mouseClicked, this, &CreateMobileGeodatabase::addFeature);

  m_featureListModel = new FeatureListModel(this);

  createGeodatabase();

  emit mapViewChanged();
}

void CreateMobileGeodatabase::createGeodatabase()
{
  QString path = m_tempDir.path() + "/LocationHistory.geodatabase";
  connect(Geodatabase::instance(), &Geodatabase::createCompleted, this, [this](QUuid, Geodatabase* geodatabaseResult)
  {
    m_gdb = geodatabaseResult;
    m_gdb->setParent(this);
    createTable();
  });
  Geodatabase::create(path);
}

void CreateMobileGeodatabase::createTable()
{
  auto tableDescription = new TableDescription("LocationHistory", SpatialReference::wgs84(), GeometryType::Point, this);
  tableDescription->setHasAttachments(false);
  tableDescription->setHasM(false);
  tableDescription->setHasZ(false);
  tableDescription->fieldDescriptions()->append(new FieldDescription("oid", FieldType::OID));
  tableDescription->fieldDescriptions()->append(new FieldDescription("collection_timestamp", FieldType::Date));

  connect(m_gdb, &Geodatabase::createTableCompleted, this, [this](QUuid, GeodatabaseFeatureTable* gdbFeatureTableResult)
  {
    m_featureTable = gdbFeatureTableResult;

    connect(m_featureTable, &FeatureTable::addFeatureCompleted, this, [this](QUuid, bool)
    {
      emit featureListModelChanged();
    });

    FeatureLayer* featureLayer = new FeatureLayer(m_featureTable, this);
    m_map->operationalLayers()->append(featureLayer);
  });
  m_gdb->createTable(tableDescription);
}

void CreateMobileGeodatabase::test()
{
  clearTable();
}

void CreateMobileGeodatabase::addFeature(QMouseEvent mouseEvent)
{
  const Point mousePoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
  QVariantMap attributes = {};
  attributes.insert("collection_timestamp", QDateTime::currentDateTime());
  Feature* feature = m_featureTable->createFeature(attributes, mousePoint, this);
  m_featureTable->addFeature(feature);
  m_featureListModel->addFeature(feature);
}

void CreateMobileGeodatabase::deleteFeatures()
{
  QueryParameters params;
  params.setWhereClause("1=1");
  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, this, [this](QUuid, FeatureQueryResult* rawQueryResult)
  {
    auto queryResults = std::unique_ptr<FeatureQueryResult>(rawQueryResult);
    auto resultIterator = queryResults->iterator();

    m_featureTable->deleteFeatures(resultIterator.features());

    m_featureTable->disconnect();
  });
  m_featureTable->queryFeatures(params);
}

void CreateMobileGeodatabase::clearTable()
{
  if (m_featureTable->numberOfFeatures() > 0)
    deleteFeatures();

  m_featureListModel->clear();
}

FeatureListModel* CreateMobileGeodatabase::featureListModel() const
{
  return m_featureListModel;
}
