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
class MultilayerPolygonSymbol;
class Symbol;
class SimpleFillSymbol;
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
  Q_INVOKABLE void setScale(int16_t scale);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::Renderer* m_renderer = nullptr;
  Esri::ArcGISRuntime::ClassBreaksRenderer* m_classBreaksRenderer = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageSublayer* m_sublayer = nullptr;


  void createClassBreaksRenderer();
  QList<Esri::ArcGISRuntime::Symbol*> createClassBreak(Esri::ArcGISRuntime::MultilayerPolygonSymbol* ml_symbol, const QList<Esri::ArcGISRuntime::Symbol*>& alternateSymbols);
  QList<Esri::ArcGISRuntime::Symbol*> createAlternateSymbols();


  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  bool m_initialized = false;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
};

#endif // CLASSBREAKSWITHALTERNATESYMBOLS_H
