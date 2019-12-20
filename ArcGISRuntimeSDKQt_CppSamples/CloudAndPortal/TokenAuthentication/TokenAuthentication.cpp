// [WriteFile Name=TokenAuthentication, Category=CloudAndPortal]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "TokenAuthentication.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ArcGISMapImageLayer.h"
#include "AuthenticationManager.h"
#include "Viewpoint.h"
#include "Envelope.h"
#include "Basemap.h"
#include <QUrl>

using namespace Esri::ArcGISRuntime;

TokenAuthentication::TokenAuthentication(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

TokenAuthentication::~TokenAuthentication() = default;

void TokenAuthentication::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<TokenAuthentication>("Esri.Samples", 1, 0, "TokenAuthenticationSample");
  // Register the AuthenticationManager for QML
  qmlRegisterUncreatableType<AuthenticationManager>("Esri.Samples", 1, 0, "AuthenticationManager", "AuthenticationManager is uncreateable");
}

void TokenAuthentication::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  if (!m_mapView)
      return;

  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Envelope(-19221397.32591107, -208306.59714691807, -3024411.4154897667, 11939432.83566906, SpatialReference(3857))));  

  // Set map to map view
  m_mapView->setMap(m_map);
}

AuthenticationManager* TokenAuthentication::authenticationManager() const
{
  return AuthenticationManager::instance();
}

void TokenAuthentication::loadSecuredLayer()
{
  // add a map service that is secured with token-based authentication
  // username/password is user1/user1
  ArcGISMapImageLayer* mapImgLyr = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA_secure_user1/MapServer"), this);
  m_map->operationalLayers()->append(mapImgLyr);
}
