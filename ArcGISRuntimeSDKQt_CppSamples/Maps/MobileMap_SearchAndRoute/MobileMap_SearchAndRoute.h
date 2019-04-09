// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
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

#ifndef MOBILEMAP_SEARCHANDROUTE_H
#define MOBILEMAP_SEARCHANDROUTE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class RouteTask;
    class LocatorTask;
    class CalloutData;
    class MapQuickView;
    class GraphicsOverlay;
    class MobileMapPackage;
    class PictureMarkerSymbol;
    class ReverseGeocodeParameters;
  }
}

#include "Stop.h"
#include "Point.h"
#include "RouteParameters.h"
#include "ReverseGeocodeParameters.h"

#include <QQuickItem>
#include <QFileInfoList>
#include <QVariantMap>

class MobileMap_SearchAndRoute : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(bool canRoute READ canRoute NOTIFY canRouteChanged)
  Q_PROPERTY(bool canClear READ canClear NOTIFY canClearChanged)
  Q_PROPERTY(bool isGeocodeInProgress READ isGeocodeInProgress NOTIFY isGeocodeInProgressChanged)
  Q_PROPERTY(QStringList mmpkList READ mmpkList NOTIFY mmpkListChanged)
  Q_PROPERTY(QVariantList mapList READ mapList NOTIFY mapListChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* calloutData READ calloutData NOTIFY calloutDataChanged)

public:
  explicit MobileMap_SearchAndRoute(QQuickItem* parent = nullptr);
  ~MobileMap_SearchAndRoute() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void resetMapView();
  Q_INVOKABLE void createMapList(int index);
  Q_INVOKABLE void selectMap(int index);
  Q_INVOKABLE void solveRoute();

signals:
  void mmpkListChanged();
  void mapListChanged();
  void calloutDataChanged();
  void canClearChanged();
  void canRouteChanged();
  void isGeocodeInProgressChanged();

private:
  void connectSignals();
  bool canRoute() const;
  bool canClear() const;
  void createMobileMapPackages(int index);
  QStringList mmpkList() const;
  QVariantList mapList() const;
  bool isGeocodeInProgress() const;
  Esri::ArcGISRuntime::CalloutData* calloutData() const;

private:
  int m_selectedMmpkIndex = 0;
  bool m_canRoute = false;
  bool m_canClear = false;
  bool m_isGeocodeInProgress = false;
  QString m_dataPath;
  QFileInfoList m_fileInfoList;
  QStringList m_mobileMapPackageList;
  QVariantList m_mapList;
  Esri::ArcGISRuntime::Point m_clickedPoint;
  Esri::ArcGISRuntime::RouteParameters m_currentRouteParameters;
  Esri::ArcGISRuntime::ReverseGeocodeParameters m_reverseGeocodeParameters;
  QList<Esri::ArcGISRuntime::Stop> m_stops;
  QList<Esri::ArcGISRuntime::MobileMapPackage*> m_mobileMapPackages;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MobileMapPackage* m_mobileMap = nullptr;
  Esri::ArcGISRuntime::LocatorTask* m_currentLocatorTask = nullptr;
  Esri::ArcGISRuntime::PictureMarkerSymbol* m_bluePinSymbol = nullptr;
  Esri::ArcGISRuntime::RouteTask* m_currentRouteTask = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_stopsGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_routeGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::CalloutData* m_calloutData = nullptr;
  QObject* m_stopGraphicParent = nullptr;
  QObject* m_routeGraphicParent = nullptr;
};

#endif // MOBILEMAP_SEARCHANDROUTE_H
