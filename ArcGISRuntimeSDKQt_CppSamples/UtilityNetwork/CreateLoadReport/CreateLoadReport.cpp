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
#include "UtilityElementTraceResult.h"
#include "UtilityFunctionTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkAttribute.h"
#include "UtilityNetworkAttributeComparison.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTerminal.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceConditionalExpression.h"
#include "UtilityTraceConfiguration.h"
#include "UtilityTraceFunction.h"
#include "UtilityTraceFunctionListModel.h"
#include "UtilityTraceFunctionOutput.h"
#include "UtilityTraceOrCondition.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResult.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityTraversability.h"


using namespace Esri::ArcGISRuntime;

CreateLoadReport::CreateLoadReport(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISColoredPencil, this)),
  m_cred(new Credential("editor01", "S7#i2LWmYH75", this)),
  m_globalId(QUuid("{1CAF7740-0BF4-4113-8DB2-654E18800028}"))
{
  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), m_cred, this);
  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this]()
  {
    initializeLoadReport();
  });
  m_utilityNetwork->load();
}

void CreateLoadReport::initializeLoadReport()
{
  m_startingLocation = createStartingLocation();
  if (!m_startingLocation)
    return;

  UtilityDomainNetwork* domainNetwork = m_utilityNetwork->definition()->domainNetwork("ElectricDistribution");
  UtilityTier* utilityTier = domainNetwork->tier("Medium Voltage Radial");

  m_baseCondition = dynamic_cast<UtilityTraceConditionalExpression*>(utilityTier->traceConfiguration()->traversability()->barriers());

  m_traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, {m_startingLocation}, this);
  m_traceParameters->resultTypes().append(UtilityTraceResultType::FunctionOutputs);
  m_traceParameters->setTraceConfiguration(utilityTier->traceConfiguration());
  //

  QList<UtilityCategory*> utilityCategories = m_utilityNetwork->definition()->categories();
  UtilityCategory* servicePointCategory = nullptr;

  for (UtilityCategory* utilityCategory : utilityCategories)
  {
    if(utilityCategory->name() == "ServicePoint")
    {
      servicePointCategory = utilityCategory;
      break;
    }
  }

  UtilityNetworkAttribute* serviceLoadAttribute = m_utilityNetwork->definition()->networkAttribute("Service Load");
  m_phasesCurrentAttribute = m_utilityNetwork->definition()->networkAttribute("Phases Current");

  if (servicePointCategory == nullptr || serviceLoadAttribute == nullptr || m_phasesCurrentAttribute == nullptr)
    return;

  UtilityCategoryComparison* serviceCategoryComparison = new UtilityCategoryComparison(servicePointCategory, UtilityCategoryComparisonOperator::Exists, this);
  UtilityTraceFunction* addLoadAttributeFunction = new UtilityTraceFunction(UtilityTraceFunctionType::Add, serviceLoadAttribute, serviceCategoryComparison);

  m_traceParameters->traceConfiguration()->functions()->clear();
  m_traceParameters->traceConfiguration()->functions()->append(addLoadAttributeFunction);
  m_traceParameters->traceConfiguration()->setOutputCondition(serviceCategoryComparison);
  m_traceParameters->traceConfiguration()->setIncludeBarriers(false);

  if (m_phasesCurrentAttribute->domain().domainType() == DomainType::CodedValueDomain)
  {
    const CodedValueDomain cvd = CodedValueDomain(m_phasesCurrentAttribute->domain());
    m_phaseList = cvd.codedValues();

    std::sort(m_phaseList.begin(), m_phaseList.end(), [](const CodedValue& cv1, const CodedValue& cv2)
    {
      return (cv1.name() < cv2.name());
    });

    runReport();
  }
}

UtilityElement* CreateLoadReport::createStartingLocation()
{
  UtilityNetworkSource* utilityNetworkSource = m_utilityNetwork->definition()->networkSource("Electric Distribution Device");
  if (!utilityNetworkSource)
    return nullptr;

  UtilityAssetGroup* utilityAssetGroup = utilityNetworkSource->assetGroup("Circuit Breaker");
  if (!utilityAssetGroup)
    return nullptr;

  m_utilityAssetType = utilityAssetGroup->assetType("Three Phase");
  if (!m_utilityAssetType)
    return nullptr;

  QList<UtilityTerminal*> utilityTerminals = m_utilityAssetType->terminalConfiguration()->terminals();
  if (!utilityTerminals.first())
    return nullptr;

  for (UtilityTerminal* utilityTerminal : utilityTerminals)
  {
    if(utilityTerminal->name() == "Load")
    {
      m_utilityTerminal = utilityTerminal;
      break;
    }
  }

  if (!m_utilityTerminal)
    return nullptr;

  return m_utilityNetwork->createElementWithAssetType(m_utilityAssetType, m_globalId, m_utilityTerminal, this);
}

void CreateLoadReport::addPhase()
{
  return;
}

void CreateLoadReport::runReport()
{
  for (CodedValue codedValue : m_phaseList)
  {
    UtilityTraceConditionalExpression* condUnac = dynamic_cast<UtilityTraceConditionalExpression*>(new UtilityNetworkAttributeComparison(m_phasesCurrentAttribute, UtilityAttributeComparisonOperator::DoesNotIncludeAny, codedValue.code()));
    UtilityTraceConditionalExpression* condBase = dynamic_cast<UtilityTraceConditionalExpression*>(m_baseCondition);


    UtilityTraceOrCondition* utoc = nullptr;
    if (condUnac && condBase)
      utoc = new UtilityTraceOrCondition(condBase, condUnac, this);

    if (utoc)
      m_traceParameters->traceConfiguration()->traversability()->setBarriers(utoc);

    connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this, codedValue](QUuid)
    {
      UtilityTraceResultListModel* results = m_utilityNetwork->traceResult();

      for (UtilityTraceResult* result : *results) {
        if (UtilityElementTraceResult* elementResult = dynamic_cast<UtilityElementTraceResult*>(result))
        {
          qDebug() << codedValue.name() << elementResult->elements().size();
        }
        if (UtilityFunctionTraceResult* functionResult = dynamic_cast<UtilityFunctionTraceResult*>(result))
        {
          qDebug() << functionResult->functionOutputs().first()->result();
        }
      }
    });

    m_utilityNetwork->trace(m_traceParameters);
  }
  emit phaseListChanged();
  return;
}

void CreateLoadReport::reset()
{
  return;
}

void CreateLoadReport::setPhases(QList<QStringList>)
{
  return;
}

QList<QStringList> CreateLoadReport::getPhases()
{
  return {{"first", "b", "c"}};
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
