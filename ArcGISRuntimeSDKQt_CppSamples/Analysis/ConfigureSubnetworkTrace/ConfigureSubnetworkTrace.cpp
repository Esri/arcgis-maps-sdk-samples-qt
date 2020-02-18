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
#include "UtilityTraceConfiguration.h"
#include "UtilityTraversability.h"

#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceAndCondition.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityCategoryComparison.h"
#include "UtilityTraceAndCondition.h"
#include "UtilityCategory.h"
#include "Domain.h"
#include "CodedValueDomain.h"
#include "UtilityTerminal.h"


//#include "Map.h"
#include "MapQuickView.h"

////* maybe change at end
//ConfigureSubnetworkTrace::ConfigureSubnetworkTrace(QQuickItem* parent /* = nullptr */):
//  QQuickItem(parent)
////  m_map(new Map(Basemap::imagery(this), this))
//{

//}

using namespace Esri::ArcGISRuntime;

ConfigureSubnetworkTrace::ConfigureSubnetworkTrace(QObject* parent /* = nullptr */):
  QObject(parent)
//  m_map(new Map(Basemap::imagery(this), this))
{
  m_utilityNetwork = new UtilityNetwork(m_featureLayerUrl, this);

  connect(m_utilityNetwork, &UtilityNetwork::errorOccurred, [](Error e)
  {
    if (e.isEmpty())
      return;

    qDebug() << QString(e.message() + " - " + e.additionalMessage());
  });

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, [this](Error e)
  {

    m_networkDefinition = m_utilityNetwork->definition();

    for (UtilityNetworkAttribute* networkAttribute : m_networkDefinition->networkAttributes())
    {
      if (!networkAttribute->isSystemDefined())
      {
        m_attributeListModel.append(networkAttribute->name());
      }
    }
    emit attributeListModelChanged();

    UtilityNetworkSource* networkSource = m_networkDefinition->networkSource(m_deviceTableName);
    UtilityAssetGroup* assetGroup = networkSource->assetGroup(m_assetGroupName);
    UtilityAssetType* assetType = assetGroup->assetType(m_assetTypeName);


    m_utilityElementStartingLocation = m_utilityNetwork->createElementWithAssetType(assetType, m_gloabId);

    QList<UtilityTerminal*> terminals = m_utilityElementStartingLocation->assetType()->terminalConfiguration()->terminals();

    for (UtilityTerminal* terminal : terminals)
    {
      if (terminal->name() == "Load")
      {
        m_utilityElementStartingLocation->setTerminal(terminal);
        break;
      }
    }

    UtilityDomainNetwork* domainNetwork = m_networkDefinition->domainNetwork(m_domainNetworkName);
    UtilityTier* utilityTierSource = domainNetwork->tier(m_tierName);

    m_traceConfiguration = utilityTierSource->traceConfiguration();

    m_initialExpression = m_traceConfiguration->traversability()->barriers();


    QString expressionString = expressionToString(static_cast<UtilityTraceConditionalExpression*>(m_initialExpression));
    qDebug() << expressionString;
    m_expressionBuilder = expressionString;
    emit expressionBuilderChanged();


    //* need to think about how to display the built expression
    //    m_conditionBarrierExpressionListModel.append(expressionString);
    //    emit conditionBarrierExpressionChanged();

    //    traceConfiguration.traversability.scope = Enums.UtilityTraversabilityScopeJunctions;

    //    let expression = expressionToString(initialExpression);
    //    print(expression); //* remove

    //    myModel.append({condition: expression});

  });

  connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, &ConfigureSubnetworkTrace::onTraceCompleted);

  m_utilityNetwork->load();
}

//* displaying information is incorrect need to look at closer
QString ConfigureSubnetworkTrace::expressionToString(UtilityTraceConditionalExpression* expression)
{
  switch (expression->traceConditionType()) {
    case UtilityTraceConditionType::UtilityNetworkAttributeComparison:
    {
      qDebug() << "UtilityNetworkAttributeComparison";

      UtilityNetworkAttributeComparison* attributeExpression = static_cast<UtilityNetworkAttributeComparison*>(expression);
      UtilityNetworkAttribute* networkAttribute = attributeExpression->networkAttribute();
      qDebug() << "NetAtt_Name - " <<networkAttribute->name();
      UtilityNetworkAttribute* otherNetworkAttribute = attributeExpression->otherNetworkAttribute();
      Domain networkDomain = networkAttribute->domain();

      if (!networkDomain.isEmpty() && (networkDomain.domainType() == DomainType::CodedValueDomain))
      {
        CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(networkDomain);
        QList<CodedValue> codedValues = codedValueDomain.codedValues();
        return QString("`%1` %2 `%3`").arg(networkAttribute->name(),comparisonOperatorToString(attributeExpression->comparisonOperator()), codedValues[attributeExpression->value().toInt()].name());
      }
      else
      {
        if (otherNetworkAttribute)
        {
          return QString("`%1` %2 `%3`").arg(networkAttribute->name(),comparisonOperatorToString(attributeExpression->comparisonOperator()),otherNetworkAttribute->name());
        }
        return QString("`%1` %2 `%3`").arg(networkAttribute->name(),comparisonOperatorToString(attributeExpression->comparisonOperator()),attributeExpression->value().toString());
      }
    }
    case UtilityTraceConditionType::UtilityCategoryComparison:
    {
      qDebug() << "UtilityCategoryComparison";

      UtilityCategoryComparison* comparisonExpression = static_cast<UtilityCategoryComparison*>(expression);

      return QString("`%1` %2").arg(comparisonExpression->category()->name(), (comparisonExpression->comparisonOperator() == UtilityCategoryComparisonOperator::Exists ) ? "Exists" : "DoesNotExist" );
    }
    case UtilityTraceConditionType::UtilityTraceAndCondition:
    {
      qDebug() << "UtilityTraceAndCondition";

      UtilityTraceAndCondition* andExpression = static_cast<UtilityTraceAndCondition*>(expression);

      //      return QString("'%1' AND\n '%2'").arg(expressionToString(andExpression->leftExpression()), expressionToString(andExpression->rightExpression()));
      return QString("(%1) AND\n (%2)").arg(expressionToString(andExpression->leftExpression()), expressionToString(andExpression->rightExpression()));
    }
    case UtilityTraceConditionType::UtilityTraceOrCondition:
    {

      qDebug() << "UtilityTraceOrCondition";

      UtilityTraceOrCondition* orExpression = static_cast<UtilityTraceOrCondition*>(expression);
      return QString("(%1) OR\n (%2)").arg(expressionToString(orExpression->leftExpression()), expressionToString(orExpression->rightExpression()));
    }

  }

  return "";
}

QString ConfigureSubnetworkTrace::comparisonOperatorToString(const UtilityAttributeComparisonOperator& comparisonOperator)
{
  switch (comparisonOperator) {
    case UtilityAttributeComparisonOperator::Equal:
      return QString("Equal");
    case UtilityAttributeComparisonOperator::NotEqual:
      return QString("NotEqual");
    case UtilityAttributeComparisonOperator::GreaterThan:
      return QString("GreaterThan");
    case UtilityAttributeComparisonOperator::GreaterThanEqual:
      return QString("GreaterThanEqual");
    case UtilityAttributeComparisonOperator::LessThan:
      return QString("LessThan");
    case UtilityAttributeComparisonOperator::LessThanEqual:
      return QString("LessThanEqual");
    case UtilityAttributeComparisonOperator::IncludesTheValues:
      return QString("IncludesTheValues");
    case UtilityAttributeComparisonOperator::DoesNotIncludeTheValues:
      return QString("DoesNotIncludeTheValues");
    case UtilityAttributeComparisonOperator::IncludesAny:
      return QString("IncludesAny");
    case UtilityAttributeComparisonOperator::DoesNotIncludeAny:
      return QString("DoesNotIncludeAny");
  }
}



void ConfigureSubnetworkTrace::codedValueOrInputText(const QString& currentText)
{
  if (m_networkDefinition)
  {
    Domain domain = m_networkDefinition->networkAttribute(currentText)->domain();
    if (!domain.isEmpty() && (domain.domainType() == DomainType::CodedValueDomain))
    {
      qDebug() << QString("domain: %1").arg(domain.name());

      m_valueSelectionListModel.clear();

      CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(domain);

      for (CodedValue codedValue: codedValueDomain.codedValues())
      {
        m_valueSelectionListModel.append(codedValue.name());
      }

      m_textFieldVisible = false;
    }
    else
    {
      m_textFieldVisible = true;
    }
    emit valueSelectionListModelChanged();
    emit textFieldVisibleChanged();
  }
}

void ConfigureSubnetworkTrace::addCondition(const QString &selectedAttribute, int selectedOperator, double selectedValue)
{
  // is this even needed?
  if (!m_traceConfiguration)
    m_traceConfiguration = new UtilityTraceConfiguration(this);

  UtilityTraversability* traversability = m_traceConfiguration->traversability();
  if (!traversability)
    m_traceConfiguration->setTraversability(new UtilityTraversability(this));

  UtilityNetworkAttribute* selectedNetworkAttribute = m_networkDefinition->networkAttribute(selectedAttribute);

  qDebug() << "selectedNetAtt-Name  - " << selectedNetworkAttribute->name();

//  if ( static_cast<int>(selectedNetworkAttribute->dataType()) != 0 )
//  {
//    qDebug() << "type - " << static_cast<int>(selectedNetworkAttribute->dataType());
//    double temp = selectedValue;
//    UtilityAttributeComparisonOperator selectedOperatorEnum = static_cast<UtilityAttributeComparisonOperator>(selectedOperator);

//    UtilityTraceConditionalExpression* expression = new UtilityNetworkAttributeComparison(selectedNetworkAttribute, selectedOperatorEnum, temp, this);

//    qDebug() << expressionToString(expression);
//    return;
//  }
  qDebug() << "data type - " << static_cast<int>(selectedNetworkAttribute->dataType());

  UtilityAttributeComparisonOperator selectedOperatorEnum = static_cast<UtilityAttributeComparisonOperator>(selectedOperator);

  UtilityTraceConditionalExpression* expression = new UtilityNetworkAttributeComparison(selectedNetworkAttribute, selectedOperatorEnum, selectedValue, this);

  qDebug() << expressionToString(expression);

  if (traversability->barriers())
  {
    UtilityTraceConditionalExpression* otherExpression = static_cast<UtilityTraceConditionalExpression*>(m_traceConfiguration->traversability()->barriers());

    UtilityTraceConditionalExpression* combineExpressions = new UtilityTraceOrCondition(otherExpression, expression, this);

    m_expressionBuilder = expressionToString(combineExpressions);
    emit expressionBuilderChanged();

    qDebug() << m_expressionBuilder;

    m_traceConfiguration->traversability()->setBarriers(combineExpressions);

  }
}

void ConfigureSubnetworkTrace::changeIncludeBarriersState()
{
  qDebug() << "changeIncludeBarriersState";
  // barriers are included by default, i.e. true
  if (m_traceConfiguration)
    m_traceConfiguration->setIncludeBarriers(!m_traceConfiguration->isIncludeBarriers());
}

void ConfigureSubnetworkTrace::changeIncludeContainersState()
{
  qDebug() << "changeIncludeContainersState";
  // containers are included by default, i.e. true
  if (m_traceConfiguration)
    m_traceConfiguration->setIncludeContainers(!m_traceConfiguration->isIncludeContainers());
}

void ConfigureSubnetworkTrace::reset()
{
  qDebug() << "Reset";
  m_traceConfiguration->traversability()->setBarriers(m_initialExpression);
  m_expressionBuilder.clear();
  m_expressionBuilder = expressionToString(static_cast<UtilityTraceConditionalExpression*>(m_initialExpression));
  emit expressionBuilderChanged();
}

void ConfigureSubnetworkTrace::trace()
{
  qDebug() << "Trace";

  if (!m_utilityNetwork || !m_utilityElementStartingLocation)
  {
    return;
  }
  else
  {
    QList<UtilityElement*> startingLocations {m_utilityElementStartingLocation};
    m_traceParams = new UtilityTraceParameters(UtilityTraceType::Subnetwork, startingLocations, this);
    m_traceParams->setTraceConfiguration(m_traceConfiguration);
    m_utilityNetwork->trace(m_traceParams);
  }
}

void ConfigureSubnetworkTrace::onTraceCompleted()
{
  if (m_utilityNetwork->traceResult()->isEmpty())
  {
    m_dialogText = QString("No results returned");
    emit dialogTextChanged();
    m_dialogVisible = true;
    emit dialogVisibleChanged();
  }
  UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);

  const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);

  m_dialogText = QString("%1 elements found.").arg(elements.length());
  emit dialogTextChanged();
  m_dialogVisible = true;
  emit dialogVisibleChanged();
}

ConfigureSubnetworkTrace::~ConfigureSubnetworkTrace() = default;

void ConfigureSubnetworkTrace::init()
{
  // Register the map view for QML
  //  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");

  // MapQuickView.h needed for this and I don't know why
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
