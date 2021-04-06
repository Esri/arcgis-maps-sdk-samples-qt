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
class UtilityAssetType;
class UtilityElement;
class UtilityNetwork;
class UtilityNetworkAttribute;
class UtilityTerminal;
class UtilityTraceCondition;
class UtilityTraceConditionalExpression;
class UtilityTraceParameters;
}
}

#include <QObject>
#include <QUuid>

class CreateLoadReport : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QList<QStringList> phases READ getPhases WRITE setPhases NOTIFY phaseListChanged)

public:
  explicit CreateLoadReport(QObject* parent = nullptr);
  ~CreateLoadReport();

  static void init();
  Q_INVOKABLE void addPhase();
  Q_INVOKABLE void runReport();
  Q_INVOKABLE void reset();
  Q_INVOKABLE void setPhases(QList<QStringList>);

signals:
  void mapViewChanged();
  void phaseListChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QList<QStringList> getPhases();
  Esri::ArcGISRuntime::UtilityElement* createStartingLocation();
  void initializeLoadReport();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Credential* m_cred = nullptr;
  Esri::ArcGISRuntime::UtilityAssetType* m_utilityAssetType = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityTerminal* m_utilityTerminal = nullptr;
  QList<Esri::ArcGISRuntime::CodedValue> m_phaseList;
  Esri::ArcGISRuntime::UtilityTraceConditionalExpression* m_baseCondition = nullptr;
  Esri::ArcGISRuntime::UtilityNetworkAttribute* m_phasesCurrentAttribute = nullptr;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParameters = nullptr;

  QUuid m_globalId;
};

#endif // CREATELOADREPORT_H
