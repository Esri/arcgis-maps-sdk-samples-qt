// [WriteFile Name=ConfigureSubnetworkTrace, Category=UtilityNetwork]
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

#include "ConfigureSubnetworkTrace.h"

#include "CodedValueDomain.h"
#include "TaskWatcher.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityCategory.h"
#include "UtilityCategoryComparison.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkTypes.h"
#include "UtilityTerminal.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceAndCondition.h"
#include "UtilityTraceConfiguration.h"
#include "UtilityTraceOrCondition.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityTraversability.h"

#include <QQmlEngine>
#include <algorithm>

using namespace Esri::ArcGISRuntime;

ConfigureSubnetworkTrace::ConfigureSubnetworkTrace(QObject* parent /* = nullptr */):
  QObject(parent)
{
  m_utilityNetwork = new UtilityNetwork(m_featureLayerUrl, this);

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, &ConfigureSubnetworkTrace::onUtilityNetworkLoaded);

  connect(m_utilityNetwork, &UtilityNetwork::traceCompleted, this, &ConfigureSubnetworkTrace::onTraceCompleted);

  m_utilityNetwork->load();
}

QString ConfigureSubnetworkTrace::expressionToString(UtilityTraceConditionalExpression* expression)
{
  switch (expression->traceConditionType())
  {
    case UtilityTraceConditionType::UtilityNetworkAttributeComparison:
    {
      const UtilityNetworkAttributeComparison* attributeExpression = static_cast<UtilityNetworkAttributeComparison*>(expression);
      const UtilityNetworkAttribute* networkAttribute = attributeExpression->networkAttribute();
      const UtilityNetworkAttribute* otherNetworkAttribute = attributeExpression->otherNetworkAttribute();
      const Domain networkDomain = networkAttribute->domain();
      const QString operatorAsString = comparisonOperatorToString(attributeExpression->comparisonOperator());

      // check if attribute domain is a coded value domain.
      if (!networkDomain.isEmpty() && (networkDomain.domainType() == DomainType::CodedValueDomain))
      {
        const CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(networkDomain);
        const QList<CodedValue> codedValues = codedValueDomain.codedValues();

        // get the coded value using the value as the index for the list of coded values
        const QString codedValueName = codedValues[attributeExpression->value().toInt()].name();

        return QString("`%1` %2 `%3`").arg(networkAttribute->name(), operatorAsString, codedValueName);
      }
      else
      {
        if (otherNetworkAttribute)
        {
          return QString("`%1` %2 `%3`").arg(networkAttribute->name(), operatorAsString, otherNetworkAttribute->name());
        }
        return QString("`%1` %2 `%3`").arg(networkAttribute->name(), operatorAsString, attributeExpression->value().toString());
      }
    }
    case UtilityTraceConditionType::UtilityCategoryComparison:
    {
      const UtilityCategoryComparison* comparisonExpression = static_cast<UtilityCategoryComparison*>(expression);

      return QString("`%1` %2").arg(comparisonExpression->category()->name(), (comparisonExpression->comparisonOperator() == UtilityCategoryComparisonOperator::Exists) ? "Exists" : "DoesNotExist");
    }
    case UtilityTraceConditionType::UtilityTraceAndCondition:
    {
      const UtilityTraceAndCondition* andExpression = static_cast<UtilityTraceAndCondition*>(expression);

      return QString("(%1) AND\n (%2)").arg(expressionToString(andExpression->leftExpression()), expressionToString(andExpression->rightExpression()));
    }
    case UtilityTraceConditionType::UtilityTraceOrCondition:
    {
      const UtilityTraceOrCondition* orExpression = static_cast<UtilityTraceOrCondition*>(expression);
      return QString("(%1) OR\n (%2)").arg(expressionToString(orExpression->leftExpression()), expressionToString(orExpression->rightExpression()));
    }
    default:
      return QString("Unknown trace conditional expression");
  }
}

QString ConfigureSubnetworkTrace::comparisonOperatorToString(const UtilityAttributeComparisonOperator& comparisonOperator)
{
  switch (comparisonOperator)
  {
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
    default:
      return QString("Unknown comparison operator");
  }
}

QVariant ConfigureSubnetworkTrace::convertToDataType(const QVariant& value, const Esri::ArcGISRuntime::UtilityNetworkAttributeDataType& dataType)
{
  switch (dataType)
  {
    case UtilityNetworkAttributeDataType::Integer:
    {
      // inconsistent results when using QVariant.toInt() on a
      // QString that doesn't contain an Integer dataType.
      // e.g. QVariant(QString("123.321")).toInt();
      return static_cast<int>(value.toDouble());
    }
    case UtilityNetworkAttributeDataType::Float:
    {
      return value.toFloat();
    }
    case UtilityNetworkAttributeDataType::Double:
    {
      return value.toDouble();
    }
    case UtilityNetworkAttributeDataType::Boolean:
    {
      return value.toBool();
    }
    default:
      return QVariant();
  }
}

void ConfigureSubnetworkTrace::codedValueOrInputText(const QString& currentText)
{
  // Update the UI to show the correct value entry for the attribute.
  if (m_networkDefinition)
  {
    const Domain domain = m_networkDefinition->networkAttribute(currentText)->domain();
    if (!domain.isEmpty() && (domain.domainType() == DomainType::CodedValueDomain))
    {
      m_valueSelectionListModel.clear();
      const CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(domain);

      for (CodedValue codedValue: codedValueDomain.codedValues())
        m_valueSelectionListModel.append(codedValue.name());

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

void ConfigureSubnetworkTrace::addCondition(const QString& selectedAttribute, int selectedOperator, const QVariant& selectedValue)
{
  // NOTE: You may also create a UtilityCategoryComparison with UtilityNetworkDefinition.Categories and UtilityCategoryComparisonOperator.

  UtilityNetworkAttribute* selectedNetworkAttribute = m_networkDefinition->networkAttribute(selectedAttribute);
  const QVariant convertedSelectedValue = convertToDataType(selectedValue, selectedNetworkAttribute->dataType());

  if (convertedSelectedValue.isNull())
  {
    m_dialogText = "Unknow network attribute data type";
    emit dialogTextChanged();
    emit showDialog();
    return;
  }

  const UtilityAttributeComparisonOperator selectedOperatorEnum = static_cast<UtilityAttributeComparisonOperator>(selectedOperator);

  // NOTE: You may also create a UtilityNetworkAttributeComparison with another NetworkAttribute.
  UtilityTraceConditionalExpression* expression = new UtilityNetworkAttributeComparison(selectedNetworkAttribute, selectedOperatorEnum, convertedSelectedValue, this);

  UtilityTraceConditionalExpression* otherExpression = static_cast<UtilityTraceConditionalExpression*>(m_traceConfiguration->traversability()->barriers());

  // NOTE: You may also combine expressions with UtilityTraceAndCondition
  UtilityTraceConditionalExpression* combineExpressions = new UtilityTraceOrCondition(otherExpression, expression, this);

  m_expressionBuilder = expressionToString(combineExpressions);
  emit expressionBuilderChanged();

  if (m_traceConfiguration)
    m_traceConfiguration->traversability()->setBarriers(combineExpressions);
}

void ConfigureSubnetworkTrace::changeIncludeBarriersState(bool includeBarriers)
{
  if (m_traceConfiguration)
    m_traceConfiguration->setIncludeBarriers(includeBarriers);
}

void ConfigureSubnetworkTrace::changeIncludeContainersState(bool includeContainers)
{
  if (m_traceConfiguration)
    m_traceConfiguration->setIncludeContainers(includeContainers);
}

void ConfigureSubnetworkTrace::reset()
{
  // Reset the barrier condition to the initial value.
  m_traceConfiguration->traversability()->setBarriers(m_initialExpression);
  m_expressionBuilder.clear();
  m_expressionBuilder = expressionToString(static_cast<UtilityTraceConditionalExpression*>(m_initialExpression));
  emit expressionBuilderChanged();
}

void ConfigureSubnetworkTrace::trace()
{
  if (!m_utilityNetwork || !m_utilityElementStartingLocation)
  {
    return;
  }

  m_busy = true;
  emit busyChanged();
  const QList<UtilityElement*> startingLocations {m_utilityElementStartingLocation};
  // Create utility trace parameters for the starting location.
  m_traceParams = new UtilityTraceParameters(UtilityTraceType::Subnetwork, startingLocations, this);
  m_traceParams->setTraceConfiguration(m_traceConfiguration);

  // trace the network
  m_utilityNetwork->trace(m_traceParams);
}

void ConfigureSubnetworkTrace::onTraceCompleted()
{
  if (m_utilityNetwork->traceResult()->isEmpty())
  {
    m_dialogText = "No results returned";
    emit dialogTextChanged();
    emit showDialog();
    return;
  }
  // Get the first result.
  UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);

  const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);

  // Display the number of elements found by the trace.
  m_dialogText = QString("%1 elements found.").arg(elements.length());
  m_busy = false;
  emit dialogTextChanged();
  emit showDialog();
  emit busyChanged();
}

void ConfigureSubnetworkTrace::onUtilityNetworkLoaded(const Error& e)
{
  if (!e.isEmpty())
  {
    m_dialogText = QString("%1 - %2").arg(e.message(), e.additionalMessage());
    m_busy = false;
    emit dialogTextChanged();
    emit showDialog();
    emit busyChanged();
    return;
  }

  m_busy = false;
  emit busyChanged();
  m_networkDefinition = m_utilityNetwork->definition();

  // Build the choice lists for network attribute comparison.
  for (UtilityNetworkAttribute* networkAttribute : m_networkDefinition->networkAttributes())
  {
    if (!networkAttribute->isSystemDefined())
      m_attributeListModel.append(networkAttribute->name());
  }
  emit attributeListModelChanged();

  // Create a default starting location.
  const UtilityNetworkSource* networkSource = m_networkDefinition->networkSource(m_deviceTableName);
  const UtilityAssetGroup* assetGroup = networkSource->assetGroup(m_assetGroupName);
  UtilityAssetType* assetType = assetGroup->assetType(m_assetTypeName);
  m_utilityElementStartingLocation = m_utilityNetwork->createElementWithAssetType(assetType, m_gloabId);

  QList<UtilityTerminal*> terminals = m_utilityElementStartingLocation->assetType()->terminalConfiguration()->terminals();

  // Set the terminal for this location. (For our case, we use the 'Load' terminal.)
  auto terminal = std::find_if(terminals.begin(), terminals.end(), [](UtilityTerminal* terminal)
  {
      return terminal->name() == "Load";
  });

  m_utilityElementStartingLocation->setTerminal(static_cast<UtilityTerminal*>(*terminal));

  // Get a default trace configuration from a tier to update the UI.
  const UtilityDomainNetwork* domainNetwork = m_networkDefinition->domainNetwork(m_domainNetworkName);
  const UtilityTier* utilityTierSource = domainNetwork->tier(m_tierName);

  // Set the trace configuration.
  m_traceConfiguration = utilityTierSource->traceConfiguration();

  m_initialExpression = m_traceConfiguration->traversability()->barriers();

  if (m_initialExpression)
  {
    m_expressionBuilder = expressionToString(static_cast<UtilityTraceConditionalExpression*>(m_initialExpression));
    emit expressionBuilderChanged();
  }

  // Set the traversability scope.
  utilityTierSource->traceConfiguration()->traversability()->setScope(UtilityTraversabilityScope::Junctions);
}

ConfigureSubnetworkTrace::~ConfigureSubnetworkTrace() = default;

void ConfigureSubnetworkTrace::init()
{
  qmlRegisterType<ConfigureSubnetworkTrace>("Esri.Samples", 1, 0, "ConfigureSubnetworkTraceSample");
}
