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
class FeatureQueryResult;
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
#include <QMouseEvent>

class ContingentValues : public QObject
{
  Q_OBJECT


  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

  Q_PROPERTY(QVariantMap statusValues READ statusValues NOTIFY featureValuesChanged)

  Q_PROPERTY(bool featureAttributesPaneVisibe READ featureAttributesPaneVisibe WRITE setFeatureAttributesPaneVisibe NOTIFY featureAttributesPaneVisibeChanged)
  Q_PROPERTY(QList<double> featureAttributesPaneXY READ featureAttributesPaneXY NOTIFY featureAttributesPaneVisibeChanged)



public:
  explicit ContingentValues(QObject* parent = nullptr);
  ~ContingentValues();

  static void init();

  Q_INVOKABLE void createNewNest();
  Q_INVOKABLE bool validateContingentValues();
  Q_INVOKABLE QVariantList getContingentValues(QString field, QString fieldGroupName);
  Q_INVOKABLE void updateField(QString field, QVariant value);

signals:
  void mapViewChanged();
  void featureAttributesPaneVisibeChanged();
  void featureValuesChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  QList<double> featureAttributesPaneXY() const;

  bool featureAttributesPaneVisibe() const;
  void setFeatureAttributesPaneVisibe(bool showFeatureAttributesPane);

  void bufferFeaturesFromQueryResults(QUuid, Esri::ArcGISRuntime::FeatureQueryResult* results);

  QVariantMap statusValues() const;

  void deleteNest(Esri::ArcGISRuntime::ArcGISFeature* nestFeature);
  void bufferFeatures();
  void createConnections();

  void createNewEmptyFeature(QMouseEvent mouseEvent);

  Esri::ArcGISRuntime::ArcGISFeature* m_newFeature = nullptr;
  Esri::ArcGISRuntime::ContingentValuesDefinition* m_contingentValuesDefinition = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
  Esri::ArcGISRuntime::GeodatabaseFeatureTable* m_gdbFeatureTable = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  QVariantMap m_statusValues;
  QVariantMap m_protectionValues;
  bool m_featureAttributesPaneVisible = false;
  QList<double> m_featureAttributesPaneXY;
};

#endif // CONTINGENTVALUES_H
