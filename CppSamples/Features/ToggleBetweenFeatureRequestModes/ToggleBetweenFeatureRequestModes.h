// [WriteFile Name=ToggleBetweenFeatureRequestModes, Category=Features]
// [Legal]
// Copyright 2025 Esri.

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

#ifndef TOGGLEBETWEENFEATUREREQUESTMODES_H
#define TOGGLEBETWEENFEATUREREQUESTMODES_H

// ArcGIS Maps SDK headers
#include "GeodatabaseTypes.h"

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class FeatureLayer;
  class ServiceFeatureTable;
  enum class FeatureRequestMode;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class ToggleBetweenFeatureRequestModes : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  enum class RequestMode
  {
    Cache = static_cast<int>(Esri::ArcGISRuntime::FeatureRequestMode::OnInteractionCache),
    NoCache = static_cast<int>(Esri::ArcGISRuntime::FeatureRequestMode::OnInteractionNoCache),
    ManualCache = static_cast<int>(Esri::ArcGISRuntime::FeatureRequestMode::ManualCache)
  };
  Q_ENUM(RequestMode)

  explicit ToggleBetweenFeatureRequestModes(QObject* parent = nullptr);
  ~ToggleBetweenFeatureRequestModes() override;

  static void init();

  Q_INVOKABLE void setFeatureRequestMode(Esri::ArcGISRuntime::FeatureRequestMode featureRequestMode);
  Q_INVOKABLE void fetchCacheManually();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_treeFeatureTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_treeFeatureLayer = nullptr;
};

#endif // TOGGLEBETWEENFEATUREREQUESTMODES_H
