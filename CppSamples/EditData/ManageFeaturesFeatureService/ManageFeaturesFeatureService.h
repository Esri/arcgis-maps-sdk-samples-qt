
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

  Q_PROPERTY(QString featureType READ featureType NOTIFY featureTypeChanged)
  Q_PROPERTY(OperationMode operationMode READ operationMode WRITE setOperationMode NOTIFY operationModeChanged)

public:
  enum class OperationMode
  {
    AddFeatures,
    DeleteFeatures,
    UpdateAttributes,
    UpdateGeometry
  };
  Q_ENUM(OperationMode)

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
  void connectSignals_();
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
