// [WriteFile Name=CreateMobileGeodatabase, Category=Features]
// [Legal]
// Copyright 2022 Esri.
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
#include "CreateMobileGeodatabase.h"
#include "FeatureListModel.h"

// ArcGIS Maps SDK headers
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "FieldDescription.h"
#include "FieldDescriptionListModel.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "QueryParameters.h"
#include "ServiceTypes.h"
#include "SpatialReference.h"
#include "TableDescription.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

CreateMobileGeodatabase::CreateMobileGeodatabase(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  // The FeatureListModel is a helper class created specifically for this sample to display all features in the table view
  m_featureListModel = new FeatureListModel(this);
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
  m_mapView->setViewpointAsync(Viewpoint(39.3238, -77.7332, 10'000));

  connect(m_mapView, &MapQuickView::mouseClicked, this, &CreateMobileGeodatabase::addFeature);

  emit mapViewChanged();
}

// Create a Geodatabase in an empty directory
void CreateMobileGeodatabase::createGeodatabase()
{
  m_gdbFilePath = QString{m_tempDir.path() + "/LocationHistory_%1.geodatabase"}.arg(QDateTime::currentSecsSinceEpoch() % 1000);
  emit gdbFilePathChanged();

  // We cannot overwrite an existing geodatabase
  if (QFile::exists(m_gdbFilePath))
    return;

  // Use static Geodatabase::createAsync with an empty file path
  Geodatabase::createAsync(m_gdbFilePath, this).then(this, [this](Geodatabase* geodatabase)
  {
    m_gdb = geodatabase;
    createTable();
  });
}

// Create a GeodatabaseFeatureTable from the new Geodatabase with a TableDescription
void CreateMobileGeodatabase::createTable()
{
  m_gdbOpen = true;
  emit gdbOpenChanged();

  // Create a TableDescription to define the GeodatabaseFeatureTable's attributes and fields
  TableDescription* tableDescription = new TableDescription("LocationHistory", SpatialReference::wgs84(), GeometryType::Point, this);
  tableDescription->setHasAttachments(false);
  tableDescription->setHasM(false);
  tableDescription->setHasZ(false);
  tableDescription->fieldDescriptions()->append(new FieldDescription("oid", FieldType::OID));
  tableDescription->fieldDescriptions()->append(new FieldDescription("collection_timestamp", FieldType::Date));

  m_gdb->createTableAsync(tableDescription, this).then(this, [this](GeodatabaseFeatureTable* gdbFeatureTableResult)
  {
    m_featureTable = gdbFeatureTableResult;

    FeatureLayer* featureLayer = new FeatureLayer(m_featureTable, this);
    m_map->operationalLayers()->append(featureLayer);
  });
}

// Close the Geodatabase so that it can be safely shared
void CreateMobileGeodatabase::closeGdb()
{
  if (!m_gdb)
    return;

  m_gdb->close();
  m_gdbOpen = false;

  m_map->operationalLayers()->clear();
  m_featureListModel->clear();

  m_featureCount = 0;

  emit gdbOpenChanged();
  emit featureCountChanged();
}

void CreateMobileGeodatabase::addFeature(QMouseEvent& mouseEvent)
{
  if (!m_featureTable)
    return;

  const Point mousePoint = m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y());
  QVariantMap attributes = {};
  attributes.insert("collection_timestamp", QDateTime::currentDateTime());
  Feature* feature = m_featureTable->createFeature(attributes, mousePoint, this);

  m_featureTable->addFeatureAsync(feature).then(this, [this, feature]()
  {
    m_featureListModel->addFeature(feature);
    emit featureListModelChanged();

    m_featureCount = static_cast<int>(m_featureTable->numberOfFeatures());
    emit featureCountChanged();
  });
}

void CreateMobileGeodatabase::deleteFeatures()
{
  QueryParameters params;
  params.setWhereClause("1=1");

  m_featureTable->queryFeaturesAsync(params).then(this, [this](FeatureQueryResult* rawQueryResult)
  {
    // Cast the FeatureQueryResult to a unique pointer to delete it when it goes out of scope
    auto queryResults = std::unique_ptr<FeatureQueryResult>(rawQueryResult);
    FeatureIterator resultIterator = queryResults->iterator();

    auto deleteFeaturesFuture = m_featureTable->deleteFeaturesAsync(resultIterator.features());
    Q_UNUSED(deleteFeaturesFuture)
    m_featureCount = 0;
    emit featureCountChanged();
  });
}

void CreateMobileGeodatabase::clearFeatures()
{
  if (m_featureTable->numberOfFeatures() > 0)
    deleteFeatures();

  m_featureListModel->clear();
}

FeatureListModel* CreateMobileGeodatabase::featureListModel() const
{
  return m_featureListModel;
}

QString CreateMobileGeodatabase::gdbFilePath() const
{
  return m_gdbFilePath;
}

int CreateMobileGeodatabase::featureCount() const
{
  return m_featureCount;
}

bool CreateMobileGeodatabase::gdbOpen() const
{
  return m_gdbOpen;
}
