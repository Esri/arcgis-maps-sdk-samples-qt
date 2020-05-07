// [WriteFile Name=FeatureLayerDictionaryRenderer, Category=Features]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "FeatureLayerDictionaryRenderer.h"

#include "DictionaryRenderer.h"
#include "FeatureLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEngine.h"
#include "Map.h"
#include "MapQuickView.h"
#include "DictionarySymbolStyle.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

FeatureLayerDictionaryRenderer::FeatureLayerDictionaryRenderer(QQuickItem* parent) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data")
{ 
}

FeatureLayerDictionaryRenderer::~FeatureLayerDictionaryRenderer() = default;

void FeatureLayerDictionaryRenderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<FeatureLayerDictionaryRenderer>("Esri.Samples", 1, 0, "FeatureLayerDictionaryRendererSample");
}

void FeatureLayerDictionaryRenderer::componentComplete()
{
  QQuickItem::componentComplete();  

  m_mapView = findChild<MapQuickView*>("mapView");
  // Create a map using the topo basemap
  Map* map = new Map(Basemap::topographic(this), this);
  // set map on the map view
  m_mapView->setMap(map);

  m_geodatabase = new Geodatabase(m_dataPath + "/geodatabase/militaryoverlay.geodatabase", this);

  connect(m_geodatabase, &Geodatabase::loadStatusChanged, this, [this](LoadStatus gdbLoadStatus)
  {
    if (gdbLoadStatus == LoadStatus::Loaded)
    {
      /**
             * If the field names in your data don't match the contents of DictionarySymbolStyle::symbologyFieldNames(),
             * you must add key-value pairs to a QMap<QString, QString> and include this QMap when calling the
             * DictionaryRenderer constructor. The keys and values are like this:
             *
             *   "dictionaryFieldName1": "myFieldName1"
             *   "dictionaryFieldName2": "myFieldName2"
             *
             * The same principle applies to DictionarySymbolStyle::textFieldNames() for text attributes that appear
             * as part of military symbols.
             *
             * The following commented-out code demonstrates one way to do it, in a scenario where the dictionary
             * expects the field name "identity" but the database table contains the field "affiliation" instead,
             * and where the dictionary expects the field name "uniquedesignation" but the database table contains
             * the field "uniqueId" instead.
             */
      //            QMap<QString, QString> symbologyFieldOverrides;
      //            QMap<QString, QString> textFieldOverrides;
      //            symbologyFieldOverrides["identity"] = "affiliation";
      //            textFieldOverrides["uniquedesignation"] = "uniqueId";

      //! [Create Dictionary Symbol Style Cpp]
      const QString specType = QStringLiteral("mil2525d");
      const QString stylePath = m_dataPath + "/styles/arcade_style/mil2525d.stylx";
      DictionarySymbolStyle* dictionarySymbolStyle = DictionarySymbolStyle::createFromFile(stylePath, this);
      //! [Create Dictionary Symbol Style Cpp]

      for (GeodatabaseFeatureTable* table : m_geodatabase->geodatabaseFeatureTables())
      {
        //! [Apply Dictionary Renderer Feature Layer Cpp]
        // Create a layer and set the feature table
        FeatureLayer* layer = new FeatureLayer(table, this);

        // Create a dictionary renderer and apply to the layer
        DictionaryRenderer* renderer = new DictionaryRenderer(dictionarySymbolStyle, this);
        layer->setRenderer(renderer);
        //! [Apply Dictionary Renderer Feature Layer Cpp]

        // Check to see if all layers have loaded
        connect(layer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus layerLoadStatus)
        {
          if (layerLoadStatus == LoadStatus::Loaded)
          {
            m_loadedLayerCount++;

            if (m_loadedLayerCount == m_geodatabase->geodatabaseFeatureTables().length())
            {
              /**
                             * If we get here, all the layers loaded. Union the extents and set
                             * the viewpoint.
                             */
              LayerListModel* layers = m_mapView->map()->operationalLayers();
              if (layers->size() > 0)
              {
                Envelope bbox = layers->at(0)->fullExtent();
                for (int j = 1; j < layers->size(); j++)
                {
                  bbox = GeometryEngine::unionOf(bbox, layers->at(j)->fullExtent());
                }
                m_mapView->setViewpointGeometry(bbox, 100);
              }

              emit allLayersLoaded();
            }
          }
        }, Qt::DirectConnection);

        /**
                 * Add the layer to the map. Inserting at index 0 puts the layer below previously added
                 * layers.
                 */
        m_mapView->map()->operationalLayers()->insert(0, layer);
      }
    }
  });
  m_geodatabase->load();
}
