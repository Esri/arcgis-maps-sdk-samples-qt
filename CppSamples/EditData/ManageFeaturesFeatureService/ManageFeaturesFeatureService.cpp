// [WriteFile Name=ManageFeaturesFeatureService, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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
#include "ManageFeaturesFeatureService.h"

// ArcGIS Maps SDK headers
#include "ArcGISFeature.h"
#include "AttributeListModel.h"
#include "Basemap.h"
#include "CalloutData.h"
#include "Envelope.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureEditResult.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "FeatureTableEditResult.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Point.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QMap>
#include <QMouseEvent>
#include <QScopeGuard>
#include <QString>
#include <QUrl>
#include <QUuid>
#include <QVariant>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

ManageFeaturesFeatureService::ManageFeaturesFeatureService(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ManageFeaturesFeatureService::~ManageFeaturesFeatureService() = default;

void ManageFeaturesFeatureService::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ManageFeaturesFeatureService>("Esri.Samples", 1, 0, "ManageFeaturesFeatureServiceSample");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void ManageFeaturesFeatureService::componentComplete()
{
  QQuickItem::componentComplete();

  // Find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a Map by passing in the Basemap
  m_map = new Map(BasemapStyle::ArcGISStreets, this);
  m_map->setInitialViewpoint(Viewpoint(Point(-10800000, 4500000, SpatialReference(102100)), 3e7));

  // Set map on the map view
  m_mapView->setMap(m_map);

  // Create a ServiceGeodatabase
  m_serviceGeodatabase =
    new ServiceGeodatabase(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/"), this);

  connect(m_serviceGeodatabase, &ServiceGeodatabase::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    if (loadStatus != LoadStatus::Loaded)
    {
      return;
    }
    // Get the first table(id 0) from the ServiceGeodatabase
    if (ServiceFeatureTable* table = m_serviceGeodatabase->table(0); table)
    {
      ServiceFeatureTable* featureTable = table;
      m_featureLayer = new FeatureLayer(featureTable, this);
      m_map->operationalLayers()->append(m_featureLayer);

      connectSignals();
    }
  });

  // Load the ServiceGeodatabase
  m_serviceGeodatabase->load();
}

void ManageFeaturesFeatureService::connectSignals()
{
  // Connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (m_operationMode == OperationMode::AddFeatures)
    {
      handleAddFeatureClick_(mouseEvent);
    }
    else if (m_operationMode == OperationMode::UpdateGeometry)
    {
      handleUpdateGeometryClick_(mouseEvent);
    }
    else
    {
      handleIdentifyFeatureClick_(mouseEvent);
    }
  });

  // Connect to the viewpoint changed signal on the MapQuickView
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    if (m_operationMode == OperationMode::AddFeatures)
    {
      return;
    }
    m_featureLayer->clearSelection();
    emit hideWindow();
    if (m_operationMode == OperationMode::UpdateGeometry)
    {
      m_featureSelected = false;
    }
  });
}

ManageFeaturesFeatureService::OperationMode ManageFeaturesFeatureService::operationMode() const
{
  return m_operationMode;
}

void ManageFeaturesFeatureService::setOperationMode(OperationMode mode)
{
  if (m_operationMode == mode)
  {
    return;
  }

  m_operationMode = mode;

  // Clear any existing selection when switching modes
  if (m_featureLayer)
  {
    m_featureLayer->clearSelection();
    m_featureSelected = false;
  }
  emit hideWindow();
  emit operationModeChanged();
}

QString ManageFeaturesFeatureService::featureType() const
{
  return m_featureType;
}

void ManageFeaturesFeatureService::deleteSelectedFeature()
{
  m_serviceGeodatabase->table(0)
    ->deleteFeatureAsync(m_selectedFeature)
    .then(this, [this]()
  {
    // Handle the completion of applyEditsAsync from the ServiceGeodatabase
    m_serviceGeodatabase->applyEditsAsync().then(this, [](const QList<FeatureTableEditResult*>& tableEditResults)
    {
      const QScopeGuard deleteAllOnExit{[&tableEditResults]()
      {
        qDeleteAll(tableEditResults);
      }};

      if (tableEditResults.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* const tableResult = tableEditResults.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* const featureEditResult = tableResult->editResults().first();
        // Check if there were errors, and if not, log the object ID
        if (featureEditResult && !featureEditResult->isCompletedWithErrors())
        {
          qDebug() << "Successfully deleted Object ID:" << featureEditResult->objectId();
        }
        else
        {
          qDebug() << "Apply edits error.";
        }
      }
    });
  });
}

void ManageFeaturesFeatureService::updateSelectedFeature(const QString& fieldVal)
{
  if (!m_selectedFeature)
  {
    return;
  }

  // Cancel any previously pending connection so at most one connection in queue
  disconnect(m_featureLoadStatusChangedConnection);

  m_featureLoadStatusChangedConnection = connect(m_selectedFeature, &ArcGISFeature::doneLoading, this, [this, fieldVal](const Error& error)
  {
    if (!error.isEmpty())
    {
      return;
    }
    if (!m_selectedFeature || !m_serviceGeodatabase)
    {
      return;
    }
    m_selectedFeature->attributes()->replaceAttribute("typdamage", fieldVal);
    if (ServiceFeatureTable* table = m_serviceGeodatabase->table(0); table)
    {
      table->updateFeatureAsync(m_selectedFeature)
        .then(this, [this]()
      {
        m_serviceGeodatabase->applyEditsAsync().then(this, [this](const QList<FeatureTableEditResult*>& tableEditResults)
        {
          onApplyEditsCompleted_(tableEditResults);
        });
      });
    }
  }, Qt::SingleShotConnection);

  m_selectedFeature->load();
}

void ManageFeaturesFeatureService::onIdentifyLayerCompleted_(const IdentifyLayerResult* identifyResult)
{
  if (!identifyResult)
  {
    return;
  }

  if (identifyResult->geoElements().empty())
  {
    return;
  }

  // Select the first item in the result
  Feature* const feature = static_cast<Feature*>(identifyResult->geoElements().at(0));
  m_featureLayer->selectFeature(feature);
  // Ensure layer isn't deleted with identify result
  m_featureLayer->setParent(this);

  // Query full feature with attributes (could also cast if already ArcGISFeature with attributes loaded)
  QueryParameters queryParams;
  const QString whereClause = QStringLiteral("objectid=") + feature->attributes()->attributeValue(QStringLiteral("objectid")).toString();
  queryParams.setWhereClause(whereClause);
  m_serviceGeodatabase->table(0)
    ->queryFeaturesAsync(queryParams)
    .then(this, [this](FeatureQueryResult* featureQueryResult)
  {
    if (!featureQueryResult || !featureQueryResult->iterator().hasNext())
    {
      return;
    }

    m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
    m_selectedFeature->setParent(this);
    m_featureType = m_selectedFeature->attributes()->attributeValue(QStringLiteral("typdamage")).toString();
    m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(m_featureType));
    m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());
    emit featureTypeChanged();
    emit featureSelected();
  });
}

void ManageFeaturesFeatureService::onApplyEditsCompleted_(const QList<FeatureTableEditResult*>& tableEditResults)
{
  const QScopeGuard deleteAllOnExit{[&tableEditResults]()
  {
    qDeleteAll(tableEditResults);
  }};

  if (tableEditResults.isEmpty())
  {
    qDebug() << "No table edit results";
    return;
  }

  // Get the first table result
  FeatureTableEditResult* const tableResult = tableEditResults.first();
  if (!tableResult->editResults().isEmpty())
  {
    // Get the first feature edit result from the table
    FeatureEditResult* const featureEditResult = tableResult->editResults().first();
    // Check if there were errors, and if not, log the object ID
    if (featureEditResult && !featureEditResult->isCompletedWithErrors())
    {
      qDebug() << "Successfully updated Object ID:" << featureEditResult->objectId();
    }
    else
    {
      qDebug() << "Apply edits error.";
    }
  }
}

void ManageFeaturesFeatureService::handleAddFeatureClick_(const QMouseEvent& mouseEvent)
{
  // Add features mode
  // Obtain the map point
  const double screenX = mouseEvent.position().x();
  const double screenY = mouseEvent.position().y();
  Point newPoint = m_mapView->screenToLocation(screenX, screenY);

  // Create the feature attributes
  QMap<QString, QVariant> featureAttributes;
  featureAttributes.insert("typdamage", "Minor");
  featureAttributes.insert("primcause", "Earthquake");

  // Get the first table from ServiceGeodatabase
  ServiceFeatureTable* featureTable = m_serviceGeodatabase->table(0);

  // Create a new feature and add it to the feature table
  Feature* feature = featureTable->createFeature(featureAttributes, newPoint, this);
  featureTable->addFeatureAsync(feature).then(this, [this]()
  {
    // If add feature was successful, call apply edits on ServiceGeodatabase
    m_serviceGeodatabase->applyEditsAsync().then(this, [](const QList<FeatureTableEditResult*>& tableEditResults)
    {
      const QScopeGuard deleteAllOnExit{[&tableEditResults]()
      {
        qDeleteAll(tableEditResults);
      }};

      if (tableEditResults.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* const tableResult = tableEditResults.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* const featureEditResult = tableResult->editResults().first();
        // Check if there were errors, and if not, log the new object ID
        if (!featureEditResult->isCompletedWithErrors())
        {
          qDebug() << "New Object ID is:" << featureEditResult->objectId();
        }
        else
        {
          qDebug() << "Apply edits error.";
        }
      }
    });
  });
}

void ManageFeaturesFeatureService::handleUpdateGeometryClick_(const QMouseEvent& mouseEvent)
{
  // Update geometry mode
  // Get the point from the mouse point
  Point mapPoint = m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y());

  // If a feature is already selected, move the selected feature to the new geometry
  if (m_featureSelected)
  {
    updateSelectedFeatureGeometry_(mapPoint);
  }
  else
  {
    selectFeatureForGeometryUpdate_(mouseEvent);
  }
}

void ManageFeaturesFeatureService::handleIdentifyFeatureClick_(const QMouseEvent& mouseEvent)
{
  // Identify and select feature mode (for delete and update attributes)
  // First clear the selection
  m_featureLayer->clearSelection();

  // Set the properties for qml
  emit hideWindow();

  // Call identify on the map view
  constexpr double tolerance = 5.0;
  constexpr int maxResults = 1;
  constexpr bool returnPopupsOnly = false;
  m_mapView->identifyLayerAsync(m_featureLayer, mouseEvent.position(), tolerance, returnPopupsOnly, maxResults)
    .then(this, [this](IdentifyLayerResult* rawIdentifyResult)
  {
    std::unique_ptr<IdentifyLayerResult> identifyResult(rawIdentifyResult);
    onIdentifyLayerCompleted_(identifyResult.get());
  });
}

void ManageFeaturesFeatureService::updateSelectedFeatureGeometry_(const Point& mapPoint)
{
  // Set the selected feature's geometry to the tapped map point
  m_selectedFeature->setGeometry(mapPoint);

  // Get the feature table that contains this feature
  ServiceFeatureTable* featureTable = static_cast<ServiceFeatureTable*>(m_selectedFeature->featureTable());

  // Update the feature table with the new feature
  featureTable->updateFeatureAsync(m_selectedFeature)
    .then(this, [this]()
  {
    // Once updateFeatureAsync is done, call applyEditsAsync on ServiceGeodatabase
    m_serviceGeodatabase->applyEditsAsync().then(this, [](const QList<FeatureTableEditResult*>& tableEditResults)
    {
      const QScopeGuard deleteAllOnExit{[&tableEditResults]()
      {
        qDeleteAll(tableEditResults);
      }};

      if (tableEditResults.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* const tableResult = tableEditResults.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* const featureEditResult = tableResult->editResults().first();
        // Check if there were errors, and if not, log the object ID
        if (featureEditResult && !featureEditResult->isCompletedWithErrors())
        {
          qDebug() << "Successfully updated geometry for Object ID:" << featureEditResult->objectId();
        }
        else
        {
          qDebug() << "Apply edits error.";
        }
      }
    });
  });

  // Reset the feature layer
  m_featureLayer->clearSelection();
  m_featureSelected = false;
}

void ManageFeaturesFeatureService::selectFeatureForGeometryUpdate_(const QMouseEvent& mouseEvent)
{
  // First clear the selection
  m_featureLayer->clearSelection();

  // Call identify on the map view
  constexpr double tolerance = 5.0;
  constexpr int maxResults = 1;
  constexpr bool returnPopupsOnly = false;
  m_mapView->identifyLayerAsync(m_featureLayer, mouseEvent.position(), tolerance, returnPopupsOnly, maxResults)
    .then(this, [this](IdentifyLayerResult* rawIdentifyResult)
  {
    std::unique_ptr<IdentifyLayerResult> identifyResult(rawIdentifyResult);
    if (!identifyResult)
    {
      return;
    }

    if (identifyResult->geoElements().isEmpty())
    {
      return;
    }

    Feature* const feature = static_cast<Feature*>(identifyResult->geoElements().at(0));
    QueryParameters query;
    query.setObjectIds(QList<qint64>{feature->attributes()->attributeValue(QStringLiteral("objectid")).toLongLong()});
    m_featureLayer->selectFeaturesAsync(query, SelectionMode::New)
      .then(this, [](FeatureQueryResult* rawResult)
    {
      std::unique_ptr<FeatureQueryResult>{rawResult};
    });

    m_selectedFeature = static_cast<ArcGISFeature*>(feature);
    m_selectedFeature->setParent(this);
    m_featureSelected = true;
  });
}
