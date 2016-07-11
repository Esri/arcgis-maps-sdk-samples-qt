// Copyright 2016 ESRI
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

#ifndef UNIQUE_VALUE_RENDERER_H
#define UNIQUE_VALUE_RENDERER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class FeatureLayer;
class ServiceFeatureTable;
class UniqueValueRenderer;
class SimpleFillSymbol;
}
}

#include <QQuickItem>

class Unique_Value_Renderer : public QQuickItem
{
    Q_OBJECT

public:
    Unique_Value_Renderer(QQuickItem* parent = 0);
    ~Unique_Value_Renderer();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
    Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
    Esri::ArcGISRuntime::UniqueValueRenderer* m_uniqueValueRenderer;
    Esri::ArcGISRuntime::SimpleFillSymbol* m_defaultSymbol;
    Esri::ArcGISRuntime::SimpleFillSymbol* m_californiaSymbol;
    Esri::ArcGISRuntime::SimpleFillSymbol* m_arizonaSymbol;
    Esri::ArcGISRuntime::SimpleFillSymbol* m_nevadaSymbol;
};

#endif // UNIQUE_VALUE_RENDERER_H
