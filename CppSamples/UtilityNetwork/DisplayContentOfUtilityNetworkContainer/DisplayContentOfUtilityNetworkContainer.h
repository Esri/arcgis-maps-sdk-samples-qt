// [WriteFile Name=DisplayContentOfUtilityNetworkContainer, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.
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

#ifndef DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H
#define DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H

// ArcGIS Maps SDK headers
#include "Authentication/ArcGISAuthenticationChallengeHandler.h"
#include "Geometry.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QMouseEvent>

namespace Esri::ArcGISRuntime
{
  class ArcGISFeature;
  class ErrorException;
  class GraphicsOverlay;
  class IdentifyLayerResult;
  class Map;
  class MapQuickView;
  class SubtypeFeatureLayer;
  class Symbol;
  class UtilityAssociation;
  class UtilityElement;
  class UtilityNetwork;
}

namespace Esri::ArcGISRuntime::Authentication
{
  class ArcGISAuthenticationChallenge;
}

class SymbolImageProvider;

Q_MOC_INCLUDE("MapQuickView.h")

class DisplayContentOfUtilityNetworkContainer : public Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool showContainerView READ showContainerView WRITE setShowContainerView NOTIFY showContainerViewChanged)
  Q_PROPERTY(QString attachmentSymbolUrl READ attachmentSymbolUrl NOTIFY attachmentSymbolUrlChanged)
  Q_PROPERTY(QString connectivitySymbolUrl READ connectivitySymbolUrl NOTIFY connectivitySymbolUrlChanged)
  Q_PROPERTY(QString boundingBoxSymbolUrl READ boundingBoxSymbolUrl NOTIFY boundingBoxSymbolUrlChanged)
  Q_PROPERTY(QString messageBoxText READ messageBoxText WRITE setMessageBoxText NOTIFY messageBoxTextChanged)

public:
  explicit DisplayContentOfUtilityNetworkContainer(QObject* parent = nullptr);
  ~DisplayContentOfUtilityNetworkContainer();

  static void init();

signals:
  void mapViewChanged();
  void showContainerViewChanged();
  void attachmentSymbolUrlChanged();
  void connectivitySymbolUrlChanged();
  void boundingBoxSymbolUrlChanged();
  void messageBoxTextChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void createConnections();
  bool showContainerView() const;
  void identifyFeaturesAtMouseClick(QMouseEvent& mouseEvent);
  void getUtilityAssociationsOfFeature(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResult);
  void displayFeaturesAndGetAssociations();
  void getFeaturesForElementsOfUtilityAssociations(const QList<Esri::ArcGISRuntime::UtilityAssociation*>& containmentAssociations);
  void showAttachmentAndConnectivitySymbols(const QList<Esri::ArcGISRuntime::UtilityAssociation*>& containmentAssociations);
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setShowContainerView(bool showContainerView);
  void setMessageBoxText(const QString& message);
  QString messageBoxText() const;
  void createLegend();
  QString attachmentSymbolUrl() const;
  QString connectivitySymbolUrl() const;
  QString boundingBoxSymbolUrl() const;
  void onAssociationsCompleted_(const QList<Esri::ArcGISRuntime::UtilityAssociation*>& containmentAssociations);
  void onTaskFailed(const QString& errorMsg, const Esri::ArcGISRuntime::ErrorException& taskException);

  void handleArcGISAuthenticationChallenge(Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallenge* challenge) override;

  Esri::ArcGISRuntime::Geometry m_boundingBox;
  Esri::ArcGISRuntime::GraphicsOverlay* m_containerGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QFuture<QList<Esri::ArcGISRuntime::ArcGISFeature*>> m_featuresFuture;
  QFuture<QList<Esri::ArcGISRuntime::UtilityAssociation*>> m_utilityAssociationFuture;
  Esri::ArcGISRuntime::UtilityElement* m_containerElement = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::Viewpoint m_previousViewpoint;
  bool m_showContainerView = false;
  bool m_setBoundingBox = false;
  QString m_messageBoxText;
  QString m_attachmentSymbolUrl;
  QString m_connectivitySymbolUrl;
  QString m_boundingBoxSymbolUrl;
  Esri::ArcGISRuntime::Symbol* m_attachmentSymbol = nullptr;
  Esri::ArcGISRuntime::Symbol* m_connectivitySymbol = nullptr;
  Esri::ArcGISRuntime::Symbol* m_boundingBoxSymbol = nullptr;
  SymbolImageProvider* m_symbolImageProvider = nullptr;
};

#endif // DISPLAYCONTENTOFUTILITYNETWORKCONTAINER_H
