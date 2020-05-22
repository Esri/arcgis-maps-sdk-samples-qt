// [WriteFile Name=TraceUtilityNetwork, Category=UtilityNetwork]
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

#ifndef TraceUtilityNetwork_H
#define TraceUtilityNetwork_H

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeature;
class FeatureLayer;
class GraphicsOverlay;
class IdentifyLayerResult;
class Map;
class MapQuickView;
class ServiceFeatureTable;
class SimpleFillSymbol;
class SimpleLineSymbol;
class SimpleMarkerSymbol;
class Symbol;
class UniqueValue;
class UniqueValueRenderer;
class UtilityElement;
class UtilityNetwork;
class UtilityTerminal;
class UtilityTier;
class UtilityTraceParameters;
}
}

#include <QObject>
#include <QUrl>
#include <QUuid>
#include "Point.h"

class TraceUtilityNetwork : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool terminalDialogVisisble MEMBER m_terminalDialogVisisble NOTIFY terminalDialogVisisbleChanged)
  Q_PROPERTY(bool dialogVisible MEMBER m_dialogVisible NOTIFY dialogVisibleChanged)
  Q_PROPERTY(QString dialogText MEMBER m_dialogText NOTIFY dialogTextChanged)
  Q_PROPERTY(bool startingLocationsEnabled MEMBER m_startingLocationsEnabled NOTIFY startingLocationsChanged)
  Q_PROPERTY(double fractionAlongEdge MEMBER m_fractionAlongEdge NOTIFY fractionAlongEdgeChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(bool junctionSelected MEMBER m_junctionSelected NOTIFY junctionSelectedChanged)

public:
  explicit TraceUtilityNetwork(QObject* parent = nullptr);
  ~TraceUtilityNetwork();

  static void init();

  Q_INVOKABLE void multiTerminalIndex(int index);
  Q_INVOKABLE void trace(int index);
  Q_INVOKABLE void reset();

signals:
  void mapViewChanged();
  void terminalDialogVisisbleChanged();
  void dialogVisibleChanged();
  void dialogTextChanged();
  void startingLocationsChanged();
  void fractionAlongEdgeChanged();
  void junctionSelectedChanged();
  void busyChanged();

private slots:
  void onTraceCompleted();
  void onIdentifyLayersCompleted(QUuid, const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& results);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void connectSignals();
  void updateTraceParams(Esri::ArcGISRuntime::UtilityElement* element);
  Esri::ArcGISRuntime::UniqueValue* createUniqueValue(const QString& label, Esri::ArcGISRuntime::Symbol* fillSymbol, int value);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_deviceLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_lineLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_deviceFeatureTable = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_lineFeatureTable = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_startingSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_barrierSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_mediumVoltageSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lowVoltageSymbol = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParams = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_feature = nullptr;
  Esri::ArcGISRuntime::UtilityTier * m_mediumVoltageTier = nullptr;
  Esri::ArcGISRuntime::UniqueValueRenderer* m_uniqueValueRenderer = nullptr;

  const QUrl m_serviceUrl = QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer");
  bool m_terminalDialogVisisble = false;
  bool m_dialogVisible = false;
  bool m_startingLocationsEnabled = true;
  bool m_busy = false;
  bool m_junctionSelected = false;
  double m_fractionAlongEdge = 0.0;
  QString m_dialogText;
  Esri::ArcGISRuntime::Point m_clickPoint;
  QList<Esri::ArcGISRuntime::UtilityElement*> m_startingLocations;
  QList<Esri::ArcGISRuntime::UtilityElement*> m_barriers;
  QList<Esri::ArcGISRuntime::UtilityTerminal*> m_terminals;
  QScopedPointer<QObject> m_graphicParent;

};

#endif // TraceUtilityNetwork_H
