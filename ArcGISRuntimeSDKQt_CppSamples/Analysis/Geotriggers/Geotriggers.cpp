// [WriteFile Name=Geotriggers, Category=Analysis]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Geotriggers.h"

#include "ArcadeExpression.h"
#include "ArcGISTiledLayer.h"
#include "FeatureLayer.h"
#include "FenceGeotrigger.h"
#include "FenceGeotriggerNotificationInfo.h"
#include "GeotriggerNotificationInfo.h"
#include "GeotriggerMonitor.h"
#include "GeotriggersTypes.h"
#include "Graphic.h"
#include "GraphicFenceParameters.h"
#include "FeatureFenceParameters.h"
#include "GraphicsOverlayFenceParameters.h"
#include "LocationGeotriggerFeed.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolylineBuilder.h"
#include "PortalItem.h"
#include "ServiceFeatureTable.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SimulatedLocationDataSource.h"
#include "SimulationParameters.h"

using namespace Esri::ArcGISRuntime;

namespace
{
const QString polylineJson("{\"paths\":[[[-119.709736033981,34.4570521669693],[-119.709722123569,34.4570946820209],[-119.709784418141,34.4571204667321],[-119.709823025935,34.4571301344228],[-119.709877143143,34.4571245857966],[-119.709916169552,34.4571573872012],[-119.709908648856,34.4571881901705],[-119.709877482706,34.4571754441156],[-119.709896736298,34.457212711951],[-119.709902178291,34.4572477754329],[-119.709919718796,34.4572907622135],[-119.709865739929,34.4573111649088],[-119.709844121073,34.4573334082482],[-119.709848592886,34.4573947888717],[-119.709796834655,34.4574173114339],[-119.709756465264,34.4574429390165],[-119.709742037422,34.4575083067601],[-119.709764360557,34.4575663481716],[-119.709783503656,34.4576157937041],[-119.70981359093,34.4576861721529],[-119.709821072998,34.4577396188772],[-119.709818174134,34.4577958276367],[-119.709820048918,34.457851899329],[-119.709823581094,34.4578779934187],[-119.709810698355,34.4579333799097],[-119.709820564551,34.4579849288074],[-119.709815260898,34.4580455564033],[-119.709823405024,34.4580977229731],[-119.709831231147,34.4581371976731],[-119.709835737096,34.4581954586341],[-119.709836779142,34.4582531469968],[-119.709827725921,34.4583134151609],[-119.709787835332,34.4583643105312],[-119.709755274996,34.4584108054059],[-119.709717554738,34.4584392910219],[-119.709696692264,34.4584711527103],[-119.709643918037,34.4585081414667],[-119.70958710229,34.4585341598005],[-119.709557409377,34.4585606188385],[-119.70956936775,34.4585766578023],[-119.709577652912,34.4586101408143],[-119.709545027897,34.4586550386199],[-119.709520364651,34.4586959603681],[-119.709507457657,34.4587473717671],[-119.709510294537,34.458799790474],[-119.709525521879,34.4588483116351],[-119.709548894246,34.4588765882272],[-119.709588338372,34.4589083290164],[-119.709615701056,34.4589152737244],[-119.70962745102,34.4588715552375],[-119.709655613204,34.4588327519503],[-119.709716008737,34.4587638617759],[-119.709762350128,34.4586986409315],[-119.709786079126,34.45866401803],[-119.709918153032,34.4587226593388],[-119.709974082142,34.4587450022956],[-119.709990760263,34.4587543927796]]],\"spatialReference\":{\"wkid\":4326,\"latestWkid\":4326}}");
}

Geotriggers::Geotriggers(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISDarkGrayBase, this))
{
  ArcGISTiledLayer* gardenMap = new ArcGISTiledLayer(QUrl("https://arcgisruntime.maps.arcgis.com/home/item.html?id=b6cb2e140f7248b89092e0eae88351e1"), this);
  m_map->operationalLayers()->append(gardenMap);

  m_gardenSections = new ServiceFeatureTable(new PortalItem("1ba816341ea04243832136379b8951d9", this), 0, this);
}

Geotriggers::~Geotriggers() = default;

void Geotriggers::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Geotriggers>("Esri.Samples", 1, 0, "GeotriggersSample");
}

MapQuickView* Geotriggers::mapView() const
{
  return m_mapView;
}

QVariantMap Geotriggers::sectionDesc() const
{
  return m_sectionDesc;
}
QVariantMap Geotriggers::sectionImgUrl() const
{
  return m_sectionImgUrl;
}
QStringList Geotriggers::nearbySections() const
{
  return m_nearbySections;
}

// Set the view (created in QML)
void Geotriggers::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  initializeSimulatedLocationDisplay();
  setupGeotriggerMonitor();

  emit mapViewChanged();
}

void Geotriggers::initializeSimulatedLocationDisplay()
{
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  SimulationParameters* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), 10.0, 0.0, 0.0, this);

  m_simulatedLocationDataSource->setLocationsWithPolyline(Polyline::fromJson(polylineJson), simulationParameters);

  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->setInitialZoomScale(1000);
  m_mapView->locationDisplay()->start();

  m_simulatedLocationDataSource->start();
}

void Geotriggers::setupGeotriggerMonitor()
{
  GeotriggerFeed* feed = new LocationGeotriggerFeed(m_simulatedLocationDataSource, this);
  FeatureFenceParameters* gardenSectionParams = new FeatureFenceParameters(m_gardenSections, 10.0, this);

  FenceGeotrigger* fenceGeotrigger = new FenceGeotrigger(feed, FenceRuleType::EnterOrExit, gardenSectionParams, this);

  m_geotriggerMonitor = new GeotriggerMonitor(fenceGeotrigger, this);

  connect(m_geotriggerMonitor, &GeotriggerMonitor::geotriggerNotification, this, [this](GeotriggerNotificationInfo* notification)
  {
    FenceGeotriggerNotificationInfo* fenceGeotriggerNotifInfo = static_cast<FenceGeotriggerNotificationInfo*>(notification);
    GeoElement* fenceFeature = fenceGeotriggerNotifInfo->fenceGeoElement();

    QVariantMap attributeMap = fenceFeature->attributes()->attributesMap();
    QString sectionName = attributeMap.value("name").toString();
    qDebug() << QStringList{"enter","exit"}[(int)fenceGeotriggerNotifInfo->fenceNotificationType()] << attributeMap.value("name").toString();

    if (fenceGeotriggerNotifInfo->fenceNotificationType() == FenceNotificationType::Exited)
    {
      m_nearbySections.removeAll(sectionName);
      m_sectionDesc.remove(sectionName);
      m_sectionImgUrl.remove(sectionName);
      sectionInfoChanged();
      return;
    }

    m_nearbySections.append(sectionName);
    m_sectionDesc[sectionName] = attributeMap.value("description");

    ArcGISFeature* feature = static_cast<ArcGISFeature*>(fenceFeature);

    getSectionImage(feature, sectionName);
    sectionInfoChanged();
  });

  m_geotriggerMonitor->start();
}

void Geotriggers::getSectionImage(ArcGISFeature* gardenSection, QString sectionName)
{
  QueryParameters queryParams;
  queryParams.setWhereClause("objectid=" + gardenSection->attributes()->attributeValue(QString("objectid")).toString());

  connect(m_gardenSections, &ServiceFeatureTable::queryFeaturesCompleted, this, [this, sectionName](QUuid, FeatureQueryResult* featureQueryResult)
  {
    ArcGISFeature* feature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().features(this).at(0));
    connect(feature->attachments(), &AttachmentListModel::fetchAttachmentsCompleted, this, [this, sectionName](QUuid, const QList<Attachment*>& attachments)
    {
      Attachment* sectionImageAttachment = attachments.at(0);
      connect(sectionImageAttachment, &Attachment::fetchDataCompleted, this, [this, sectionImageAttachment, sectionName]()
      {
        qDebug() << sectionName;
        if (!m_sectionImgUrl.contains(sectionName))
          m_sectionImgUrl[sectionName] = sectionImageAttachment->attachmentUrl();
        sectionInfoChanged();
        sectionImageAttachment->disconnect();
      });
      sectionImageAttachment->fetchData();
    });
    m_gardenSections->disconnect();
  });
  m_gardenSections->queryFeatures(queryParams);
}
