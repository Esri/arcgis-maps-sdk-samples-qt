// [WriteFile Name=EditGeodatabaseWithTransactions, Category=EditData]
// [Legal]
// Copyright 2025 Esri.

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

// sample headers
#include "EditGeodatabaseWithTransactions.h"

// ArcGIS Maps SDK headers
#include "Error.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "FeatureTemplate.h"
#include "FeatureType.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "Geometry.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SimpleLineSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return dataPath;
}
} // namespace

EditGeodatabaseWithTransactions::EditGeodatabaseWithTransactions(QObject *parent /* = nullptr */)
  : QObject(parent)
  , m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  m_extent = Envelope(-95.3035, 29.0100, -95.1053, 29.1298, SpatialReference::wgs84());
  m_map->setInitialViewpoint(Viewpoint(m_extent));

  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/SaveTheBay.geodatabase";

  m_geodatabase = new Geodatabase(dataPath, this);
  connect(m_geodatabase, &Geodatabase::doneLoading, this, &EditGeodatabaseWithTransactions::onGeodatabaseDoneLoading_);

  m_geodatabase->load();
}

EditGeodatabaseWithTransactions::~EditGeodatabaseWithTransactions() = default;

void EditGeodatabaseWithTransactions::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<EditGeodatabaseWithTransactions>("Esri.Samples", 1, 0, "EditGeodatabaseWithTransactionsSample");
}

MapQuickView *EditGeodatabaseWithTransactions::mapView() const
{
  return m_mapView;
}

bool EditGeodatabaseWithTransactions::startEditingEnabled() const
{
  return m_startEditingEnabled;
}

bool EditGeodatabaseWithTransactions::stopEditingEnabled() const
{
  return m_stopEditingEnabled;
}

bool EditGeodatabaseWithTransactions::requireTransaction() const
{
  return m_requireTransaction;
}

QString EditGeodatabaseWithTransactions::messageText() const
{
  return m_messageText;
}

bool EditGeodatabaseWithTransactions::loadingVisible() const
{
  return m_loadingVisible;
}

QStringList EditGeodatabaseWithTransactions::availableTableNames() const
{
  return m_availableTableNames;
}

QString EditGeodatabaseWithTransactions::selectedTableName() const
{
  return m_selectedTableName;
}

QStringList EditGeodatabaseWithTransactions::currentFeatureTypes() const
{
  return m_currentFeatureTypes;
}

// Set the view (created in QML)
void EditGeodatabaseWithTransactions::setMapView(MapQuickView *mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();

  connectSignals();
}

void EditGeodatabaseWithTransactions::connectSignals()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent &mouseEvent)
  {
    if (!m_geodatabase)
    {
      return;
    }

    // Only allow adding features if transaction conditions are met
    if (m_requireTransaction && !m_geodatabase->isInTransaction())
    {
      setMessageText("Start a transaction first.");
      return;
    }

    // Get screen coordinates and emit signal to show feature type dialog
    const double screenX = mouseEvent.position().x();
    const double screenY = mouseEvent.position().y();

    emit featureTypeSelectionRequested(screenX, screenY);
  });
}

void EditGeodatabaseWithTransactions::onGeodatabaseDoneLoading_(const Error &error)
{
  if (!error.isEmpty())
  {
    setMessageText(error.message());
    setLoadingVisible(false);
    return;
  }

  setLoadingVisible(false);
  setMessageText("Using local geodatabase.");

  m_allFeatureTables.clear();
  m_tablesByName.clear();
  m_availableTableNames.clear();

  int totalTablesToLoad = m_geodatabase->geodatabaseFeatureTables().size();

  // Use a shared pointer to track the count across lambda captures
  auto tablesLoadedCount = std::make_shared<int>(0);

  for (GeodatabaseFeatureTable *table : m_geodatabase->geodatabaseFeatureTables())
  {
    // Capture totalTablesToLoad by value and tablesLoadedCount by reference
    connect(table, &GeodatabaseFeatureTable::doneLoading, this, [this, table, totalTablesToLoad, tablesLoadedCount](const Error &error)
    {
      if (!error.isEmpty())
      {
        setMessageText(error.message());
        return;
      }

      // Store reference to the table with proper name mapping
      m_allFeatureTables.append(table);

      // Set specific display names as per the Geodatabase
      QString displayName;
      if (table->tableName() == "Save_The_Bay_Marine_Sync")
      {
        table->setDisplayName("Marine");
        displayName = "Marine";
      }
      else if (table->tableName() == "Save_The_Bay_Birds_Sync")
      {
        table->setDisplayName("Bird");
        displayName = "Bird";
      }
      else
      {
        displayName = table->displayName().isEmpty() ? table->tableName() : table->displayName();
      }

      m_tablesByName[displayName] = table;
      m_availableTableNames.append(displayName);

      // Create feature layer for the map
      if (m_mapView && m_mapView->map())
      {
        FeatureLayer *layer = new FeatureLayer(table, this);
        layer->setMinScale(0);
        layer->setMaxScale(0);
        layer->setVisible(true);
        m_mapView->map()->operationalLayers()->append(layer);
      }

      (*tablesLoadedCount)++;

      // When all tables are loaded
      if (*tablesLoadedCount == totalTablesToLoad)
      {
        // Initialize with first table as default selection
        if (!m_availableTableNames.isEmpty())
        {
          m_selectedTableName = m_availableTableNames.first();
          updateFeatureTypesForTable(m_tablesByName[m_selectedTableName]);
          emit selectedTableNameChanged();
        }

        emit availableTableNamesChanged();

        // Connect transaction status monitoring
        connect(m_geodatabase, &Geodatabase::transactionStatusChanged, this, &EditGeodatabaseWithTransactions::gdbTransactionStatusChanged_);

        // Show the extent graphic
        showExtent();

        // Set the map viewpoint to show the geodatabase extent
        if (m_mapView)
        {
          m_mapView->setViewpointAsync(Viewpoint(m_extent));
        }

        m_startEditingEnabled = true;
        emit startEditingEnabledChanged();

        setMessageText("Tap Start to begin a transaction.");
      }
    });

    table->load();
  }
}

void EditGeodatabaseWithTransactions::gdbTransactionStatusChanged_()
{
  if (!m_geodatabase)
  {
    return;
  }
  // Update UI controls based on whether the geodatabase has a current transaction
  const bool isInTransaction = m_geodatabase->isInTransaction();

  m_stopEditingEnabled = isInTransaction;
  m_startEditingEnabled = !isInTransaction && m_requireTransaction;

  // Update status message based on current state
  if (isInTransaction)
  {
    setMessageText("Transaction started.");
  }
  else if (m_requireTransaction)
  {
    setMessageText("Tap Start to begin a transaction.");
  }
  else
  {
    setMessageText("Tap on the map to add a feature.");
  }

  emit stopEditingEnabledChanged();
  emit startEditingEnabledChanged();
}

void EditGeodatabaseWithTransactions::setRequireTransaction(bool require)
{
  if (!m_geodatabase)
  {
    return;
  }

  if (!require && m_geodatabase->isInTransaction())
  {
    setMessageText("Stop editing to end the current transaction.");
    return;
  }

  m_requireTransaction = require;
  emit requireTransactionChanged();

  // Update status message based on requirement
  if (require)
  {
    setMessageText("Tap Start to begin a transaction.");
  }
  else
  {
    setMessageText("Tap on the map to add a feature.");
  }

  // Update control states
  bool isInTransaction = m_geodatabase->isInTransaction();
  m_startEditingEnabled = require && !isInTransaction;
  m_stopEditingEnabled = require && isInTransaction;

  emit startEditingEnabledChanged();
  emit stopEditingEnabledChanged();
}

void EditGeodatabaseWithTransactions::beginTransaction()
{
  if (!m_geodatabase)
  {
    return;
  }

  // Begin transaction for the geodatabase if its not in transaction
  if (!m_geodatabase->isInTransaction())
  {
    m_geodatabase->beginTransaction();
  }
}

void EditGeodatabaseWithTransactions::stopTransaction()
{
  if (!m_geodatabase || !m_mapView)
  {
    return;
  }

  // Ask the user if they want to commit or rollback the transaction
  emit commitDialogRequested();
}

void EditGeodatabaseWithTransactions::commitTransaction()
{
  if (!m_geodatabase)
  {
    return;
  }

  // Commit the transaction to store the edits (this will also end the transaction)
  m_geodatabase->commitTransaction();
  setMessageText("Edits committed to geodatabase.");
}

void EditGeodatabaseWithTransactions::rollbackTransaction()
{
  if (!m_geodatabase)
  {
    return;
  }

  // Rollback the transaction to discard the edits (this will also end the transaction)
  m_geodatabase->rollbackTransaction();
  setMessageText("Edits discarded.");
}

void EditGeodatabaseWithTransactions::cancelTransaction()
{
  // User canceled - keep transaction active
  setMessageText("Transaction started.");
}

void EditGeodatabaseWithTransactions::updateFeatureTypesForTable(GeodatabaseFeatureTable *table)
{
  if (!table)
  {
    return;
  }

  m_currentFeatureTypes.clear();

  QStringList featureTypeNames;
  for (const FeatureType &featureType : table->featureTypes())
  {
    featureTypeNames.append(featureType.name());
  }

  featureTypeNames.sort(Qt::CaseInsensitive);
  m_currentFeatureTypes = featureTypeNames;

  emit currentFeatureTypesChanged();
}

void EditGeodatabaseWithTransactions::setSelectedTableName(const QString &tableName)
{
  if (m_selectedTableName != tableName)
  {
    m_selectedTableName = tableName;
    emit selectedTableNameChanged();

    // Update feature types for the newly selected table
    GeodatabaseFeatureTable *table = m_tablesByName.value(tableName, nullptr);
    if (table)
    {
      updateFeatureTypesForTable(table);
    }
  }
}

void EditGeodatabaseWithTransactions::addFeatureAtLocation(const QString &tableName, const QString &featureTypeName, const QPoint &location)
{
  if (!m_mapView || !m_geodatabase)
  {
    return;
  }

  // Get the correct table using dynamic lookup
  GeodatabaseFeatureTable *targetTable = m_tablesByName.value(tableName, nullptr);

  if (!targetTable)
  {
    setMessageText(QString("Table '%1' not found.").arg(tableName));
    return;
  }

  // Find the feature type
  FeatureType selectedFeatureType;
  bool foundType = false;
  for (const FeatureType &featureType : targetTable->featureTypes())
  {
    if (featureType.name() == featureTypeName)
    {
      selectedFeatureType = featureType;
      foundType = true;
      break;
    }
  }

  if (!foundType)
  {
    setMessageText(QString("Feature type '%1' not found in table '%2'.").arg(featureTypeName, tableName));
    return;
  }

  // Convert screen point to map point
  Point mapPoint = m_mapView->screenToLocation(location.x(), location.y());

  if (mapPoint.isEmpty())
  {
    setMessageText("Invalid location clicked.");
    return;
  }

  // Convert point to WGS84 for comparison with extent (extent is in WGS84)
  Point wgs84Point = mapPoint;
  if (!mapPoint.spatialReference().isEmpty() && mapPoint.spatialReference().wkid() != 4326)
  {
    // Project the point to WGS84
    Geometry projectedGeometry = GeometryEngine::project(mapPoint, SpatialReference::wgs84());
    if (!projectedGeometry.isEmpty())
    {
      wgs84Point = static_cast<Point>(projectedGeometry);
    }
  }

  // Check if point is within extent
  bool withinBounds = (wgs84Point.x() >= m_extent.xMin() && wgs84Point.x() <= m_extent.xMax()
                       && wgs84Point.y() >= m_extent.yMin()
                       && wgs84Point.y() <= m_extent.yMax());

  if (!withinBounds)
  {
    setMessageText("Error: Feature geometry is outside the generate geodatabase geometry.");
    return;
  }

  // Create feature using the selected type's template attributes
  QVariantMap attributes;
  if (!selectedFeatureType.templates().empty())
  {
    // Use the first template's attributes if available
    attributes = selectedFeatureType.templates().at(0).prototypeAttributes();
  }

  Feature *newFeature = targetTable->createFeature(attributes, mapPoint);
  if (!newFeature)
  {
    setMessageText("Failed to create new feature.");
    return;
  }

  // Add the new feature to the table
  QFuture<void> addFuture = targetTable->addFeatureAsync(newFeature);
  QFutureWatcher<void> *addWatcher = new QFutureWatcher<void>(this);

  connect(addWatcher, &QFutureWatcher<void>::finished, this, [this, addWatcher, targetTable]()
  {
    try
    {
      if (addWatcher && addWatcher->future().isValid() && !addWatcher->future().isCanceled())
      {
        setMessageText("Added feature.");

        // Force refresh of the feature layer to show the new feature
        if (m_mapView && m_mapView->map() && m_mapView->map()->operationalLayers())
        {
          for (Layer *layer : *m_mapView->map()->operationalLayers())
          {
            if (FeatureLayer *featureLayer = qobject_cast<FeatureLayer *>(layer))
            {
              if (featureLayer->featureTable() == targetTable)
              {
                // Force redraw of the layer to show new features
                featureLayer->setVisible(false);
                featureLayer->setVisible(true);
                break;
              }
            }
          }
        }
      }
      else
      {
        setMessageText("Error adding feature.");
      }
    }
    catch (...)
    {
      setMessageText("Error adding feature.");
    }

    if (addWatcher)
    {
      addWatcher->deleteLater();
    }
  });
  addWatcher->setFuture(addFuture);
}

void EditGeodatabaseWithTransactions::updateFeatureTypesForSelectedTable(const QString &tableName)
{
  GeodatabaseFeatureTable *table = m_tablesByName.value(tableName, nullptr);
  updateFeatureTypesForTable(table);
}

void EditGeodatabaseWithTransactions::setMessageText(const QString &message)
{
  if (m_messageText != message)
  {
    m_messageText = message;
    emit messageTextChanged();
  }
}

void EditGeodatabaseWithTransactions::setLoadingVisible(bool visible)
{
  if (m_loadingVisible != visible)
  {
    m_loadingVisible = visible;
    emit loadingVisibleChanged();
  }
}

void EditGeodatabaseWithTransactions::showExtent()
{
  if (!m_mapView)
  {
    return;
  }

  // Create a graphic for the geodatabase extent
  SimpleLineSymbol *lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::red), 2, this);
  Graphic *extentGraphic = new Graphic(m_extent, lineSymbol, this);

  // Create a graphics overlay for the extent graphic
  GraphicsOverlay *extentOverlay = new GraphicsOverlay(this);
  extentOverlay->graphics()->append(extentGraphic);

  // Add graphics overlay to the map view
  m_mapView->graphicsOverlays()->append(extentOverlay);
}
