// [WriteFile Name=Unique_Value_Renderer, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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
    void createUniqueValue(QString stateName, Esri::ArcGISRuntime::SimpleFillSymbol* fillSymbol);

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
