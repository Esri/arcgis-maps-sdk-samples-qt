// Copyright 2022 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef CLASSBREAKSWITHALTERNATESYMBOLS_H
#define CLASSBREAKSWITHALTERNATESYMBOLS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class FeatureLayer;
class ServiceFeatureTable;
class Renderer;
class ClassBreaksRenderer;
class ArcGISMapImageSublayer;
class ClassBreak;
}
}

#include <QObject>

class ClassBreaksWithAlternateSymbols : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ClassBreaksWithAlternateSymbols(QObject* parent = nullptr);
  ~ClassBreaksWithAlternateSymbols() override;

  static void init();
  Q_INVOKABLE void applyRenderer();
  Q_INVOKABLE void resetRenderer();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::Renderer* m_originalRenderer = nullptr;
  Esri::ArcGISRuntime::ClassBreaksRenderer* m_classBreaksRenderer = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageSublayer* m_sublayer = nullptr;
  void createClassBreaksRenderer();
  Esri::ArcGISRuntime::ClassBreak* createClassBreak(const QColor& color, double min, double max);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  bool m_initialized = false;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
};

#endif // CLASSBREAKSWITHALTERNATESYMBOLS_H
