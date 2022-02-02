// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef CONTINGENTVALUES_H
#define CONTINGENTVALUES_H

namespace Esri
{
namespace ArcGISRuntime
{
class ArcGISFeature;
class CodedValue;
class ContingentValuesDefinition;
class FeatureLayer;
class Geodatabase;
class GeodatabaseFeatureTable;
class GraphicsOverlay;
class Map;
class MapQuickView;
class ServiceFeatureTable;
class Symbol;
}
}

#include "Point.h"

#include <QObject>
#include <QMap>
#include <QList>
#include <QStringList>

class ContingentValues : public QObject
{
  Q_OBJECT


  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantMap featureAttributes READ featureAttributes WRITE setFeatureAttributes NOTIFY featureAttributesChanged)
  Q_PROPERTY(QVariantMap codedValueDomains READ codedValueDomains NOTIFY domainsChanged)
  Q_PROPERTY(QVariantMap rangeDomains READ rangeDomains NOTIFY domainsChanged)

  Q_PROPERTY(QStringList activityValues READ activityValues WRITE setActivityValues NOTIFY featureValuesChanged)
  Q_PROPERTY(QStringList protectionValues READ protectionValues WRITE setProtectionValues NOTIFY featureValuesChanged)
  Q_PROPERTY(QVariantList bufferSizeValues READ bufferSizeValues NOTIFY featureValuesChanged)

  Q_PROPERTY(bool featureAttributesPaneVisibe READ featureAttributesPaneVisibe WRITE setFeatureAttributesPaneVisibe NOTIFY featureAttributesPaneVisibeChanged)
  Q_PROPERTY(double featureAttributesPaneX READ featureAttributesPaneX NOTIFY featureAttributesPaneVisibeChanged)
  Q_PROPERTY(double featureAttributesPaneY READ featureAttributesPaneY NOTIFY featureAttributesPaneVisibeChanged)



public:
  explicit ContingentValues(QObject* parent = nullptr);
  ~ContingentValues();

  static void init();

  Q_INVOKABLE void modifyFeatures(QVariantMap attributes, QString modificationType);
  Q_INVOKABLE bool validateContingentValues();
  Q_INVOKABLE QStringList getContingentValues(QString field, QString fieldGroupName);
  Q_INVOKABLE void updateField(QString field, QVariant value);
  // Q_INVOKABLE void createNewNest();

signals:
  void mapViewChanged();
  void featureAttributesChanged();
  void featureAttributesPaneVisibeChanged();
  void domainsChanged();
  void featureValuesChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  //QVariantMap contingentValuesMap() const;

  double featureAttributesPaneX() const;
  double featureAttributesPaneY() const;

  QVariantMap featureAttributes() const;
  void setFeatureAttributes(QVariantMap featureAttributes);
  void setActivityValues(QStringList activityValues);
  void setProtectionValues(QStringList protectionValues);

  bool featureAttributesPaneVisibe() const;
  void setFeatureAttributesPaneVisibe(bool showFeatureAttributesPane);

  void createNewEmptyFeature();

  QVariantMap codedValueDomains() const;
  QVariantMap rangeDomains() const;

  QStringList activityValues() const;
  QStringList protectionValues() const;
  QVariantList bufferSizeValues() const;


  void deleteNest(Esri::ArcGISRuntime::ArcGISFeature* nestFeature);
  void bufferFeatures();
  void createConnections();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_serviceFeatureTable = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Symbol* m_bufferSymbol = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_nestsLayer = nullptr;
  Esri::ArcGISRuntime::ContingentValuesDefinition* m_contingentValuesDefinition = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
  Esri::ArcGISRuntime::GeodatabaseFeatureTable* m_gdbFeatureTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_newFeature = nullptr;
  bool m_featureAttributesPaneVisible = false;
  double m_featureAttributesPaneX;
  double m_featureAttributesPaneY;
  QVariantMap m_featureAttributes = {};
  QVariantMap m_codedValueDomains;
  QVariantMap m_rangeDomains;
  bool m_valuesAreValid;
  QStringList m_fieldGroups;

  QStringList m_activityValues;
  QStringList m_protectionValues;
  QVariantList m_bufferSizeValues;

  QStringList fields();
  QStringList validContingentValues();
};

#endif // CONTINGENTVALUES_H
