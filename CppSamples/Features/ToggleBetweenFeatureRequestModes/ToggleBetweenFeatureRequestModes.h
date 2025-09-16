
#ifndef TOGGLEBETWEENFEATUREREQUESTMODES_H
#define TOGGLEBETWEENFEATUREREQUESTMODES_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class FeatureLayer;
  class ServiceFeatureTable;
  enum class FeatureRequestMode;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class ToggleBetweenFeatureRequestModes : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ToggleBetweenFeatureRequestModes(QObject* parent = nullptr);
  ~ToggleBetweenFeatureRequestModes() override;

  static void init();

  Q_INVOKABLE void setFeatureRequestMode(Esri::ArcGISRuntime::FeatureRequestMode featureRequestMode);
  Q_INVOKABLE void fetchCacheManually();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_treeFeatureTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_treeFeatureLayer = nullptr;
};

#endif // TOGGLEBETWEENFEATUREREQUESTMODES_H
