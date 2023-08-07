#ifndef SIMULATEDDATASOURCE_H
#define SIMULATEDDATASOURCE_H

#include "DynamicEntityDataSource.h"
#include "Field.h"

#include <QTextStream>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

namespace Esri::ArcGISRuntime {

class SimulatedDataSource : public DynamicEntityDataSource
{
  Q_OBJECT

public:
  SimulatedDataSource(QObject* parent = nullptr);
  explicit SimulatedDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent = nullptr);
  ~SimulatedDataSource() override;

  QFuture<DynamicEntityDataSourceInfo*> onLoadAsync() override;
  QFuture<void> onConnectAsync() override;
  QFuture<void> onDisconnectAsync() override;

  void addObservation(const Geometry& geometry, const QVariantMap& attributes);
  [[nodiscard]] QFuture<void> deleteEntityAsync(const QString& entityId);

  void setConnectionFailed(const QString& userDefinedError, bool reconnect);
  void setConnectionStatusAndError(ConnectionStatus status, const QString& userDefinedError = "", bool reconnect = false);
  void setLoadInfoAndError(DynamicEntityDataSourceInfo* dynamicEntityDataSourceInfo, const QString& userDefinedError = "");

private:
  void observationProcessLoopAsync();
  QList<Field> getSchema();

  QString m_fileName;
  QString m_entityIdField;
  int m_msDelay;
  QList<Field> m_fields;
  QFutureWatcher<void> m_watcher;
  bool m_isConnected = false;
};

} // namespace Esri::ArcGISRuntime

#endif // SIMULATEDDATASOURCE_H
