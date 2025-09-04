
#ifndef EDITGEODATABASETRANSACTIONS_H
#define EDITGEODATABASETRANSACTIONS_H

// ArcGIS Maps SDK headers
#include "Error.h"

// Qt headers
#include <QList>
#include <QMap>
#include <QObject>

namespace Esri::ArcGISRuntime
{
class Envelope;
class Geodatabase;
class GeodatabaseFeatureTable;
class Map;
class MapQuickView;
}

Q_MOC_INCLUDE("MapQuickView.h");

class EditGeodatabaseWithTransactions : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool startEditingEnabled READ startEditingEnabled NOTIFY startEditingEnabledChanged)
  Q_PROPERTY(bool stopEditingEnabled READ stopEditingEnabled NOTIFY stopEditingEnabledChanged)
  Q_PROPERTY(bool requireTransaction READ requireTransaction WRITE setRequireTransaction NOTIFY requireTransactionChanged)
  Q_PROPERTY(QString messageText READ messageText NOTIFY messageTextChanged)
  Q_PROPERTY(bool loadingVisible READ loadingVisible NOTIFY loadingVisibleChanged)
  Q_PROPERTY(QStringList availableTableNames READ availableTableNames NOTIFY availableTableNamesChanged)
  Q_PROPERTY(QString selectedTableName READ selectedTableName WRITE setSelectedTableName NOTIFY selectedTableNameChanged)
  Q_PROPERTY(QStringList currentFeatureTypes READ currentFeatureTypes NOTIFY currentFeatureTypesChanged)

public:
  explicit EditGeodatabaseWithTransactions(QObject* parent = nullptr);
  ~EditGeodatabaseWithTransactions() override;

  static void init();

  // Property getters
  bool startEditingEnabled() const{return m_startEditingEnabled;}
  bool stopEditingEnabled() const{return m_stopEditingEnabled;}
  bool requireTransaction() const{return m_requireTransaction;}
  QString messageText() const{return m_messageText;}
  bool loadingVisible() const{return m_loadingVisible;}
  QStringList availableTableNames() const{return m_availableTableNames;}
  QString selectedTableName() const{return m_selectedTableName;}
  QStringList currentFeatureTypes() const{return m_currentFeatureTypes;}

public slots:
  void beginTransaction();
  void stopTransaction();
  void commitTransaction();
  void rollbackTransaction();
  void cancelTransaction();
  void setRequireTransaction(bool require);
  void setSelectedTableName(const QString& tableName);
  void handleMapClick(int x, int y);
  void addFeatureAtLocation(const QString& tableName, const QString& featureTypeName, const QPoint& location);
  void updateFeatureTypesForSelectedTable(const QString& tableName);

signals:
  void mapViewChanged();
  void startEditingEnabledChanged();
  void stopEditingEnabledChanged();
  void requireTransactionChanged();
  void messageTextChanged();
  void loadingVisibleChanged();
  void availableTableNamesChanged();
  void selectedTableNameChanged();
  void currentFeatureTypesChanged();
  void featureTypeSelectionRequested(int x, int y);
  void commitDialogRequested();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void onGeodatabaseDoneLoading_(const Esri::ArcGISRuntime::Error& error);

  void setMessageText(const QString& message);
  void setLoadingVisible(bool visible);
  void updateFeatureTypesForTable(Esri::ArcGISRuntime::GeodatabaseFeatureTable* table);
  void showExtent();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  const Esri::ArcGISRuntime::Envelope *m_extent = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;

  // Store references to all available feature tables
  QList<Esri::ArcGISRuntime::GeodatabaseFeatureTable*> m_allFeatureTables;
  QMap<QString, Esri::ArcGISRuntime::GeodatabaseFeatureTable*> m_tablesByName;

  // UI state properties
  bool m_startEditingEnabled = false;
  bool m_stopEditingEnabled = false;
  bool m_requireTransaction = true;
  QString m_messageText;
  bool m_loadingVisible = true;
  QStringList m_availableTableNames;
  QString m_selectedTableName;
  QStringList m_currentFeatureTypes;

  // Table loading tracking
  int m_totalTablesToLoad = 0;
  int m_tablesLoadedCount = 0;

private slots:
  void gdbTransactionStatusChanged();
};

#endif
