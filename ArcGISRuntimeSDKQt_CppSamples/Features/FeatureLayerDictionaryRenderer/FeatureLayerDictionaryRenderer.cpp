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

#include "FeatureLayerDictionaryRenderer.h"

#include <QQmlProperty>

#include "DictionaryRenderer.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "SymbolDictionary.h"

FeatureLayerDictionaryRenderer::FeatureLayerDictionaryRenderer(QQuickItem* parent) :
    QQuickItem(parent),
    m_pMapView(nullptr),
    m_pGeodatabase(nullptr)
{

}

FeatureLayerDictionaryRenderer::~FeatureLayerDictionaryRenderer()
{
    delete m_pGeodatabase;
}

void FeatureLayerDictionaryRenderer::componentComplete()
{
    QQuickItem::componentComplete();

    m_dataPath = QQmlProperty::read(this, "dataPath").toString();
    m_scaleFactor = QQmlProperty::read(this, "scaleFactor").toDouble();

    m_pMapView = findChild<MapQuickView*>("mapView");
    // Create a map using the topo basemap
    Map* pMap = new Map(Basemap::topographic(this), this);
    // set map on the map view
    m_pMapView->setMap(pMap);

    m_pGeodatabase = new Geodatabase(m_dataPath + "/geodatabase/militaryoverlay.geodatabase");

    connect(m_pGeodatabase, &Geodatabase::loadStatusChanged, [this](LoadStatus gdbLoadStatus)
    {
        if (gdbLoadStatus == LoadStatus::Loaded)
        {
            QMap<QString, QString> symbologyFieldOverrides;
            QMap<QString, QString> textFieldOverrides;
            /**
             * If the field names in your data don't match the contents of SymbolDictionary::symbologyFieldNames(),
             * you must add key-value pairs to a QMap<QString, QString> and include this QMap when calling the
             * DictionaryRenderer constructor. The keys and values are like this:
             *
             *   "dictionaryFieldName1": "myFieldName1"
             *   "dictionaryFieldName2": "myFieldName2"
             *
             * The same principle applies to SymbolDictionary::textFieldNames() for text attributes that appear
             * as part of military symbols.
             *
             * The following commented-out code demonstrates one way to do it, in a scenario where the dictionary
             * expects the field name "identity" but the database table contains the field "affiliation" instead,
             * and where the dictionary expects the field name "uniquedesignation" but the database table contains
             * the field "uniqueId" instead.
             */
//            symbologyFieldOverrides["identity"] = "affiliation";
//            textFieldOverrides["uniquedesignation"] = "uniqueId";

            SymbolDictionary symbolDictionary("mil2525d", m_dataPath + "/styles/mil2525d.stylx", this);

            // Create a layer for each table
            QList<GeodatabaseFeatureTable*> tables = m_pGeodatabase->geodatabaseFeatureTables();
            for (int i = tables.length() - 1; i >= 0 ; i--)
            {
                GeodatabaseFeatureTable* pTable = tables[i];
                FeatureLayer* pLayer = new FeatureLayer(pTable);
                m_layers.append(pLayer);
                // Each layer needs its own renderer, though all renderers can share the SymbolDictionary.
                DictionaryRenderer renderer(&symbolDictionary, symbologyFieldOverrides, textFieldOverrides);
                pLayer->setRenderer(&renderer);
            }

            for (FeatureLayer* pLayer : m_layers)
            {
                // Check to see if all layers have loaded
                connect(pLayer, &FeatureLayer::loadStatusChanged, [this](LoadStatus layerLoadStatus)
                {
                    if (layerLoadStatus == LoadStatus::Loaded)
                    {
                        /*
                         * Lock this block so that only one thread at a time checks to to see if all
                         * layers loaded.
                         */
                        m_mutexLayers.lock();
                        {
                            if (m_layers.length() == m_pGeodatabase->geodatabaseFeatureTables().length())
                            {
                                // See if all the layers have loaded
                                bool foundNonLoadedLayer = false;
                                for (FeatureLayer* pLayer : m_layers)
                                {
                                    if (pLayer->loadStatus() != LoadStatus::Loaded)
                                    {
                                        foundNonLoadedLayer = true;
                                        break;
                                    }
                                }

                                if (!foundNonLoadedLayer)
                                {
                                    /**
                                     * If we get here, all the layers loaded. Union the extents and set
                                     * the viewpoint.
                                     */
                                    Envelope bbox = m_layers[0]->fullExtent();
                                    for (FeatureLayer* pLayer : m_layers)
                                    {
                                        bbox = GeometryEngine::unionOf(bbox, pLayer->fullExtent());
                                    }

                                    m_pMapView->setViewpointGeometry(bbox, 300 * m_scaleFactor);
                                    findChild<QQuickItem*>("progressBar_loading")->setVisible(false);
                                }
                            }
                        }
                        m_mutexLayers.unlock();
                    }
                });

                // Add the layer to the map
                LayerListModel* pOperationalLayers = m_pMapView->map()->operationalLayers();
                pOperationalLayers->append(pLayer);
            }
        }
    });
    m_pGeodatabase->load();
}
