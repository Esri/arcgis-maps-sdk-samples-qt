// [WriteFile Name=FindRoute, Category=Routing]
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

#ifndef FIND_ROUTE_H
#define FIND_ROUTE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class PictureMarkerSymbol;
    class RouteTask;
  }
}

#include <RouteParameters.h>
#include <QQuickItem>
#include <QUrl>

class FindRoute : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* directions READ directions NOTIFY directionsChanged)

public:
  explicit FindRoute(QQuickItem* parent = nullptr);
  ~FindRoute() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void solveRoute();

signals:
  void directionsChanged();
  void solveRouteComplete();

private:
  void addStopGraphics();
  void setupRouteTask();
  QAbstractListModel* directions();
  Esri::ArcGISRuntime::PictureMarkerSymbol* getPictureMarkerSymbol(QUrl imageUrl);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_routeGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_stopsGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::RouteTask* m_routeTask = nullptr;
  Esri::ArcGISRuntime::RouteParameters m_routeParameters;
  QAbstractListModel* m_directions = nullptr;
};

#endif // FIND_ROUTE_H

