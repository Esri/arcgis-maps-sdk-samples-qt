// [WriteFile Name=FilterBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "FilterBuildingSceneLayer.h"

// ArcGIS Maps SDK headers
#include "AttributeListModel.h"
#include "BuildingComponentSublayer.h"
#include "BuildingFilter.h"
#include "BuildingFilterBlock.h"
#include "BuildingGroupSublayer.h"
#include "BuildingSceneLayer.h"
#include "BuildingSceneLayerAttributeStatistics.h"
#include "BuildingSolidFilterMode.h"
#include "BuildingSublayer.h"
#include "BuildingSublayerListModel.h"
#include "BuildingXrayFilterMode.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "GeoElement.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "LocalSceneQuickView.h"
#include "MapTypes.h"
#include "PopupDefinition.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"

// Qt headers
#include <QFuture>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

FilterBuildingSceneLayer::FilterBuildingSceneLayer(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_scene(new Scene(QUrl("https://arcgisruntime.maps.arcgis.com/home/item.html?id=b7c387d599a84a50aafaece5ca139d44"), this))
{
  connect(m_scene, &Scene::doneLoading, this, [this]()
  {
    getFloorList();
    getCategoriesList();
  });
}

FilterBuildingSceneLayer::~FilterBuildingSceneLayer() = default;

void FilterBuildingSceneLayer::init()
{
  // Register classes for QML
  qmlRegisterType<LocalSceneQuickView>("Esri.Samples", 1, 0, "LocalSceneView");
  qmlRegisterType<FilterBuildingSceneLayer>("Esri.Samples", 1, 0, "FilterBuildingSceneLayerSample");
  qmlRegisterUncreatableType<BuildingSublayerListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

LocalSceneQuickView* FilterBuildingSceneLayer::localSceneView() const
{
  return m_localSceneView;
}

// Set the view (created in QML)
void FilterBuildingSceneLayer::setLocalSceneView(LocalSceneQuickView* localSceneView)
{
  if (!localSceneView || localSceneView == m_localSceneView)
  {
    return;
  }

  m_localSceneView = localSceneView;
  m_localSceneView->setArcGISScene(m_scene);

  m_scene->load();

  connect(m_localSceneView, &LocalSceneQuickView::mouseClicked, this, &FilterBuildingSceneLayer::onMouseClicked);

  emit localSceneViewChanged();
}

void FilterBuildingSceneLayer::onMouseClicked(QMouseEvent& mouseEvent)
{
  if (!m_buildingSceneLayer || m_buildingSceneLayer->loadStatus() != LoadStatus::Loaded)
  {
    return;
  }

  // Clear previous selection if any
  if (m_selectedSublayer)
  {
    m_selectedSublayer->clearSelection();
    m_selectedSublayer = nullptr;
  }

  constexpr double tolerance = 5.0;

  // Identify on the building scene layer.
  QFuture future = m_localSceneView->identifyLayerAsync(m_buildingSceneLayer, mouseEvent.position(), tolerance, false, this);

  future.then(this, [this](IdentifyLayerResult* result)
  {
    if (!result || result->sublayerResults().empty())
    {
      return;
    }

    // Select the first identified feature and show the feature details in a popup.
    IdentifyLayerResult* sublayerResult = result->sublayerResults().first();
    GeoElement* geoElement = sublayerResult->geoElements().first();
    Feature* identifiedFeature = static_cast<Feature*>(geoElement);

    m_selectedSublayer = static_cast<BuildingComponentSublayer*>(sublayerResult->layerContent());
    if (m_selectedSublayer)
    {
      m_selectedSublayer->selectFeature(identifiedFeature);
    }

    m_popup = new Popup(geoElement, this);
    emit popupChanged();
  });
}

void FilterBuildingSceneLayer::getFloorList()
{
  if (!m_scene || !m_scene->operationalLayers() || m_scene->operationalLayers()->isEmpty())
  {
    return;
  }

  m_buildingSceneLayer = dynamic_cast<BuildingSceneLayer*>(m_scene->operationalLayers()->first());

  // Get the floor listing from the statistics.
  m_buildingSceneLayer->fetchStatisticsAsync(this).then(this, [this](QMap<QString, BuildingSceneLayerAttributeStatistics*> statistics)
  {
    if (statistics.value("BldgLevel"))
    {
      m_floorList = statistics.value("BldgLevel")->mostFrequentValues();
      std::sort(m_floorList.begin(), m_floorList.end());
      m_floorList.prepend("All");
      emit floorListChanged();
    }
  });
}

void FilterBuildingSceneLayer::getCategoriesList()
{
  if (!m_buildingSceneLayer)
  {
    return;
  }

  connect(m_buildingSceneLayer, &BuildingSceneLayer::loadStatusChanged, this, [this]()
  {
    if (m_buildingSceneLayer->loadStatus() != LoadStatus::Loaded)
    {
      return;
    }
    for (BuildingSublayer* sublayer : *m_buildingSceneLayer->sublayers())
    {
      if (sublayer->name() == "Full Model")
      {
        // Get the sublayer group for the full building model.
        m_fullModelSublayer = qobject_cast<BuildingGroupSublayer*>(sublayer);
        break;
      }
    }
    if (m_fullModelSublayer)
    {
      // The top-level sublayer groups will be the categories.
      m_sublayerListModel = m_fullModelSublayer->sublayers();
      emit sublayerListModelChanged();
    }
  });
  m_buildingSceneLayer->load();
}

// To update the building filters based on the selected floor.
void FilterBuildingSceneLayer::updateFloorFilter(const QString& selectedFloor)
{
  if (!m_buildingSceneLayer)
  {
    return;
  }

  // No filtering applied if 'All' floors are selected.
  if (selectedFloor == "All")
  {
    m_buildingSceneLayer->setActiveFilter(nullptr);
    return;
  }

  // Build a building filter to show the selected floor and an xray view of the floors below.
  // Floors above the selected floor are not shown at all.
  BuildingFilterBlock* solidBlock =
    new BuildingFilterBlock("solid block", QString("BldgLevel = '%1'").arg(selectedFloor), new BuildingSolidFilterMode(), this);

  m_blocks.append(solidBlock);

  BuildingFilterBlock* xrayBlock =
    new BuildingFilterBlock("xray block", QString("BldgLevel < '%1'").arg(selectedFloor), new BuildingXrayFilterMode(), this);

  m_blocks.append(xrayBlock);

  m_buildingFilter = new BuildingFilter("Floor filter", "Show selected floor and xray filter for lower floors.", m_blocks, this);

  // Apply the filter to the building scene layer.
  m_buildingSceneLayer->setActiveFilter(m_buildingFilter);
  m_blocks.clear();
}

// Get the component sublayers for each category.
BuildingSublayerListModel* FilterBuildingSceneLayer::getComponentSubLayerListModel(int layerId) const
{
  if (!m_scene)
  {
    return nullptr;
  }

  BuildingGroupSublayer* categorySublayer = static_cast<BuildingGroupSublayer*>(m_fullModelSublayer->sublayers()->at(layerId));

  if (categorySublayer)
  {
    return categorySublayer->sublayers();
  }

  return nullptr;
}

QStringList FilterBuildingSceneLayer::floorList() const
{
  return m_floorList;
}

void FilterBuildingSceneLayer::clearSelection()
{
  if (m_selectedSublayer)
  {
    m_selectedSublayer->clearSelection();
  }
}
