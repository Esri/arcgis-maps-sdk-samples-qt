// [WriteFile Name=FindAddress, Category=Search]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef FIND_ADDRESS_H
#define FIND_ADDRESS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class Graphic;
    class LocatorTask;
  }
}

#include "GeocodeParameters.h"
#include <QQuickItem>

class FindAddress : public QQuickItem
{
  Q_OBJECT

public:
  explicit FindAddress(QQuickItem* parent = nullptr);
  ~FindAddress() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void geocodeAddress(const QString& address);
  Q_INVOKABLE void clearGraphics();

signals:
  void hideCallout();
  void showCallout();

private:
  void connectSignals();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask = nullptr;
  Esri::ArcGISRuntime::GeocodeParameters m_geocodeParameters;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
};

#endif // FIND_ADDRESS_H

