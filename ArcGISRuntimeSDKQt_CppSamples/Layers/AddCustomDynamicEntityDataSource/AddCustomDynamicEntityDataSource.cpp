// [WriteFile Name=AddCustomDynamicEntityDataSource, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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

#include "Geometry.h"
#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "AddCustomDynamicEntityDataSource.h"

#include "CustomDataSource.h"
#include "Domain.h"
#include "ServiceTypes.h"
#include "ArcGISStreamService.h"
#include "DynamicEntityDataSource.h"
#include "DynamicEntityLayer.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "TaskWatcher.h"
#include "Viewpoint.h"
#include "LayerListModel.h"
#include "Basemap.h"
#include "ArcGISMapImageLayer.h"
#include "TrackDisplayProperties.h"
#include "SimpleLabelExpression.h"
#include "TextSymbol.h"
#include "LabelDefinition.h"
#include "ServiceTypes.h"
#include "LabelDefinitionListModel.h"
#include "CalloutData.h"
#include "IdentifyLayerResult.h"
#include "DynamicEntityObservation.h"
#include "DynamicEntity.h"
#include "AttributeListModel.h"
#include "DynamicEntityChangedInfo.h"
#include "DynamicEntityDataSourcePurgeOptions.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

using namespace Esri::ArcGISRuntime;

AddCustomDynamicEntityDataSource::AddCustomDynamicEntityDataSource(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISOceans, this))
{
  CustomDataSource* dataSource = new CustomDataSource(":/Samples/Layers/AddCustomDynamicEntityDataSource/AIS_MarineCadastre_SelectedVessels_CustomDataSource.json", "MMSI", 10, this);
  m_dynamicEntityLayer = new DynamicEntityLayer(dataSource, this);

  m_map->operationalLayers()->append(m_dynamicEntityLayer);

  m_dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(true);
  m_dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(true);
  m_dynamicEntityLayer->trackDisplayProperties()->setMaximumObservations(20);
  m_dynamicEntityLayer->dataSource()->purgeOptions()->setMaximumObservationsPerTrack(20);

  SimpleLabelExpression* simpleLabelExpression = new SimpleLabelExpression("[VesselName]", this);

  TextSymbol* labelSymbol = new TextSymbol(this);
  labelSymbol->setColor(Qt::red);
  labelSymbol->setSize(12);

  LabelDefinition* labelDef = new LabelDefinition(simpleLabelExpression, labelSymbol, this);

  labelDef->setPlacement(LabelingPlacement::PointAboveCenter);

  m_dynamicEntityLayer->labelDefinitions()->append(labelDef);
  m_dynamicEntityLayer->setLabelsEnabled(true);
}

AddCustomDynamicEntityDataSource::~AddCustomDynamicEntityDataSource() = default;

void AddCustomDynamicEntityDataSource::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddCustomDynamicEntityDataSource>("Esri.Samples", 1, 0, "AddCustomDynamicEntityDataSourceSample");
}

MapQuickView* AddCustomDynamicEntityDataSource::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void AddCustomDynamicEntityDataSource::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpoint(Viewpoint(47.984, -123.657, 3e6));

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent &e)
  {
    auto layer = m_map->operationalLayers()->first();

    m_mapView->calloutData()->setVisible(false);

    if (m_dynamicEntity)
    {
      delete m_dynamicEntity;
      m_dynamicEntity = nullptr;
    }

    m_mapView->identifyLayerAsync(layer, e.position(), 10, false, this).then(this, [this](IdentifyLayerResult* result)
    {
      if (!result || result->geoElements().empty())
        return;

      if (DynamicEntityObservation* observation = dynamic_cast<DynamicEntityObservation*>(result->geoElements().constFirst()))
      {
        m_dynamicEntity = observation->dynamicEntity();

        if (!m_dynamicEntity)
          return;

        connect(m_dynamicEntity, &DynamicEntity::dynamicEntityChanged, this, [this](DynamicEntityChangedInfo* changedInfo)
        {
          auto changedInfoPtr = std::unique_ptr<DynamicEntityChangedInfo>(changedInfo);
          DynamicEntityObservation* currentObservation = changedInfo->receivedObservation();
          QString calloutText =
          {
            "Vessel Name: " + currentObservation->attributes()->attributeValue("VesselName").toString() + "\n" +
            "Call Sign: " + currentObservation->attributes()->attributeValue("CallSign").toString() + "\n" +
            "Course over Ground: " + currentObservation->attributes()->attributeValue("COG").toString() + "º\n" +
            "Speed over Ground: " + currentObservation->attributes()->attributeValue("SOG").toString() + " knots"
          };
          m_mapView->calloutData()->setDetail(calloutText);
          m_mapView->calloutData()->setGeoElement(currentObservation);
          m_mapView->calloutData()->setVisible(true);
        });
      }
    });
  });

  emit mapViewChanged();
}
