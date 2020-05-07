// [WriteFile Name=ReverseGeocodeOnline, Category=Search]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef REVERSEGEOCODEONLINE_H
#define REVERSEGEOCODEONLINE_H

namespace Esri
{
namespace ArcGISRuntime
{
class CalloutData;
class Graphic;
class GraphicsOverlay;
class LocatorTask;
class Map;
class MapQuickView;
}
}

#include <QObject>

class ReverseGeocodeOnline : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* calloutData READ calloutData NOTIFY calloutDataChanged)

public:
  explicit ReverseGeocodeOnline(QObject* parent = nullptr);
  ~ReverseGeocodeOnline();

  static void init();

signals:
  void mapViewChanged();
  void calloutDataChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::CalloutData* calloutData() const;

  void configureGraphic();
  void getAddress();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::CalloutData* m_calloutData = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask = nullptr;
};

#endif // REVERSEGEOCODEONLINE_H
