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
#include "IdentifyLayerResult.h"

namespace Esri::ArcGISRuntime {
  class ArcGISFeature;
  class FeatureLayer;
  class LabelDefinition;
  class Map;
  class MapQuickView;
  class ServiceGeodatabase;
  class ServiceVersionParameters;
  class UtilityAssetType;
  class UtilityElement;
  class UtilityNetwork;
  class UtilityNetworkState;
  class UtilityTraceParameters;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");

class ValidateUtilityNetworkTopology : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
  Q_PROPERTY(QList<QString> choices MEMBER m_choices NOTIFY choicesChanged)
  Q_PROPERTY(bool updateWindowVisibility MEMBER m_updateWindowVisibility NOTIFY updateWindowVisibilityChanged)
  Q_PROPERTY(QString fieldName MEMBER m_fieldName NOTIFY fieldNameChanged)
  Q_PROPERTY(bool validateButtonAvailability MEMBER m_validateButtonAvailability NOTIFY validateButtonAvailabilityChanged)
  Q_PROPERTY(bool traceButtonAvailability MEMBER m_traceButtonAvailability NOTIFY traceButtonAvailabilityChanged)
  Q_PROPERTY(bool clearButtonAvailability MEMBER m_clearButtonAvailability NOTIFY clearButtonAvailabilityChanged)
  Q_PROPERTY(bool progressBarVisibility MEMBER m_progressBarVisibility NOTIFY progressBarVisibilityChanged)
  Q_PROPERTY(bool stateButtonAvailability MEMBER m_stateButtonAvailability NOTIFY stateButtonAvailabilityChanged)

public:
  explicit ValidateUtilityNetworkTopology(QObject* parent = nullptr);
  ~ValidateUtilityNetworkTopology() override;

  static void init();
  Q_INVOKABLE void onApplyEdits(const QString& choice);
  Q_INVOKABLE void onClear();
  Q_INVOKABLE void onGetState();
  Q_INVOKABLE void onValidate();
  Q_INVOKABLE void onTrace();

signals:
  void mapViewChanged();
  void messageChanged();
  void choicesChanged();
  void updateWindowVisibilityChanged();
  void fieldNameChanged();
  void validateButtonAvailabilityChanged();
  void traceButtonAvailabilityChanged();
  void progressBarVisibilityChanged();
  void clearButtonAvailabilityChanged();
  void stateButtonAvailabilityChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::LabelDefinition* createDeviceLabelDefinition();
  Esri::ArcGISRuntime::LabelDefinition* createLineLabelDefinition();
  void displayLabelDefinitions();
  void setupTraceParameters();
  void connectSignals();
  void onMapLoaded();
  void onServiceGeodatabaseLoaded(Esri::ArcGISRuntime::ServiceVersionParameters* params, const Esri::ArcGISRuntime::Error& loadError);
  void onIdentifyLayersAsyncCompleted(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& results);
  void updateMessage(QString message);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::UtilityAssetType* m_utilityAssetType = nullptr;
  Esri::ArcGISRuntime::UtilityElement* m_startingLocation = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_feature = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_featureToEdit = nullptr;
  QList<Esri::ArcGISRuntime::CodedValue> m_codedValues;
  Esri::ArcGISRuntime::UtilityNetworkState* m_utilityNetworkstate;
  Esri::ArcGISRuntime::UtilityTraceParameters* m_traceParameters;
  Esri::ArcGISRuntime::FeatureLayer* m_deviceFeatureLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_lineFeatureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  QString m_message;
  QString m_fieldName;
  QList<QString> m_choices;
  bool m_updateWindowVisibility = false;
  bool m_validateButtonAvailability = false;
  bool m_traceButtonAvailability = false;
  bool m_progressBarVisibility = false;
  bool m_clearButtonAvailability = false;
  bool m_stateButtonAvailability = false;
};

#endif // VALIDATEUTILITYNETWORKTOPOLOGY_H
