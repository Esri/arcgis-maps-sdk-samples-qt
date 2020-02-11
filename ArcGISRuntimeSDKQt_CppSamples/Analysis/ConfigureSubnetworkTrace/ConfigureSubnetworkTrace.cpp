// [WriteFile Name=ConfigureSubnetworkTrace, Category=Analysis]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ConfigureSubnetworkTrace.h"
#include "UtilityTraceOrCondition.h"
#include "UtilityNetworkAttributeComparison.h"
#include "UtilityTraceCondition.h"
#include "UtilityNetworkAttribute.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkTypes.h"

//#include "Map.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

ConfigureSubnetworkTrace::ConfigureSubnetworkTrace(QObject* parent /* = nullptr */):
  QObject(parent)
//  m_map(new Map(Basemap::imagery(this), this))
{

  auto unac_1 = new UtilityNetworkAttributeComparison(nullptr, UtilityAttributeComparisonOperator::Equal, 2, this);
  auto unac_2 = new UtilityNetworkAttributeComparison(nullptr, UtilityAttributeComparisonOperator::Equal, 1, this);

  auto utoc_1 = new UtilityTraceOrCondition(unac_1, unac_2, this);

  auto tmp = static_cast<UtilityNetworkAttributeComparison*>(utoc_1->leftExpression());

  qDebug() << tmp->networkAttribute()->name();

}

ConfigureSubnetworkTrace::~ConfigureSubnetworkTrace() = default;

void ConfigureSubnetworkTrace::init()
{
  // Register the map view for QML
//  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ConfigureSubnetworkTrace>("Esri.Samples", 1, 0, "ConfigureSubnetworkTraceSample");
}

//MapQuickView* ConfigureSubnetworkTrace::mapView() const
//{
//  return m_mapView;
//}

// Set the view (created in QML)
//void ConfigureSubnetworkTrace::setMapView(MapQuickView* mapView)
//{
//  if (!mapView || mapView == m_mapView)
//    return;

//  m_mapView = mapView;
//  m_mapView->setMap(m_map);

//  emit mapViewChanged();
//}
