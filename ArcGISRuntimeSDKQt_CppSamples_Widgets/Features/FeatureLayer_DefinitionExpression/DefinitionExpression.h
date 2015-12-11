// [WriteFile Name=DefinitionExpression, Category=Features]
// [Legal]
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
// [Legal]

#ifndef DEFINITIONEXPRESSION_H
#define DEFINITIONEXPRESSION_H

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapGraphicsView;
  class FeatureLayer;
  }
}

class QPushButton;

#include <QWidget>

class DefinitionExpression : public QWidget
{
  Q_OBJECT

public:
  explicit DefinitionExpression(QWidget* parent = 0);
  ~DefinitionExpression();

private:
  void createUi();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
  QPushButton* m_applyExpressionButton;
  QPushButton* m_resetButton;
};

#endif // DEFINITIONEXPRESSION_H
