// [WriteFile Name=QueryFeaturesWithArcadeExpression, Category=DisplayInformation]
// [Legal]
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
// [Legal]

#ifndef QUERYFEATURESWITHARCADEEXPRESSION_H
#define QUERYFEATURESWITHARCADEEXPRESSION_H

#include "Point.h"

namespace Esri
{
namespace ArcGISRuntime
{
class CalloutData;
class FeatureLayer;
class Map;
class MapQuickView;
class Point;
}
}

#include <QObject>

class QueryFeaturesWithArcadeExpression : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit QueryFeaturesWithArcadeExpression(QObject* parent = nullptr);
  ~QueryFeaturesWithArcadeExpression();

  static void init();

signals:
  void mapViewChanged();
  void showEvaluatedArcadeInCallout();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_beatsLayer = nullptr;
  Esri::ArcGISRuntime::CalloutData* m_calloutData = nullptr;
  Esri::ArcGISRuntime::Point m_clickedPoint;
  QString m_calloutText;
};

#endif // QUERYFEATURESWITHARCADEEXPRESSION_H
