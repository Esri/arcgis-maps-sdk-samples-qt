// [WriteFile Name=FilterByDefinitionExpressionOrDisplayFilter, Category=Features]
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

#include "FilterByDefinitionExpressionOrDisplayFilter.h"

#include "Basemap.h"
#include "DisplayFilter.h"
#include "DisplayFilterDefinition.h"
#include "FeatureLayer.h"
#include "ManualDisplayFilterDefinition.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "MapViewTypes.h"
#include "MapTypes.h"
#include "LayerListModel.h"
#include "QueryParameters.h"

#include <QFuture>
#include <QUuid>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

FilterByDefinitionExpressionOrDisplayFilter::FilterByDefinitionExpressionOrDisplayFilter(QQuickItem* parent) :
  QQuickItem(parent)
{
}

FilterByDefinitionExpressionOrDisplayFilter::~FilterByDefinitionExpressionOrDisplayFilter() = default;

void FilterByDefinitionExpressionOrDisplayFilter::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FilterByDefinitionExpressionOrDisplayFilter>("Esri.Samples", 1, 0, "FilterByDefinitionExpressionOrDisplayFilterSample");
}

void FilterByDefinitionExpressionOrDisplayFilter::componentComplete()
{
  QQuickItem::componentComplete();

  //! [Obtain the instantiated map view in Cpp]
  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);
  const Point center = Point(-13630484, 4545415, SpatialReference(102100));
  constexpr double scale = 300000.0;
  m_map->setInitialViewpoint(Viewpoint(center, scale));

  // Set map to map view
  m_mapView->setMap(m_map);
  //! [Obtain the instantiated map view in Cpp]

  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;

    // Initalize the feature count when the feature layer first loads
    queryFeatureCountInCurrentExtent();
    emit layerInitializedChanged();
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);
}

bool FilterByDefinitionExpressionOrDisplayFilter::layerInitialized() const
{
  return m_initialized;
}

int FilterByDefinitionExpressionOrDisplayFilter::currentFeatureCount() const
{
  return m_currentFeatureCount;
}

void FilterByDefinitionExpressionOrDisplayFilter::setDefExpression(const QString& whereClause)
{
  // reset display filter parameters before setting definition expression parameters
  resetDisplayFilterParams();

  m_featureLayer->setDefinitionExpression(whereClause);

  // Feature count in the extent should not change with different definition expressions.
  // If the extent changes and user clicks the button to reapply definition expression
  // the feature count number will update to reflect the number of feature count in the new extent
  queryFeatureCountInCurrentExtent();
}

void FilterByDefinitionExpressionOrDisplayFilter::setDisplayFilter(const QString& whereClause)
{
  // reset definition expression parameters before setting display filter parameters
  resetDefExpressionParams();

  DisplayFilter* displayFilter = new DisplayFilter("Damaged Trees", whereClause, this);
  const QList<DisplayFilter*> availableFilters{displayFilter};

  ManualDisplayFilterDefinition* displayFilterDefinition = new ManualDisplayFilterDefinition(displayFilter, availableFilters, this);
  m_featureLayer->setDisplayFilterDefinition(displayFilterDefinition);

  queryFeatureCountInCurrentExtent();
}

void FilterByDefinitionExpressionOrDisplayFilter::queryFeatureCountInCurrentExtent()
{
  QueryParameters parameters;
  parameters.setGeometry(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry());

  m_featureTable->queryFeatureCountAsync(parameters).then(this, [this](int countResult)
  {
    m_currentFeatureCount = countResult;
    emit currentFeatureCountChanged();
  });
}

void FilterByDefinitionExpressionOrDisplayFilter::resetDisplayFilterParams()
{
  DisplayFilter* displayFilter = new DisplayFilter("No Filter", "1=1", this);
  const QList<DisplayFilter*> availableFilters{displayFilter};

  ManualDisplayFilterDefinition* displayFilterDefinition = new ManualDisplayFilterDefinition(displayFilter, availableFilters, this);
  m_featureLayer->setDisplayFilterDefinition(displayFilterDefinition);

  queryFeatureCountInCurrentExtent();
}

void FilterByDefinitionExpressionOrDisplayFilter::resetDefExpressionParams()
{
  m_featureLayer->setDefinitionExpression("");

  connect(m_mapView, &MapQuickView::drawStatusChanged, this, [this](DrawStatus drawStatus)
  {
    if (drawStatus == DrawStatus::Completed)
    {
      m_mapDrawing = false;
      queryFeatureCountInCurrentExtent();
    }
    else
    {
      m_mapDrawing = true;
    }

    emit mapDrawStatusChanged();
  });
}

bool FilterByDefinitionExpressionOrDisplayFilter::mapDrawing() const
{
  return m_mapDrawing;
}
