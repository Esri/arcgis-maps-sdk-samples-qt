// [WriteFile Name=DisplaySubtypeFeatureLayer, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef DISPLAYSUBTYPEFEATURELAYER_H
#define DISPLAYSUBTYPEFEATURELAYER_H

// ArcGIS Maps SDK headers
#include "SubtypeSublayer.h"
#include "Authentication/ArcGISAuthenticationChallengeHandler.h"

namespace Esri::ArcGISRuntime
{
class FeatureLayer;
class LabelDefinition;
class Map;
class MapQuickView;
class Renderer;
class SimpleRenderer;
class SubtypeFeatureLayer;
class SubtypeSublayer;
}

namespace Esri::ArcGISRuntime::Authentication
{
  class ArcGISAuthenticationChallenge;
}

Q_MOC_INCLUDE("MapQuickView.h")

class DisplaySubtypeFeatureLayer : public Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(double mapScale MEMBER m_mapScale NOTIFY mapScaleChanged)
  Q_PROPERTY(double sublayerMinScale MEMBER m_sublayerMinScale NOTIFY sublayerMinScaleChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)

public:
  explicit DisplaySubtypeFeatureLayer(QObject* parent = nullptr);
  ~DisplaySubtypeFeatureLayer();

  Q_INVOKABLE void switchSublayerVisibility();
  Q_INVOKABLE void setOringalRenderer();
  Q_INVOKABLE void setAlternativeRenderer();
  Q_INVOKABLE void setSublayerMinScale();

  static void init();

signals:
  void mapViewChanged();
  void mapScaleChanged();
  void sublayerMinScaleChanged();
  void busyChanged();

private slots:
  void getSubtypeSublayerAndDefineLabels(const Esri::ArcGISRuntime::Error& e);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::LabelDefinition* createLabelDefinition();

  void handleArcGISAuthenticationChallenge(Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallenge* challenge) override;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::SubtypeFeatureLayer* m_subtypeFeatureLayer = nullptr;
  Esri::ArcGISRuntime::SubtypeSublayer* m_subtypeSublayer = nullptr;
  Esri::ArcGISRuntime::LabelDefinition* m_labelDefinition = nullptr;
  Esri::ArcGISRuntime::Renderer* m_originalRenderer = nullptr;
  Esri::ArcGISRuntime::SimpleRenderer* m_alternateRenderer = nullptr;
  Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler* m_previousChallengeHandler = nullptr;

  double m_mapScale = 0.0;
  double m_sublayerMinScale = 0.0;
  bool m_busy = false;
};

#endif // DISPLAYSUBTYPEFEATURELAYER_H
