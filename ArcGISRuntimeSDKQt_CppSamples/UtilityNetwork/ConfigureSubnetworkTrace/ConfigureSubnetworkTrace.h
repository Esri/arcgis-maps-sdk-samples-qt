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

#ifndef CONFIGURESUBNETWORKTRACE_H
#define CONFIGURESUBNETWORKTRACE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Error;
class UtilityTraceCondition;
class UtilityElement;
class UtilityNetwork;
class UtilityNetworkDefinition;
class UtilityTerminal;
class UtilityTier;
class UtilityTraceParameters;
class UtilityTraceConditionalExpression;
class UtilityTraceConfiguration;
}
}

#include <QObject>
#include <QUrl>
#include <QUuid>

#include "UtilityNetworkAttributeComparison.h"
#include "UtilityNetworkAttribute.h"

class ConfigureSubnetworkTrace : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(bool textFieldVisible MEMBER m_textFieldVisible NOTIFY textFieldVisibleChanged)
  Q_PROPERTY(QString dialogText MEMBER m_dialogText NOTIFY dialogTextChanged)
  Q_PROPERTY(QString expressionBuilder MEMBER m_expressionBuilder NOTIFY expressionBuilderChanged)
  Q_PROPERTY(QStringList attributeListModel MEMBER m_attributeListModel NOTIFY attributeListModelChanged)
  Q_PROPERTY(QStringList conditionBarrierExpressionListModel MEMBER m_conditionBarrierExpressionListModel NOTIFY conditionBarrierExpressionChanged)
  Q_PROPERTY(QStringList valueSelectionListModel MEMBER m_valueSelectionListModel NOTIFY valueSelectionListModelChanged)

public:
  explicit ConfigureSubnetworkTrace(QObject* parent = nullptr);
  ~ConfigureSubnetworkTrace();

  static void init();

  Q_INVOKABLE void addCondition(const QString& selectedAttribute, int selectedOperator, const QVariant& selectedValue);
  Q_INVOKABLE void changeIncludeBarriersState(bool includeBarriers);
  Q_INVOKABLE void changeIncludeContainersState(bool includeContainers);
  Q_INVOKABLE void codedValueOrInputText(const QString& currentText);
  Q_INVOKABLE void reset();
  Q_INVOKABLE void trace();

signals:
  void attributeListModelChanged();
  void busyChanged();
  void conditionBarrierExpressionChanged();
  void dialogTextChanged();
  void showDialog();
  void expressionBuilderChanged();
  void textFieldVisibleChanged();
  void valueSelectionListModelChanged();

private slots:
  void onTraceCompleted();
  void onUtilityNetworkLoaded(const Esri::ArcGISRuntime::Error& e);

private:

  static QString comparisonOperatorToString(const Esri::ArcGISRuntime::UtilityAttributeComparisonOperator& comparisonOperator);
  static QString expressionToString(Esri::ArcGISRuntime::UtilityTraceConditionalExpression* expression);
  static QVariant convertToDataType(const QVariant& value, const Esri::ArcGISRuntime::UtilityNetworkAttributeDataType& dataType);

  Esri::ArcGISRuntime::UtilityElement* m_utilityElementStartingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkDefinition* m_networkDefinition = nullptr;
  Esri::ArcGISRuntime::UtilityTraceCondition* m_initialExpression = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParams = nullptr;

  QStringList m_attributeListModel;
  QStringList m_conditionBarrierExpressionListModel;
  QStringList m_valueSelectionListModel;

  bool m_busy = true;
  bool m_textFieldVisible = true;
  const QString m_assetGroupName = "Circuit Breaker";
  const QString m_assetTypeName = "Three Phase";
  const QString m_deviceTableName = "Electric Distribution Device";
  const QString m_domainNetworkName = "ElectricDistribution";
  const QString m_tierName = "Medium Voltage Radial";
  const QUrl m_featureLayerUrl = QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer");
  const QUuid m_gloabId = QUuid("{1CAF7740-0BF4-4113-8DB2-654E18800028}");
  QString m_dialogText;
  QString m_expressionBuilder;

};

#endif // CONFIGURESUBNETWORKTRACE_H
