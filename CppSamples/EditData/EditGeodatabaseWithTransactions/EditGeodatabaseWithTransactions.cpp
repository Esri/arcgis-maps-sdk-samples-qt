#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "EditGeodatabaseWithTransactions.h"

// ArcGIS Maps SDK headers
#include "Envelope.h"
#include "FeatureLayer.h"
#include "FeatureType.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

// Qt headers
#include <QStandardPaths>

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

EditGeodatabaseWithTransactions::EditGeodatabaseWithTransactions(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this)),
  m_extent(new Envelope(-95.3035, 29.0100, -95.1053, 29.1298, SpatialReference::wgs84()))
{
  m_map->setInitialViewpoint(Viewpoint(*m_extent));

  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/SaveTheBay.geodatabase";
  qDebug()<<"Start Loading"<<dataPath;

  m_geodatabase = new Geodatabase(dataPath,this);
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

MapQuickView* EditGeodatabaseWithTransactions::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void EditGeodatabaseWithTransactions::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView){
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}

QStringList EditGeodatabaseWithTransactions::availableTableNames() const
{
  QStringList featureTableNames;

  for (Esri::ArcGISRuntime::GeodatabaseFeatureTable* featureTable : m_allFeatureTables)
  {
    QString currentTableName = featureTable->displayName().isEmpty() ? featureTable->tableName() : featureTable->displayName();
    featureTableNames.append(currentTableName);
  }

  return featureTableNames;
}

void EditGeodatabaseWithTransactions::onGeodatabaseDoneLoading_(const Error& error)
{
  if (!error.isEmpty())
  {
    return;
  }

  m_allFeatureTables.clear();
  m_totalTablesToLoad = m_geodatabase->geodatabaseFeatureTables().size();
  m_tablesLoadedCount = 0;

  for (GeodatabaseFeatureTable* table : m_geodatabase->geodatabaseFeatureTables())
  {
    m_allFeatureTables.append(table);
    FeatureLayer* layer = new FeatureLayer(table, this);
    m_mapView->map()->operationalLayers()->append(layer);

    m_tablesLoadedCount++;

    if (m_tablesLoadedCount == m_totalTablesToLoad)
    {
      if (!m_allFeatureTables.isEmpty())
      {
        updateFeatureTypesForTable(m_allFeatureTables[0]);
        emit selectedTableNameChanged();
      }
      emit availableTableNamesChanged();
      connect(m_geodatabase,&Geodatabase::transactionStatusChanged,this, &EditGeodatabaseWithTransactions::gdbTransactionStatusChanged);

      m_startEditingEnabled = true;
      emit startEditingEnabledChanged();

      setMessageText("Tap Start to begin a transaction.");
    }

  }

}

void EditGeodatabaseWithTransactions::gdbTransactionStatusChanged()
{
  if (!m_geodatabase)
    return;

  // Update UI controls based on whether the geodatabase has a current transaction
  bool isInTransaction = m_geodatabase->isInTransaction();

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
    return;

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
  if (!m_geodatabase->isInTransaction()) {
    m_geodatabase->beginTransaction();
  }
}

void EditGeodatabaseWithTransactions::stopTransaction()
{
  if (!m_geodatabase || !m_mapView)
    return;
}

void EditGeodatabaseWithTransactions::handleMapClick(int x, int y)
{
  if (!m_mapView || !m_geodatabase)
  {
    return;
  }

  // Only allow adding features if transaction conditions are met
  if (m_requireTransaction && !m_geodatabase->isInTransaction()) {
    setMessageText("Start a transaction first.");
    return;
  }

  // Emit signal to show the feature type selection dialog
  emit featureTypeSelectionRequested(x, y);
}

void EditGeodatabaseWithTransactions::updateFeatureTypesForTable(GeodatabaseFeatureTable* table)
{
  if (!table)
    return;

  m_currentFeatureTypes.clear();

  QStringList featureTypeNames;
  for (const FeatureType& featureType : table->featureTypes()) {
    QString typeName = featureType.name();
    featureTypeNames.append(typeName);
  }

  featureTypeNames.sort(Qt::CaseInsensitive);
  m_currentFeatureTypes = featureTypeNames;

  qDebug()<<"Feature types size = "<<m_currentFeatureTypes.size();

  emit currentFeatureTypesChanged();
}

void EditGeodatabaseWithTransactions::setSelectedTableName(const QString& tableName)
{
  if (m_selectedTableName != tableName)
  {
    m_selectedTableName = tableName;
    emit selectedTableNameChanged();

    for (GeodatabaseFeatureTable* table : m_geodatabase->geodatabaseFeatureTables())
    {
      QString currentTableName = table->displayName().isEmpty() ? table->tableName() : table->displayName();
      if(currentTableName ==  tableName)
      {
        updateFeatureTypesForTable(table);
      }
    }
  }
}

void EditGeodatabaseWithTransactions::setMessageText(const QString& message)
{
  if (m_messageText != message)
  {
    m_messageText = message;
    emit messageTextChanged();
  }
}
