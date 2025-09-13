// [WriteFile Name=ManageFeaturesFeatureService, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef MANAGEFEATURESFEATURESERVICE_H
#define MANAGEFEATURESFEATURESERVICE_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class ArcGISFeature;
  class Feature;
  class FeatureLayer;
  class FeatureQueryResult;
  class FeatureTableEditResult;
  class IdentifyLayerResult;
  class Map;
  class MapQuickView;
  class Point;
  class ServiceGeodatabase;
} // namespace Esri::ArcGISRuntime

class QString;

class ManageFeaturesFeatureService : public QQuickItem
{
  Q_OBJECT

  enum class OperationMode
  {
    AddFeatures,
    DeleteFeatures,
    UpdateAttributes,
    UpdateGeometry
  };
  Q_ENUM(OperationMode)

  Q_PROPERTY(QString featureType READ featureType NOTIFY featureTypeChanged)
  Q_PROPERTY(OperationMode operationMode READ operationMode WRITE setOperationMode NOTIFY operationModeChanged)

public:
  explicit ManageFeaturesFeatureService(QQuickItem* parent = nullptr);
  ~ManageFeaturesFeatureService() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void deleteSelectedFeature();
  Q_INVOKABLE void updateSelectedFeature(const QString& fieldVal);

  OperationMode operationMode() const;
  void setOperationMode(OperationMode mode);
  QString featureType() const;

signals:
  void featureSelected();
  void featureTypeChanged();
  void hideWindow();
  void operationModeChanged();

private:
  void connectSignals();
  void onIdentifyLayerCompleted_(const Esri::ArcGISRuntime::IdentifyLayerResult* identifyResult);
  void onApplyEditsCompleted_(const QList<Esri::ArcGISRuntime::FeatureTableEditResult*>& tableEditResults);

  void handleAddFeatureClick_(const QMouseEvent& mouseEvent);
  void handleUpdateGeometryClick_(const QMouseEvent& mouseEvent);
  void handleIdentifyFeatureClick_(const QMouseEvent& mouseEvent);
  void updateSelectedFeatureGeometry_(const Esri::ArcGISRuntime::Point& mapPoint);
  void selectFeatureForGeometryUpdate_(const QMouseEvent& mouseEvent);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  QPointer<Esri::ArcGISRuntime::ArcGISFeature> m_selectedFeature;
  QString m_featureType;
  OperationMode m_operationMode = OperationMode::AddFeatures;
  bool m_featureSelected = false;
  QMetaObject::Connection m_featureLoadStatusChangedConnection;
};

#endif // MANAGEFEATURESFEATURESERVICE_H
