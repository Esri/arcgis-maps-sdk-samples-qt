
#ifndef EDITGEODATABASETRANSACTIONS_H
#define EDITGEODATABASETRANSACTIONS_H

// ArcGIS Maps SDK headers
#include "Error.h"

// Qt headers
#include <QMouseEvent>
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
  QStringList availableTableNames() const;
  QString selectedTableName() const{return m_selectedTableName;}
  QStringList currentFeatureTypes() const{return m_currentFeatureTypes;}

public slots:
  void beginTransaction();
  void stopTransaction();
  void setRequireTransaction(bool require);
  void setSelectedTableName(const QString& tableName);
  void handleMapClick(int x, int y);

signals:
  void mapViewChanged();
  void startEditingEnabledChanged();
  void stopEditingEnabledChanged();
  void requireTransactionChanged();
  void messageTextChanged();
  void availableTableNamesChanged();
  void selectedTableNameChanged();
  void currentFeatureTypesChanged();
  void featureTypeSelectionRequested(int x, int y);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void onMouseClicked_(QMouseEvent& mouseEvent);
  void onGeodatabaseDoneLoading_(const Esri::ArcGISRuntime::Error& error);

  void setMessageText(const QString& message);
  void updateFeatureTypesForTable(Esri::ArcGISRuntime::GeodatabaseFeatureTable* table);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  const Esri::ArcGISRuntime::Envelope *m_extent = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;

  // Store a reference to the table to be edited
  Esri::ArcGISRuntime::GeodatabaseFeatureTable* m_editTable = nullptr;

  QList<Esri::ArcGISRuntime::GeodatabaseFeatureTable*> m_allFeatureTables;

  // UI state properties
  bool m_startEditingEnabled = false;
  bool m_stopEditingEnabled = false;
  bool m_requireTransaction = true;
  QString m_messageText;
  QString m_selectedTableName;
  QStringList m_currentFeatureTypes;

  // Table loading tracking
  int m_totalTablesToLoad = 0;
  int m_tablesLoadedCount = 0;

private slots:
  void gdbTransactionStatusChanged();
};

#endif
