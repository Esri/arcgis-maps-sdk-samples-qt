// Copyright 2022 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INDOORSLOCATIONDATASOURCECREATOR_H
#define INDOORSLOCATIONDATASOURCECREATOR_H

#include <QUuid>
#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeatureTable;
class FeatureTable;
class IndoorsLocationDataSource;
class Map;
}
}


class IndoorsLocationDataSourceCreator : public QObject
{
  Q_OBJECT

public:
  IndoorsLocationDataSourceCreator(QObject* parent = nullptr);
  ~IndoorsLocationDataSourceCreator();

public slots:
  void createIndoorsLocationDataSource(Esri::ArcGISRuntime::Map* map, const QString &positioningTableName, const QString &pathwaysTableName, const QUuid &globalId = QUuid());

signals:
  void createIndoorsLocationDataSourceCompleted(Esri::ArcGISRuntime::IndoorsLocationDataSource* IndoorsLocationDataSource);

private:
  void returnIndoorsLocationDataSource();
  void findPositioningTable();
  void findPathwaysTable();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::FeatureTable* m_positioningTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_pathwaysTable = nullptr;
  QUuid m_globalId;
  QString m_positioningTableName;
  QString m_pathwaysTableName;
  QStringList m_globalIdSortNames;
};

#endif // INDOORSLOCATIONDATASOURCECREATOR_H
