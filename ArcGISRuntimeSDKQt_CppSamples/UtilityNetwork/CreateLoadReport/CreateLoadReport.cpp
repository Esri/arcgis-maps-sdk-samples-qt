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
  m_cred(new Credential("editor01", "S7#i2LWmYH75", this))
{
  m_networkSourceName = "Electric Distribution Device";
  m_assetGroupName = "Circuit Breaker";
  m_assetTypeName = "Three Phase";
  m_terminalName = "Load";
  m_globalId = "{1CAF7740-0BF4-4113-8DB2-654E18800028}";
  m_domainNetworkName = "ElectricDistribution";
  m_tierName = "Medium Voltage Radial";
  m_serviceCategoryName = "ServicePoint";
  m_loadNetworkAttributeName = "Service Load";
  m_phasesNetworkAttributeName = "Phases Current";

  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), m_cred, this);

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this]()
  {

    m_utilityAssetType = m_utilityNetwork
        ->definition()
        ->networkSource(m_networkSourceName)
        ->assetGroup(m_assetGroupName)
        ->assetType(m_assetTypeName);

    m_utilityTier = m_utilityNetwork
        ->definition()
        ->domainNetwork(m_domainNetworkName)
        ->tier(m_tierName);

    initializeLoadReport();
  });

  m_utilityNetwork->load();
}

void CreateLoadReport::initializeLoadReport()
{
  /* UtilityElement* */                         m_startingLocation = createStartingLocation();
  /* UtilityTraceConfiguration* */              m_traceConfiguration = createDefaultTraceConfiguration();
  /* UtilityTraceConditionalExpression* */      m_baseCondition = dynamic_cast<UtilityTraceConditionalExpression*>(m_utilityTier->traceConfiguration()->traversability()->barriers());

  m_phaseList = createPhaseList();
  createTraceCompletedConnection();
}

void CreateLoadReport::createTraceCompletedConnection()
{
  connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this](QUuid taskId)
  {
    QString codedValueName = m_tasks.take(taskId);
    int totalCustomers = 0;
    int totalPhaseLoad = 0;
    UtilityTraceResultListModel* results = m_utilityNetwork->traceResult();

    for (UtilityTraceResult* result : *results) {
      if (UtilityElementTraceResult* elementResult = dynamic_cast<UtilityElementTraceResult*>(result))
      {
        QList<int> objectIds;
        for (UtilityElement* element : elementResult->elements())
        {
          if (!objectIds.contains(element->objectId()))
          {
            ++totalCustomers;
            objectIds.append(element->objectId());
          }
        }
      }
      if (UtilityFunctionTraceResult* functionResult = dynamic_cast<UtilityFunctionTraceResult*>(result))
      {
        totalPhaseLoad = functionResult->functionOutputs().first()->result().toInt();
      }
    }

    m_phaseCust[codedValueName] = totalCustomers;
    m_phaseLoad[codedValueName] = totalPhaseLoad;
    emit loadReportUpdated();
  });
}

UtilityElement* CreateLoadReport::createStartingLocation()
{


  if (!m_utilityAssetType)
    return nullptr;

  QList<UtilityTerminal*> utilityTerminals = m_utilityAssetType->terminalConfiguration()->terminals();
  if (!utilityTerminals.first())
    return nullptr;



  for (UtilityTerminal* utilityTerminal : utilityTerminals)
  {
    if(utilityTerminal->name() == m_terminalName)
    {
      m_utilityTerminal = utilityTerminal; // does not need to be global
      break;
    }
  }

  if (!m_utilityTerminal)
    return nullptr;

  return m_utilityNetwork->createElementWithAssetType(m_utilityAssetType, QUuid(m_globalId), m_utilityTerminal, this);
}

Esri::ArcGISRuntime::UtilityTraceConfiguration* CreateLoadReport::createDefaultTraceConfiguration()
{
  UtilityTraceConfiguration* traceConfig = m_utilityTier->traceConfiguration();

  // Service Category for counting total customers
  UtilityCategory* servicePointCategory = getUtilityCategory(m_serviceCategoryName);
  UtilityTraceCondition* serviceCategoryComparison = dynamic_cast<UtilityTraceCondition*>(new UtilityCategoryComparison(servicePointCategory, UtilityCategoryComparisonOperator::Exists, this));

  UtilityNetworkAttribute* serviceLoadAttribute = m_utilityNetwork->definition()->networkAttribute(m_loadNetworkAttributeName);

  // For counting total load
  UtilityTraceFunction* addLoadAttributeFunction = new UtilityTraceFunction(UtilityTraceFunctionType::Add, serviceLoadAttribute, serviceCategoryComparison, this);

  traceConfig->functions()->clear();
  traceConfig->functions()->append(addLoadAttributeFunction);
  traceConfig->setOutputCondition(serviceCategoryComparison);
  traceConfig->setIncludeBarriers(false);

  return traceConfig;
}

UtilityCategory* CreateLoadReport::getUtilityCategory(const QString categoryName)
{
  QList<UtilityCategory*> utilityCategories = m_utilityNetwork->definition()->categories();

  for (UtilityCategory* utilityCategory : utilityCategories)
  {
    if (utilityCategory->name() == categoryName)
    {
      return utilityCategory;
      break;
    }
  }
  return nullptr;
}

UtilityTraceParameters* CreateLoadReport::createTraceParametersWithCodedValue(CodedValue codedValue)
{
  UtilityTraceParameters* traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, {m_startingLocation}, this);
  traceParameters->resultTypes().append(UtilityTraceResultType::FunctionOutputs);
  traceParameters->setResultTypes({UtilityTraceResultType::Elements, UtilityTraceResultType::FunctionOutputs});
  traceParameters->setTraceConfiguration(m_traceConfiguration);

  UtilityTraceConditionalExpression* utilityNetworkAttributeComparison = dynamic_cast<UtilityTraceConditionalExpression*>(
        new UtilityNetworkAttributeComparison(
          m_phasesCurrentAttribute,
          UtilityAttributeComparisonOperator::DoesNotIncludeAny,
          codedValue.code()));

  UtilityTraceOrCondition* utilityTraceOrCondition = nullptr;
  if (utilityNetworkAttributeComparison && m_baseCondition)
    utilityTraceOrCondition = new UtilityTraceOrCondition(m_baseCondition, utilityNetworkAttributeComparison, this);

  if (utilityTraceOrCondition)
    traceParameters->traceConfiguration()->traversability()->setBarriers(utilityTraceOrCondition);

  return traceParameters;
}


QList<Esri::ArcGISRuntime::CodedValue> CreateLoadReport::createPhaseList()
{
  m_phasesCurrentAttribute = m_utilityNetwork->definition()->networkAttribute(m_phasesNetworkAttributeName);

  if (m_phasesCurrentAttribute->domain().domainType() == DomainType::CodedValueDomain)
  {
    const CodedValueDomain cvd = CodedValueDomain(m_phasesCurrentAttribute->domain());
    QList<CodedValue> codedValues = cvd.codedValues();

    std::sort(codedValues.begin(), codedValues.end(), [](const CodedValue& cv1, const CodedValue& cv2)
    {
      return (cv1.name() < cv2.name());
    });
    return codedValues;
  }
  return {};
}

void CreateLoadReport::runReport(QStringList selectedPhaseNames)
{
  QList<CodedValue> activeValues;
  for (CodedValue codedValue : m_phaseList)
  {
    if (selectedPhaseNames.contains(codedValue.name()))
      activeValues.append(codedValue);
    else
    {
      m_phaseCust[codedValue.name()] = 0;
      m_phaseLoad[codedValue.name()] = 0;
      emit loadReportUpdated();
    }
  }

  for (CodedValue codedValue : activeValues)
  {
    TaskWatcher task = m_utilityNetwork->trace(createTraceParametersWithCodedValue(codedValue));
    m_tasks[task.taskId()] = codedValue.name();
  }
}

CreateLoadReport::~CreateLoadReport() = default;

void CreateLoadReport::init()
{
  qmlRegisterType<CreateLoadReport>("Esri.Samples", 1, 0, "CreateLoadReportSample");
}

void CreateLoadReport::reset()
{
  return;
}

void CreateLoadReport::addPhase(QString phaseToAdd)
{
  m_activePhases.append(phaseToAdd);
  return;
}

QVariantMap CreateLoadReport::phaseCust()
{
  return m_phaseCust;
}

QVariantMap CreateLoadReport::phaseLoad()
{
  return m_phaseLoad;
}
