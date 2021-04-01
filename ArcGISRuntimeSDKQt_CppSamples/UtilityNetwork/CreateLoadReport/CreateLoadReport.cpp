// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
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

#include "CreateLoadReport.h"

#include "Map.h"
#include "MapQuickView.h"

#include "CodedValueDomain.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityCategory.h"
#include "UtilityCategoryComparison.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkAttribute.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTerminal.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceConfiguration.h"
#include "UtilityTraceFunction.h"
#include "UtilityTraceFunctionListModel.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResult.h"
#include "UtilityTraversability.h"


using namespace Esri::ArcGISRuntime;

CreateLoadReport::CreateLoadReport(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(Basemap::imagery(this), this)),
  m_cred(new Credential("editor01", "S7#i2LWmYH75", this))
{
  globalId = QUuid("{1CAF7740-0BF4-4113-8DB2-654E18800028}");
  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), m_cred, this);
  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this]()
  {
    qDebug() << "done loading";
    UtilityNetworkSource* utilityNetworkSource = m_utilityNetwork->definition()->networkSource("Electric Distribution Device");
    UtilityAssetGroup* utilityAssetGroup = utilityNetworkSource->assetGroup("Circuit Breaker");
    m_utilityAssetType = utilityAssetGroup->assetType("Three Phase");
    QList<UtilityTerminal*> utilityTerminals = m_utilityAssetType->terminalConfiguration()->terminals();
    for (UtilityTerminal* utilityTerminal : utilityTerminals)
    {
      if(utilityTerminal->name() == "Load")
      {
        m_utilityTerminal = utilityTerminal;
        break;
      }
    }

    if (m_utilityAssetType != nullptr && globalId != NULL && m_utilityTerminal != nullptr)
    {
      UtilityElement* startingLocation = m_utilityNetwork->createElementWithAssetType(m_utilityAssetType, globalId, m_utilityTerminal, this);
      UtilityDomainNetwork* domainNetwork = m_utilityNetwork->definition()->domainNetwork("ElectricDistribution");
      UtilityTier* utilityTier = domainNetwork->tier("Medium Voltage Radial");
      UtilityTraceCondition* baseCondition = utilityTier->traceConfiguration()->traversability()->barriers();

      UtilityTraceParameters* traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, QList<UtilityElement*> {startingLocation}, this);
      traceParameters->resultTypes().append(UtilityTraceResultType::FunctionOutputs);
      traceParameters->setTraceConfiguration(utilityTier->traceConfiguration());
      traceParameters->traceConfiguration()->setIncludeBarriers(false);

      QList<UtilityCategory*> utilityCategories = m_utilityNetwork->definition()->categories();
      UtilityCategory* serviceCategory = nullptr;

      for (UtilityCategory* uc : utilityCategories)
      {
        if(uc->name() == "ServicePoint")
        {
          serviceCategory = uc;
          break;
        }
      }

      UtilityNetworkAttribute* loadAttribute = m_utilityNetwork->definition()->networkAttribute("Service Load");
      UtilityNetworkAttribute* phasesAttribute = m_utilityNetwork->definition()->networkAttribute("Phases Current");

      if (serviceCategory != nullptr && loadAttribute != nullptr && phasesAttribute != nullptr)
      {
        UtilityCategoryComparison* serviceCategoryComparison = new UtilityCategoryComparison(serviceCategory, UtilityCategoryComparisonOperator::Exists, this);
        UtilityTraceFunction* addLoadAttributeFunction = new UtilityTraceFunction(UtilityTraceFunctionType::Add, loadAttribute, serviceCategoryComparison);

        traceParameters->traceConfiguration()->functions()->clear();
        traceParameters->traceConfiguration()->functions()->append(addLoadAttributeFunction);
        traceParameters->traceConfiguration()->setOutputCondition(serviceCategoryComparison);
        qDebug() << "hello there";

        if (phasesAttribute->domain().domainType() == DomainType::CodedValueDomain)
        {
          CodedValueDomain cvd = CodedValueDomain(phasesAttribute->domain());
          for (CodedValue codedValue : cvd.codedValues())
          {
            m_phaseList.append(codedValue);
          }
        }
      }
    }

  });
  m_utilityNetwork->load();
}

CreateLoadReport::~CreateLoadReport() = default;

void CreateLoadReport::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateLoadReport>("Esri.Samples", 1, 0, "CreateLoadReportSample");
}

MapQuickView* CreateLoadReport::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateLoadReport::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();
}
