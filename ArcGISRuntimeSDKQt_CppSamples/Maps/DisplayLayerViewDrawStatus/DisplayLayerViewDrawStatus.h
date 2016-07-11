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

#ifndef DISPLAYLAYERVIEWDRAWSTATUS_H
#define DISPLAYLAYERVIEWDRAWSTATUS_H

namespace Esri
{
namespace ArcGISRuntime {
class Map;
class MapQuickView;
class FeatureLayer;
class ServiceFeatureTable;
class ArcGISTiledLayer;
class ArcGISMapImageLayer;
class LayerListModel;
}
}

#include <QMetaObject>
#include <QQuickItem>
#include <QStringList>

class DisplayLayerViewDrawStatus : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList layerNames READ layerNames NOTIFY namesChanged)
    Q_PROPERTY(QStringList layerViewStates READ layerViewStates NOTIFY statusChanged)

public:
    DisplayLayerViewDrawStatus(QQuickItem* parent = 0);
    ~DisplayLayerViewDrawStatus();

    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void statusChanged();
    void namesChanged();

private:
    QStringList layerViewStates() const;
    QStringList layerNames() const;
    void addLayers();
    void connectSignals();

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::ArcGISMapImageLayer* m_imageLayer;
    Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer;
    Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
    Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
    QStringList m_layerNames;
    QStringList m_layerViewStates;
};

#endif // DISPLAYLAYERVIEWDRAWSTATUS_H
