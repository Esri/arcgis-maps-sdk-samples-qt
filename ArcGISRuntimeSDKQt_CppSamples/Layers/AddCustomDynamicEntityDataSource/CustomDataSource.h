#ifndef CUSTOMDATASOURCE_H
#define CUSTOMDATASOURCE_H

#include "DynamicEntityDataSource.h"
#include "Field.h"
#include "Geometry.h"
#include "RealTimeTypes.h"

#include <QTextStream>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QObject>
#include <QVariantMap>
#include <QJsonObject>

namespace Esri::ArcGISRuntime {

class DynamicEntityDataSource;
class DynamicEntityDataSourceInfo;

class CustomDataSource : public DynamicEntityDataSource
{
  Q_OBJECT

public:
  CustomDataSource(QObject* parent = nullptr);
  CustomDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent = nullptr);


//  void addObservation(const Geometry& geometry, const QVariantMap& attributes);
//  QFuture<void> deleteEntityAsync(const QString& entityId);

//  void setConnectionFailed(const QString& userDefinedError, bool reconnect);
//  void setConnectionStatusAndError(ConnectionStatus status, const QString& userDefinedError = "", bool reconnect = false);
//  void setLoadInfoAndError(DynamicEntityDataSourceInfo* dynamicEntityDataSourceInfo, const QString& userDefinedError = "");

  QFuture<DynamicEntityDataSourceInfo*> onLoadAsync() override;
  QFuture<void> onConnectAsync() override;
  QFuture<void> onDisconnectAsync() override;

  QVariantMap convertJsonObjectToVariantMap(const QJsonObject& jsonObject);

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

#endif // CUSTOMDATASOURCE_H
