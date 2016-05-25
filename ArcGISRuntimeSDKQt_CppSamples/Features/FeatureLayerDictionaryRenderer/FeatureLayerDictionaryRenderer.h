// [WriteFile Name=FeatureLayerDictionaryRenderer, Category=Features]
// [Legal]
// Copyright 2015-2016 Esri.

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

#ifndef FEATURELAYERDICTIONARYRENDERER_H
#define FEATURELAYERDICTIONARYRENDERER_H

#include <QQuickItem>
#include <QMutex>

#include "FeatureLayer.h"
#include "Geodatabase.h"
#include "MapQuickView.h"

using namespace Esri::ArcGISRuntime;

class FeatureLayerDictionaryRenderer : public QQuickItem
{
    Q_OBJECT

public:
    explicit FeatureLayerDictionaryRenderer(QQuickItem* parent = 0);
    ~FeatureLayerDictionaryRenderer();

    void componentComplete() Q_DECL_OVERRIDE;

private:
    QString m_dataPath;
    double m_scaleFactor;
    MapQuickView* m_pMapView;
    Geodatabase* m_pGeodatabase;
    QList<FeatureLayer*> m_layers;
    QMutex m_mutexLayers;
};

#endif // FEATURELAYERDICTIONARYRENDERER_H
