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

#ifndef CONFIGURESUBNETWORKTRACE_H
#define CONFIGURESUBNETWORKTRACE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Error;
class Map;
class MapQuickView;
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
#include <QVariantList>
#include <QStringList>
#include <QUuid>
#include "UtilityNetworkAttributeComparison.h"
#include "UtilityNetworkAttribute.h"

#include <QQuickItem>

class ConfigureSubnetworkTrace : public QQuickItem  //* maybe change at end
{
  Q_OBJECT

////  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ConfigureSubnetworkTrace(QQuickItem* parent = nullptr); //* maybe change at end since

//class ConfigureSubnetworkTrace : public QObject
//{
//  Q_OBJECT

  //  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(bool dialogVisible MEMBER m_dialogVisible NOTIFY dialogVisibleChanged)
  Q_PROPERTY(bool textFieldVisible MEMBER m_textFieldVisible NOTIFY textFieldVisibleChanged)
  Q_PROPERTY(QString dialogText MEMBER m_dialogText NOTIFY dialogTextChanged)
  Q_PROPERTY(QString expressionBuilder MEMBER m_expressionBuilder NOTIFY expressionBuilderChanged)
  Q_PROPERTY(QStringList attributeListModel MEMBER m_attributeListModel NOTIFY attributeListModelChanged)
  Q_PROPERTY(QStringList conditionBarrierExpressionListModel MEMBER m_conditionBarrierExpressionListModel NOTIFY conditionBarrierExpressionChanged)
  Q_PROPERTY(QStringList valueSelectionListModel MEMBER m_valueSelectionListModel NOTIFY valueSelectionListModelChanged)

//public:
//  explicit ConfigureSubnetworkTrace(QObject* parent = nullptr);
  ~ConfigureSubnetworkTrace();

  static void init();

  Q_INVOKABLE void addCondition(const QString& selectedAttribute, int selectedOperator, const QVariant& selectedValue);
  Q_INVOKABLE void changeIncludeBarriersState(bool includeBarriers);
  Q_INVOKABLE void changeIncludeContainersState(bool includeContainers);
  Q_INVOKABLE void codedValueOrInputText(const QString& currentText);
  Q_INVOKABLE void reset();
  Q_INVOKABLE void trace();

signals:
  //  void mapViewChanged();
  void attributeListModelChanged();
  void busyChanged();
  void conditionBarrierExpressionChanged();
  void dialogTextChanged();
  void dialogVisibleChanged();
  void expressionBuilderChanged();
  void textFieldVisibleChanged();
  void valueSelectionListModelChanged();

private slots:
  void onTraceCompleted();
  void onUtilityNetworkLoaded(const Esri::ArcGISRuntime::Error& e);

private:

  QString comparisonOperatorToString(const Esri::ArcGISRuntime::UtilityAttributeComparisonOperator& comparisonOperator) const;
  QString expressionToString(Esri::ArcGISRuntime::UtilityTraceConditionalExpression* expression) const;
  QVariant convertToDataType(const QVariant& value, const Esri::ArcGISRuntime::UtilityNetworkAttributeDataType& dataType);

  Esri::ArcGISRuntime::UtilityElement* m_utilityElementStartingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkDefinition* m_networkDefinition = nullptr;
  Esri::ArcGISRuntime::UtilityTraceCondition* m_initialExpression = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParams = nullptr;

  QStringList m_attributeListModel;
  QStringList m_conditionBarrierExpressionListModel;
  QStringList m_valueSelectionListModel;

  bool m_busy;
  bool m_dialogVisible;
  bool m_textFieldVisible;
  const QString m_assetGroupName;
  const QString m_assetTypeName;
  const QString m_deviceTableName;
  const QString m_domainNetworkName;
  const QString m_tierName;
  const QUrl m_featureLayerUrl;
  const QUuid m_gloabId;
  QString m_dialogText;
  QString m_expressionBuilder;
};

#endif // CONFIGURESUBNETWORKTRACE_H
