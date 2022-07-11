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
class ArcGISFeature;
class CalloutData;
class Feature;
class Map;
class MapQuickView;
}
}

#include <QObject>
Q_MOC_INCLUDE("MapQuickView.h")

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

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void showEvaluatedArcadeInCallout(Esri::ArcGISRuntime::Feature* feature);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // QUERYFEATURESWITHARCADEEXPRESSION_H
