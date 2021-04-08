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
  m_globalId(QUuid("1CAF7740-0BF4-4113-8DB2-654E18800028"))
{
  m_utilityNetwork = new UtilityNetwork(QUrl("https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"), m_cred, this);

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this]()
  {

    m_utilityAssetType = m_utilityNetwork
        ->definition()
        ->networkSource("Electric Distribution Device")
        ->assetGroup("Circuit Breaker")
        ->assetType("Three Phase");

    m_utilityTier = m_utilityNetwork
        ->definition()
        ->domainNetwork("ElectricDistribution")
        ->tier("Medium Voltage Radial");



    initializeLoadReport();
  });

  m_utilityNetwork->load();
}

void CreateLoadReport::initializeLoadReport()
{
  qDebug() << "initilializing load report";
  /* UtilityElement* */             m_startingLocation = createStartingLocation();
  /* UtilityTraceConfiguration* */  m_traceConfiguration = createDefaultTraceConfiguration();
  /* UtilityTraceCondition* */      m_baseCondition = dynamic_cast<UtilityTraceConditionalExpression*>(m_traceConfiguration->traversability()->barriers());
  /* UtilityTraceParameters* */     m_traceParameters = createDefaultTraceParameters();
  m_traceParameters->setTraceConfiguration(m_traceConfiguration);
  m_phaseList = createPhaseList();
  qDebug() << "ready to go";
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

Esri::ArcGISRuntime::UtilityTraceConfiguration* CreateLoadReport::createDefaultTraceConfiguration()
{
  UtilityTraceConfiguration* traceConfig = m_utilityTier->traceConfiguration();

  // Service Category for counting total customers
  UtilityCategory* servicePointCategory = getUtilityCategory("Service Point", m_utilityNetwork);
  UtilityTraceCondition* serviceCategoryComparison = dynamic_cast<UtilityTraceCondition*>(new UtilityCategoryComparison(servicePointCategory, UtilityCategoryComparisonOperator::Exists, this));

  UtilityNetworkAttribute* serviceLoadAttribute = m_utilityNetwork->definition()->networkAttribute("Service Load");

  // For counting total load
  UtilityTraceFunction* addLoadAttributeFunction = new UtilityTraceFunction(UtilityTraceFunctionType::Add, serviceLoadAttribute, serviceCategoryComparison, this);

  traceConfig->functions()->clear();
  traceConfig->functions()->append(addLoadAttributeFunction);
  traceConfig->setOutputCondition(serviceCategoryComparison);
  traceConfig->setIncludeBarriers(false);

  return traceConfig;
}

Esri::ArcGISRuntime::UtilityTraceParameters* CreateLoadReport::createDefaultTraceParameters()
{
  UtilityTraceParameters* traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, {m_startingLocation}, this);
  traceParameters->resultTypes().append(UtilityTraceResultType::FunctionOutputs);
  traceParameters->setTraceConfiguration(m_utilityTier->traceConfiguration());

  return traceParameters;
}

UtilityCategory* CreateLoadReport::getUtilityCategory(const QString categoryName, const Esri::ArcGISRuntime::UtilityNetwork* utilityNetwork)
{
  QList<UtilityCategory*> utilityCategories = utilityNetwork->definition()->categories();

  for (UtilityCategory* utilityCategory : utilityCategories)
  {
    if(utilityCategory->name() == categoryName)
    {
      return utilityCategory;
      break;
    }
  }
  return nullptr;
}




QList<Esri::ArcGISRuntime::CodedValue> CreateLoadReport::createPhaseList()
{
  m_phasesCurrentAttribute = m_utilityNetwork->definition()->networkAttribute("Phases Current");

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
  qDebug() << "running report";
  QList<CodedValue> activeValues;
  for (CodedValue codedValue : m_phaseList)
  {
    if (selectedPhaseNames.contains(codedValue.name()))
      activeValues.append(codedValue);
  }
  qDebug() << activeValues.size();
  for (CodedValue codedValue : activeValues)
  {
    UtilityTraceConditionalExpression* utilityNetworkAttributeComparison = dynamic_cast<UtilityTraceConditionalExpression*>(
          new UtilityNetworkAttributeComparison(
            m_phasesCurrentAttribute,
            UtilityAttributeComparisonOperator::DoesNotIncludeAny,
            codedValue.code()));

    UtilityTraceOrCondition* utilityTraceOrCondition = nullptr;
    if (utilityNetworkAttributeComparison && m_baseCondition)
      utilityTraceOrCondition = new UtilityTraceOrCondition(utilityNetworkAttributeComparison, m_baseCondition, this);

        if (utilityTraceOrCondition)
          m_traceParameters->traceConfiguration()->traversability()->setBarriers(utilityTraceOrCondition);

    connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, [this, codedValue](QUuid taskId)
    {
      qDebug() << taskId;
      qDebug() << codedValue.name();
      int totalCustomers = 0;
      int totalPhaseLoad = 0;
      UtilityTraceResultListModel* results = m_utilityNetwork->traceResult();
      qDebug() << "Results" << results->size();
      for (UtilityTraceResult* result : *results) {
        if (UtilityElementTraceResult* elementResult = dynamic_cast<UtilityElementTraceResult*>(result))
        {
          QList<int> objectIds;
          for (UtilityElement* element : elementResult->elements())
          {
            if (!objectIds.contains(element->objectId()))
              ++totalCustomers;
          }

        }
        if (UtilityFunctionTraceResult* functionResult = dynamic_cast<UtilityFunctionTraceResult*>(result))
        {
          totalPhaseLoad = functionResult->functionOutputs().first()->result().toInt();
        }
      }
      m_phaseCust[codedValue.name()] = totalCustomers;
      m_phaseLoad[codedValue.name()] = totalPhaseLoad;
      qDebug() << totalCustomers;
      qDebug() << totalPhaseLoad;
      emit loadReportUpdated();
    });
    qDebug() << "triggering trace";
    m_utilityNetwork->trace(m_traceParameters);
    qDebug() << "trace triggered";
    qDebug() << Qt::endl;
  }
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

