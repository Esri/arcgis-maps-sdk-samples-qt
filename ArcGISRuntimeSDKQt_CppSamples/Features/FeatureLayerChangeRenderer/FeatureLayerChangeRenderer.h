// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FEATURE_LAYER_CHANGE_RENDERER_H
#define FEATURE_LAYER_CHANGE_RENDERER_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class Map;
        class MapQuickView;
        class FeatureLayer;
    }
}

#include <QQuickItem>

class FeatureLayerChangeRenderer : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool layerInitialized READ layerInitialized NOTIFY layerInitializedChanged)

public:
    FeatureLayerChangeRenderer(QQuickItem* parent = 0);
    ~FeatureLayerChangeRenderer();

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void changeRenderer();

signals:
    void layerInitializedChanged();

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
    bool m_initialized;

private:
    bool layerInitialized() const;
};

#endif // FEATURE_LAYER_CHANGE_RENDERER_H

