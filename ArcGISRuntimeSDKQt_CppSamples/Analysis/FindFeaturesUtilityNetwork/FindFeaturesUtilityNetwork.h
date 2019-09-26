// [WriteFile Name=FindFeaturesUtilityNetwork, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef FINDFEATURESUTILITYNETWORK_H
#define FINDFEATURESUTILITYNETWORK_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class UtilityNetwork;
class UtilityNetworkSource; //value type
class UtilityElement; //value type
class UtilityTerminal; //value type
class FeatureLayer;
class ServiceFeatureTable;
class SimpleMarkerSymbol;
class SimpleLineSymbol;
class GraphicsOverlay;
class Point; //valuetype
class ArcGISFeature; //value type
class UtilityTraceParameters;
}
}

#include <QObject>
#include <QUrl>
#include "Point.h"
#include "ArcGISFeature.h"

class FindFeaturesUtilityNetwork : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool terminalDialogVisisble MEMBER terminalDialogVisisble NOTIFY terminalDialogVisisbleChanged)
  Q_PROPERTY(bool startingLocationsEnabled MEMBER startingLocationsEnabled NOTIFY startingLocationsChanged)

public:
  explicit FindFeaturesUtilityNetwork(QObject* parent = nullptr);
  ~FindFeaturesUtilityNetwork();

  static void init();

  Q_INVOKABLE void multiTerminalIndex(const int& index);
  Q_INVOKABLE void trace();
  Q_INVOKABLE void reset();

signals:
  void mapViewChanged();
  void terminalDialogVisisbleChanged();
  void startingLocationsChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectSignals();
  void updateTraceParams(Esri::ArcGISRuntime::UtilityElement* element);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_deviceLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_lineLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_deviceFeatureTable = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_lineFeatureTable = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_startingSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_barrierSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParams = nullptr;

  const QUrl m_serviceUrl = QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer");
  Esri::ArcGISRuntime::Point m_clickPoint;
  bool terminalDialogVisisble = false;
  bool startingLocationsEnabled = true;
  int terminalsIndex = 0;
  Esri::ArcGISRuntime::ArcGISFeature* m_feature = nullptr;
  QList<Esri::ArcGISRuntime::UtilityTerminal*> m_terminals;
  QList<Esri::ArcGISRuntime::UtilityElement*> m_startingLocations;
  QList<Esri::ArcGISRuntime::UtilityElement*> m_barriers;

};

#endif // FINDFEATURESUTILITYNETWORK_H
