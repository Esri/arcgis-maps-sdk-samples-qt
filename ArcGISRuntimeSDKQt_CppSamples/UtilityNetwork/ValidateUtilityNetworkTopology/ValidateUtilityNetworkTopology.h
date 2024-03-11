// [WriteFile Name=ValidateUtilityNetworkTopology, Category=UtilityNetwork]
// [Legal]
// Copyright 2024 Esri.

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

#ifndef VALIDATEUTILITYNETWORKTOPOLOGY_H
#define VALIDATEUTILITYNETWORKTOPOLOGY_H

#include "CodedValueDomain.h"
#include "Field.h"
#include "Point.h"

namespace Esri::ArcGISRuntime {
  class ArcGISFeature;
  class ArcGISFeatureTable;
  class Credential;
  class FeatureLayer;
  class GraphicsOverlay;
  class LabelDefinition;
  class Layer;
  class Map;
  class MapQuickView;
  class Portal;
  class PortalItem;
  class ServiceGeodatabase;
  class SubtypeFeatureLayer;
  class SubtypeSublayer;
  class UtilityAssetType;
  class UtilityElement;
  class UtilityNetwork;
  class UtilityNetworkState;
  class UtilityTraceConfiguration;
  class UtilityTraceParameters;
}

#include <QUrl>
#include <QFuture>
#include <QObject>


Q_MOC_INCLUDE("MapQuickView.h");

class ValidateUtilityNetworkTopology : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
  Q_PROPERTY(QList<QString> choices MEMBER m_choices NOTIFY addChoices)
  Q_PROPERTY(bool isUpdateVisible MEMBER m_isUpdateVisible NOTIFY isUpdateVisible)
  Q_PROPERTY(QString updateFieldName MEMBER m_updateFieldName NOTIFY updateFieldName)
  Q_PROPERTY(bool validateBtn MEMBER m_validateBtn NOTIFY validateBtn)
  Q_PROPERTY(bool traceBtn MEMBER m_traceBtn NOTIFY traceBtn)
  Q_PROPERTY(bool clearBtn MEMBER m_clearBtn NOTIFY clearBtn)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY isBusy)
  Q_PROPERTY(bool stateBtn MEMBER m_stateBtn NOTIFY stateBtn)
  Q_PROPERTY(bool attributeValue MEMBER m_attributeValue NOTIFY attributeValue)

public:
  explicit ValidateUtilityNetworkTopology(QObject* parent = nullptr);
  ~ValidateUtilityNetworkTopology() override;

  static void init();
  Q_INVOKABLE void onApplyEdits(QString choice);
  Q_INVOKABLE void onClear();
  Q_INVOKABLE void onGetState();
  Q_INVOKABLE void onValidate();
  Q_INVOKABLE void onTrace();

signals:
  void mapViewChanged();
  void messageChanged();
  void addChoices();
  void isUpdateVisible();
  void updateFieldName();
  void validateBtn();
  void traceBtn();
  void isBusy();
  void clearBtn();
  void stateBtn();
  void attributeValue();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::LabelDefinition* createDeviceLabelDefinition();
  Esri::ArcGISRuntime::LabelDefinition* createLineLabelDefinition();
  void getLabelDefinitions();
  void setupTraceParameters();
  void connectSignals();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Credential* m_cred = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal;
  Esri::ArcGISRuntime::PortalItem* m_portalItem;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityAssetType* m_utilityAssetType = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_overlay = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_feature = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_featureToEdit = nullptr;
  Esri::ArcGISRuntime::Point m_clickPoint;
  Esri::ArcGISRuntime::UtilityTraceConfiguration* m_traceConfiguration = nullptr;
  QList<Esri::ArcGISRuntime::CodedValue> m_codedValues;
  Esri::ArcGISRuntime::CodedValueDomain m_codedValueDomain;
  Esri::ArcGISRuntime::Field m_field ;
  Esri::ArcGISRuntime::CodedValue m_choiceCodeValue;
  Esri::ArcGISRuntime::UtilityNetworkState* m_getstate;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParameters;
  QList<Esri::ArcGISRuntime::UtilityElement*> m_elements;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_deviceFeatureLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_lineFeatureLayer = nullptr;
  Esri::ArcGISRuntime::LabelDefinition* m_deviceLabelDefinition = nullptr;
  Esri::ArcGISRuntime::LabelDefinition* m_lineLabelDefinition = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  Esri::ArcGISRuntime::ArcGISFeatureTable* m_dirtyAreaTable = nullptr;
  QString m_message;
  QString m_updateFieldName;
  QList<QString> m_choices;
  bool m_isUpdateVisible;
  bool m_validateBtn;
  bool m_traceBtn;
  bool m_busy;
  bool m_clearBtn;
  bool m_stateBtn;
  int m_attributeValue;
  const QString m_lineTableName = "Electric Distribution Line";
  const QString m_deviceTableName = "Electric Distribution Device";
  const QString m_deviceStatusField = "devicestatus";
  const QString m_nominalVoltageField = "nominalvoltage";
};

#endif // VALIDATEUTILITYNETWORKTOPOLOGY_H
