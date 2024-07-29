#ifndef CUSTOMDATASOURCE_H
#define CUSTOMDATASOURCE_H

#include "DynamicEntityDataSource.h"
#include "Field.h"

#include <QTextStream>
#include <QUrl>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QObject>
#include <QVariantMap>
#include <QJsonObject>
#include <QFile>

namespace Esri::ArcGISRuntime {

class DynamicEntityDataSource;
class DynamicEntityDataSourceInfo;

class CustomDataSource : public DynamicEntityDataSource
{
  Q_OBJECT

public:
  CustomDataSource(QObject* parent = nullptr);
  CustomDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent = nullptr);
  ~CustomDataSource() override;

  QFuture<DynamicEntityDataSourceInfo*> onLoadAsync() override;
  QFuture<void> onConnectAsync() override;
  QFuture<void> onDisconnectAsync() override;

private:
  void observationProcessLoopAsync();
  QList<Field> getSchema();

  QString m_fileName;
  QFile m_file;
  QTextStream m_textStream;
  QString m_entityIdField;
  int m_msDelay;
  QList<Field> m_fields;
  QFutureWatcher<void> m_watcher;
};

} // namespace Esri::ArcGISRuntime

#endif // CUSTOMDATASOURCE_H
