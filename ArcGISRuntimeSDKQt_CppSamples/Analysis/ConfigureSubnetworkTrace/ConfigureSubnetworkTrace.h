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

//#include <QQuickItem>

//class ConfigureSubnetworkTrace : public QQuickItem  //* maybe change at end
//{
//  Q_OBJECT

////  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

//public:
//  explicit ConfigureSubnetworkTrace(QQuickItem* parent = nullptr); //* maybe change at end since

class ConfigureSubnetworkTrace : public QObject
{
  Q_OBJECT

  //  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList attributeListModel MEMBER m_attributeListModel NOTIFY attributeListModelChanged)
  Q_PROPERTY(QStringList valueSelectionListModel MEMBER m_valueSelectionListModel NOTIFY valueSelectionListModelChanged)
  Q_PROPERTY(QStringList conditionBarrierExpressionListModel MEMBER m_conditionBarrierExpressionListModel NOTIFY conditionBarrierExpressionChanged)
  Q_PROPERTY(bool textFieldVisible MEMBER m_textFieldVisible NOTIFY textFieldVisibleChanged)
  Q_PROPERTY(QString expressionBuilder MEMBER m_expressionBuilder NOTIFY expressionBuilderChanged)
  Q_PROPERTY(bool dialogVisible MEMBER m_dialogVisible NOTIFY dialogVisibleChanged)
  Q_PROPERTY(QString dialogText MEMBER m_dialogText NOTIFY dialogTextChanged)

public:
  explicit ConfigureSubnetworkTrace(QObject* parent = nullptr);
  ~ConfigureSubnetworkTrace();

  static void init();

  Q_INVOKABLE void codedValueOrInputText(const QString& currentText);
  Q_INVOKABLE void addCondition(const QString& selectedAttribute, int selectedOperator, double selectedValue);
  Q_INVOKABLE void changeIncludeBarriersState();
  Q_INVOKABLE void changeIncludeContainersState();
  Q_INVOKABLE void reset();
  Q_INVOKABLE void trace();

signals:
  //  void mapViewChanged();
  void attributeListModelChanged();
  void valueSelectionListModelChanged();
  void conditionBarrierExpressionChanged();
  void textFieldVisibleChanged();
  void expressionBuilderChanged();
  void dialogVisibleChanged();
  void dialogTextChanged();

private slots:
  void onTraceCompleted();

private:

  QString expressionToString(Esri::ArcGISRuntime::UtilityTraceConditionalExpression* expression);
  QString comparisonOperatorToString(const Esri::ArcGISRuntime::UtilityAttributeComparisonOperator& comparisonOperator);
//  Esri::ArcGISRuntime::UtilityAttributeComparisonOperator

  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParams = nullptr;
  Esri::ArcGISRuntime::UtilityTraceCondition* m_initialExpression = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkDefinition* m_networkDefinition = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_utilityElementStartingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;

  QStringList m_attributeListModel;
  QStringList m_valueSelectionListModel;
  QStringList m_conditionBarrierExpressionListModel;

  const QUrl m_featureLayerUrl = QUrl("https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer");
  const QString m_deviceTableName = "Electric Distribution Device";
  const QString m_assetGroupName = "Circuit Breaker";
  const QString m_assetTypeName = "Three Phase";
  const QUuid m_gloabId = QUuid("{1CAF7740-0BF4-4113-8DB2-654E18800028}");
  bool m_textFieldVisible = true;
  bool m_dialogVisible = false;
  QString m_dialogText;
  QString m_expressionBuilder;


  // For creating the default trace configuration.
  const QString m_domainNetworkName = "ElectricDistribution";
  const QString m_tierName = "Medium Voltage Radial";
  //  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  //  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  //  Esri::ArcGISRuntime::Map* m_map = nullptr;
  //  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // CONFIGURESUBNETWORKTRACE_H
