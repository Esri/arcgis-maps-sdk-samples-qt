// [WriteFile Name=QueryDynamicEntities, Category=Search]
// [Legal]
// Copyright 2026 Esri.
//
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

#ifndef CUSTOMDYNAMICENTITYDATASOURCE_H
#define CUSTOMDYNAMICENTITYDATASOURCE_H

// ArcGIS Maps SDK headers
#include "DynamicEntityDataSource.h"
#include "Field.h"

// Qt headers
#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QVariantMap>

namespace Esri::ArcGISRuntime
{

  class DynamicEntityDataSource;
  class DynamicEntityDataSourceInfo;

  class CustomDynamicEntityDataSource : public DynamicEntityDataSource
  {
    Q_OBJECT

  public:
    CustomDynamicEntityDataSource(const QString& fileName, const QString& entityIdField, const int msDelay, QObject* parent = nullptr);
    ~CustomDynamicEntityDataSource() override;

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

#endif // CUSTOMDYNAMICENTITYDATASOURCE_H
