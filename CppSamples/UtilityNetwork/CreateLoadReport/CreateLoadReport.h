// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef CREATELOADREPORT_H
#define CREATELOADREPORT_H

// ArcGIS Maps SDK headers
#include "Authentication/ArcGISAuthenticationChallengeHandler.h"

// Qt headers
#include <QObject>
#include <QUrl>
#include <QUuid>
#include <QVariantMap>

namespace Esri::ArcGISRuntime
{
class CodedValue;
class UtilityAssetType;
class UtilityCategory;
class UtilityElement;
class UtilityNetwork;
class UtilityNetworkAttribute;
class UtilityTier;
class UtilityTraceConditionalExpression;
class UtilityTraceConfiguration;
class UtilityTraceParameters;
}

namespace Esri::ArcGISRuntime::Authentication
{
  class ArcGISAuthenticationChallenge;
}

class CreateLoadReport : public Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallengeHandler
{
  Q_OBJECT

  Q_PROPERTY(QVariantMap phaseCust READ phaseCust NOTIFY loadReportUpdated)
  Q_PROPERTY(QVariantMap phaseLoad READ phaseLoad NOTIFY loadReportUpdated)
  Q_PROPERTY(int sampleStatus READ sampleStatus NOTIFY sampleStatusChanged)

public:
  explicit CreateLoadReport(QObject* parent = nullptr);
  ~CreateLoadReport();

  static void init();
  Q_INVOKABLE void addPhase(const QString& phaseToAdd);
  Q_INVOKABLE void runReport(const QStringList& selectedPhaseNames);

  enum SampleStatus
  {
    SampleError = -1,
    SampleNotLoaded = 0,
    SampleBusy = 1,
    SampleReady = 2
  };
  Q_ENUM(SampleStatus)

signals:
  void loadReportUpdated();
  void sampleStatusChanged();

private:
  int sampleStatus();
  QVariantMap phaseCust();
  QVariantMap phaseLoad();

  Esri::ArcGISRuntime::UtilityCategory* getUtilityCategory(const QString& categoryName);
  Esri::ArcGISRuntime::UtilityElement* createStartingLocation();
  Esri::ArcGISRuntime::UtilityTraceConfiguration* createDefaultTraceConfiguration();
  QList<Esri::ArcGISRuntime::CodedValue> createPhaseList();
  void onTraceCompleted_(const QString& codeValueName);
  void setUtilityTraceOrconditionWithCodedValue(Esri::ArcGISRuntime::CodedValue);

  void handleArcGISAuthenticationChallenge(Esri::ArcGISRuntime::Authentication::ArcGISAuthenticationChallenge* challenge) override;

  Esri::ArcGISRuntime::UtilityAssetType* m_utilityAssetType = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkAttribute* m_phasesCurrentAttribute = nullptr;
  Esri::ArcGISRuntime::UtilityTier* m_utilityTier = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConditionalExpression* m_baseCondition = nullptr;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParameters = nullptr;
  QList<Esri::ArcGISRuntime::CodedValue> m_phaseList;
  QStringList m_activePhases;
  QVariantMap m_phaseCust;
  QVariantMap m_phaseLoad;

  int m_sampleStatus;
  QString m_assetGroupName;
  QString m_assetTypeName;
  QString m_domainNetworkName;
  QString m_globalId;
  QString m_loadNetworkAttributeName;
  QString m_networkSourceName;
  QString m_phasesNetworkAttributeName;
  QString m_serviceCategoryName;
  QString m_terminalName;
  QString m_tierName;
  const QUrl m_featureLayerUrl;
  int m_traceRequestCount;
};

#endif // CREATELOADREPORT_H
