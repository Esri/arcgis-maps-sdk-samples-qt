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
#include "Geometry.h"
#include "TaskWatcher.h"
#include "Viewpoint.h"

class DisplayContentOfUtilityNetworkContainer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool showContainerView READ showContainerView WRITE setShowContainerView NOTIFY showContainerViewChanged)

public:
  explicit DisplayContentOfUtilityNetworkContainer(QObject* parent = nullptr);
  ~DisplayContentOfUtilityNetworkContainer();

  static void init();

signals:
  void mapViewChanged();
  void showContainerViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  bool showContainerView() const;
  void createConnections();
  void onIdentifyLayerCompleted(QUuid, Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult);
  void onMouseClicked(QMouseEvent& mouseEvent);
  void onFeaturesForElementsCompleted(QUuid);
  void getContainmentAssociations(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations);
  void showAttachmentAndConnectivitySymbols(QList<Esri::ArcGISRuntime::UtilityAssociation*> containmentAssociations);
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setShowContainerView(bool showContainerView);

  Esri::ArcGISRuntime::Credential* m_cred = nullptr;
  Esri::ArcGISRuntime::SubtypeFeatureLayer* m_subtypeFeatureLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_containerGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_containerElement = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::Geometry m_boundingBox;
  Esri::ArcGISRuntime::TaskWatcher m_taskWatcher;
  Esri::ArcGISRuntime::Viewpoint m_previousViewpoint;
  bool m_showContainerView = false;
  bool m_setBoundingBox = false;
};

#endif // DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H
