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
#include <QString>
#include <QUrl>
#include <QUuid>
#include <QVariant>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

namespace
{
  // Convenience RAII struct that deletes all pointers in given container.
  struct FeatureTableResultLock
  {
    FeatureTableResultLock(const QList<FeatureTableEditResult*>& list) :
      results(list)
    {
    }

    ~FeatureTableResultLock()
    {
      qDeleteAll(results);
    }

    const QList<FeatureTableEditResult*>& results;
  };
} // namespace

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
    if (m_serviceGeodatabase->table(0))
    {
      ServiceFeatureTable* featureTable = m_serviceGeodatabase->table(0);
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
      // Lock is a convenience wrapper that deletes the contents of the list once we leave scope.
      FeatureTableResultLock lock(tableEditResults);

      if (lock.results.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* tableResult = lock.results.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* featureEditResult = tableResult->editResults().first();
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

void ManageFeaturesFeatureService::updateSelectedFeature(QString fieldVal)
{
  // If the last connection is still hanging around we want to ensure it is disconnected.
  disconnect(m_featureLoadStatusChangedConnection);

  // Connect to load status changed signal, remember the connection so we can kill it once the slot has invoked.
  m_featureLoadStatusChangedConnection = connect(m_selectedFeature, &ArcGISFeature::loadStatusChanged, this, [this, fieldVal](LoadStatus)
  {
    if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
    {
      // The connection is invoked so we now forget all about this connection after this point.
      disconnect(m_featureLoadStatusChangedConnection);

      // Update the select feature's attribute value
      m_selectedFeature->attributes()->replaceAttribute("typdamage", fieldVal);

      // Update the selected feature from the table that contains it
      m_serviceGeodatabase->table(0)
        ->updateFeatureAsync(m_selectedFeature)
        .then(this, [this]()
      {
        // Call apply edits on ServiceGeodatabase
        m_serviceGeodatabase->applyEditsAsync().then(this, [this](const QList<FeatureTableEditResult*>& tableEditResults)
        {
          onApplyEditsCompleted_(tableEditResults);
        });
      });
    }
  });

  // Check if feature is loaded
  if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
  {
    // The connection is invoked so we now forget all about this connection after this point.
    disconnect(m_featureLoadStatusChangedConnection);

    // Update the select feature's attribute value
    m_selectedFeature->attributes()->replaceAttribute("typdamage", fieldVal);

    // Update the selected feature
    m_serviceGeodatabase->table(0)
      ->updateFeatureAsync(m_selectedFeature)
      .then(this, [this]()
    {
      // Call apply edits on ServiceGeodatabase
      m_serviceGeodatabase->applyEditsAsync().then(this, [this](const QList<FeatureTableEditResult*>& tableEditResults)
      {
        onApplyEditsCompleted_(tableEditResults);
      });
    });
  }
  else
  {
    m_selectedFeature->load();
  }
}

void ManageFeaturesFeatureService::onIdentifyLayerCompleted_(IdentifyLayerResult* identifyResult)
{
  if (!identifyResult)
  {
    return;
  }

  if (!identifyResult->geoElements().empty())
  {
    // Select the item in the result
    m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));
    // Update the parent so the featureLayer is not deleted when the identifyResult is deleted.
    m_featureLayer->setParent(this);

    // Obtain the selected feature with attributes
    QueryParameters queryParams;
    QString whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
    queryParams.setWhereClause(whereClause);
    m_serviceGeodatabase->table(0)
      ->queryFeaturesAsync(queryParams)
      .then(this, [this](FeatureQueryResult* featureQueryResult)
    {
      if (featureQueryResult && featureQueryResult->iterator().hasNext())
      {
        // Set selected feature member
        m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
        m_selectedFeature->setParent(this);
        m_featureType = m_selectedFeature->attributes()->attributeValue("typdamage").toString();
        m_mapView->calloutData()->setTitle(QString("<br><font size=\"+2\"><b>%1</b></font>").arg(m_featureType));
        m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());
        emit featureTypeChanged();
        emit featureSelected();
      }
    });
  }
}

void ManageFeaturesFeatureService::onApplyEditsCompleted_(const QList<FeatureTableEditResult*>& tableEditResults)
{
  // Lock is a convenience wrapper that deletes the contents of the list once we leave scope.
  FeatureTableResultLock lock(tableEditResults);

  if (lock.results.isEmpty())
  {
    qDebug() << "No table edit results";
    return;
  }

  // Get the first table result
  FeatureTableEditResult* tableResult = lock.results.first();
  if (!tableResult->editResults().isEmpty())
  {
    // Get the first feature edit result from the table
    FeatureEditResult* featureEditResult = tableResult->editResults().first();
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
      // Lock is a convenience wrapper that deletes the contents of the list once we leave scope.
      FeatureTableResultLock lock(tableEditResults);

      if (lock.results.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* tableResult = lock.results.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* featureEditResult = tableResult->editResults().first();
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
    onIdentifyLayerCompleted_(rawIdentifyResult);
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
      // Lock is a convenience wrapper that deletes the contents of tableEditResults when we leave scope.
      FeatureTableResultLock lock(tableEditResults);

      if (lock.results.isEmpty())
      {
        return;
      }

      // Get the first table result
      FeatureTableEditResult* tableResult = lock.results.first();
      if (!tableResult->editResults().isEmpty())
      {
        // Get the first feature edit result from the table
        FeatureEditResult* featureEditResult = tableResult->editResults().first();
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
    .then(this, [this](IdentifyLayerResult* identifyResult)
  {
    if (!identifyResult)
    {
      return;
    }

    if (!identifyResult->geoElements().isEmpty())
    {
      GeoElement* geoElement = identifyResult->geoElements().at(0);
      Feature* feature = static_cast<Feature*>(geoElement);

      // Select the feature
      QueryParameters query;
      query.setObjectIds(QList<qint64>{feature->attributes()->attributeValue(QStringLiteral("objectid")).toLongLong()});
      (void)m_featureLayer->selectFeaturesAsync(query, SelectionMode::New);

      // Set selected feature member and switch to selected mode
      m_selectedFeature = static_cast<ArcGISFeature*>(feature);
      m_selectedFeature->setParent(this);
      m_featureSelected = true;
    }

    delete identifyResult;
  });
}
