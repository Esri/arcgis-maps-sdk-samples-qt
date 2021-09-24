// [WriteFile Name=DisplayContentOfUtilityNetworkContainer, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H
#define DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Credential;
class SubtypeFeatureLayer;
class GraphicsOverlay;
class IdentifyLayerResult;
class Map;
class MapQuickView;
class UtilityAssociation;
class UtilityElement;
class UtilityNetwork;
}
}

#include <QObject>
#include <QMouseEvent>

class DisplayContentOfUtilityNetworkContainer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit DisplayContentOfUtilityNetworkContainer(QObject* parent = nullptr);
  ~DisplayContentOfUtilityNetworkContainer();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void onIdentifyLayerCompleted(QUuid, Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult);
  void onMouseClicked(QMouseEvent& mouseEvent);
  void onFeaturesForElementsCompleted(QUuid);
  void getContainmentAssociations(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations);
  void showAttachmentAndConnectivitySymbols(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations);
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Credential* m_cred = nullptr;
  Esri::ArcGISRuntime::SubtypeFeatureLayer* m_subtypeFeatureLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_utilityElement = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  bool m_containerView = false;
};

#endif // DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H
