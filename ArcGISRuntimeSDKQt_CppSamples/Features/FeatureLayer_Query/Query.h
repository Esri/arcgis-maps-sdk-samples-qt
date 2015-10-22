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

#ifndef QUERY_H
#define QUERY_H

#include "Map.h"
#include "MapView.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "FeatureLayer.h"
#include "ServiceFeatureTable.h"
#include "SimpleRenderer.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "QueryParameters.h"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

class Query : public QWidget
{
  Q_OBJECT

public:
  explicit Query(QWidget* parent = 0);
  virtual ~Query();

public slots:
  void onQueryClicked();

private:
  void createUi();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
  QLineEdit* m_state;
  QPushButton* m_queryButton;
};

#endif // QUERY_H
