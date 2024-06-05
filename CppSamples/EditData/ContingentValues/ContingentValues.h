// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2022 Esri.

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

namespace Esri::ArcGISRuntime
{
class ArcGISFeature;
class FeatureQueryResult;
class Geodatabase;
class GeodatabaseFeatureTable;
class GraphicsOverlay;
class Map;
class MapQuickView;
}

#include <QObject>
#include <QMouseEvent>

Q_MOC_INCLUDE("MapQuickView.h")

class ContingentValues : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantMap statusValues READ statusValues CONSTANT)
  Q_PROPERTY(bool featureAttributesPaneVisibe READ featureAttributesPaneVisibe WRITE setFeatureAttributesPaneVisibe NOTIFY featureAttributesPaneVisibeChanged)

public:
  explicit ContingentValues(QObject* parent = nullptr);
  ~ContingentValues();

  static void init();

  Q_INVOKABLE void createNewNest();
  Q_INVOKABLE void discardFeature();
  Q_INVOKABLE QVariantList getContingentValues(const QString& field, const QString& fieldGroupName);
  Q_INVOKABLE void updateField(const QString& field, const QVariant& value);
  Q_INVOKABLE bool validateContingentValues();

signals:
  void featureAttributesPaneVisibeChanged();
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  void queryAndBufferFeatures();
  void bufferFeaturesFromQueryResults(Esri::ArcGISRuntime::FeatureQueryResult* results);
  void createConnections();
  void createNewEmptyFeature(QMouseEvent& mouseEvent);
  bool featureAttributesPaneVisibe() const;
  QVariantMap statusValues() const;
  void setFeatureAttributesPaneVisibe(bool showFeatureAttributesPane);

  Esri::ArcGISRuntime::ArcGISFeature* m_newFeature = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
  Esri::ArcGISRuntime::GeodatabaseFeatureTable* m_gdbFeatureTable = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  QVariantMap m_statusValues;
  QVariantMap m_protectionValues;
  bool m_featureAttributesPaneVisible = false;
};

#endif // CONTINGENTVALUES_H
