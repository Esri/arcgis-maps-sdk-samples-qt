// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SELECTION_H
#define SELECTION_H

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapView;
  class FeatureLayer;
  class ServiceFeatureTable;
  class FeatureQueryResult;
  }
}

class QPushButton;
class QLabel;

#include <QWidget>
#include <memory>
#include <QUuid>

class Selection : public QWidget
{
  Q_OBJECT

public:
  explicit Selection(QWidget* parent = 0);
  virtual ~Selection();

public slots:
  void onSelectionQueryComplete(QUuid taskId);

private:
  void createUi();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
  std::shared_ptr<Esri::ArcGISRuntime::FeatureQueryResult> m_queryResult;
  QLabel* m_selectionResult;
};

#endif // SELECTION_H
