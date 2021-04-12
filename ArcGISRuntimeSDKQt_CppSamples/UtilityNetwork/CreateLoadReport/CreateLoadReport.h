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

#ifndef CREATELOADREPORT_H
#define CREATELOADREPORT_H

namespace Esri
{
namespace ArcGISRuntime
{
class Credential;
class Map;
class MapQuickView;
class CodedValue;
class TaskWatcher;
class UtilityAssetType;
class UtilityCategory;
class UtilityElement;
class UtilityNetwork;
class UtilityNetworkAttribute;
class UtilityTier;
class UtilityTerminal;
class UtilityTraceCondition;
class UtilityTraceConditionalExpression;
class UtilityTraceConfiguration;
class UtilityTraceParameters;
}
}

#include <QObject>
#include <QVariantMap>
#include <QUuid>

class CreateLoadReport : public QObject
{
  Q_OBJECT


  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantMap phaseCust READ phaseCust NOTIFY loadReportUpdated)
  Q_PROPERTY(QVariantMap phaseLoad READ phaseLoad NOTIFY loadReportUpdated)

public:
  explicit CreateLoadReport(QObject* parent = nullptr);
  ~CreateLoadReport();

  static void init();
  Q_INVOKABLE void addPhase(QString phaseToAdd);
  Q_INVOKABLE void runReport(QStringList selectedPhaseNames);
  Q_INVOKABLE void reset();

signals:
  void mapViewChanged();
  void loadReportUpdated();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QVariantMap phaseCust();
  QVariantMap phaseLoad();
  Esri::ArcGISRuntime::UtilityElement* createStartingLocation();
  void initializeLoadReport();
  Esri::ArcGISRuntime::UtilityTraceConfiguration* createDefaultTraceConfiguration();
  Esri::ArcGISRuntime::UtilityTraceParameters* createDefaultTraceParameters();
  QList<Esri::ArcGISRuntime::CodedValue> createPhaseList();
  Esri::ArcGISRuntime::UtilityCategory* getUtilityCategory(const QString categoryName, const Esri::ArcGISRuntime::UtilityNetwork* utilityNetwork);
  Esri::ArcGISRuntime::UtilityTraceParameters* createTraceParametersWithCodedValue(Esri::ArcGISRuntime::CodedValue);
  void createTraceCompletedConnection();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Credential* m_cred = nullptr;
  Esri::ArcGISRuntime::UtilityAssetType* m_utilityAssetType = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTerminal* m_utilityTerminal = nullptr;
  QStringList m_activePhases;
  QList<Esri::ArcGISRuntime::CodedValue> m_phaseList;
  Esri::ArcGISRuntime::UtilityTraceConditionalExpression* m_baseCondition = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkAttribute* m_phasesCurrentAttribute = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParameters = nullptr;
  QVariantMap m_phaseCust;
  QVariantMap m_phaseLoad;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  Esri::ArcGISRuntime::UtilityTier* m_utilityTier = nullptr;
  QMap<QUuid, QString> m_tasks;

  QString m_networkSourceName;
  QString m_assetGroupName;
  QString m_assetTypeName;
  QString m_terminalName;
  QString m_globalId;
  QString m_domainNetworkName;
  QString m_tierName;
  QString m_serviceCategoryName;
  QString m_loadNetworkAttributeName;
  QString m_phasesNetworkAttributeName;
};

#endif // CREATELOADREPORT_H
