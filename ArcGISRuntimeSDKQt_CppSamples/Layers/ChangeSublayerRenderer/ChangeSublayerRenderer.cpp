// [WriteFile Name=ChangeSublayerRenderer, Category=Layers]
// [Legal]
// Copyright 2018 Esri.

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

#include "ChangeSublayerRenderer.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ArcGISMapImageLayer.h"
#include "ArcGISMapImageSublayer.h"
#include "Viewpoint.h"
#include "ClassBreaksRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"

using namespace Esri::ArcGISRuntime;

ChangeSublayerRenderer::ChangeSublayerRenderer(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ChangeSublayerRenderer::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ChangeSublayerRenderer>("Esri.Samples", 1, 0, "ChangeSublayerRendererSample");
}

void ChangeSublayerRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the streets basemap
  m_map = new Map(Basemap::streets(this), this);

  // Add the map image layer
  ArcGISMapImageLayer* mapImageLayer = new ArcGISMapImageLayer(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"), this);

  // Get the sublayer
  connect(mapImageLayer, &ArcGISMapImageLayer::doneLoading, this, [this, mapImageLayer](Error e)
  {
    if (!e.isEmpty())
      return;

    if (mapImageLayer->mapImageSublayers()->size() < 3)
      return;

    m_sublayer = dynamic_cast<ArcGISMapImageSublayer*>(mapImageLayer->mapImageSublayers()->at(2));

    // get the sublayer's original renderer
    connect(m_sublayer, &ArcGISMapImageSublayer::doneLoading, this, [this](Error e)
    {
      if (!e.isEmpty())
        return;

      m_originalRenderer = m_sublayer->renderer()->clone(this);

      emit sublayerLoaded();
    });

    // load the sublayer
    m_sublayer->load();
  });

  // add the layer to the map
  m_map->operationalLayers()->append(mapImageLayer);

  // set initial viewpoint
  Envelope env(-13834661.666904, 331181.323482, -8255704.998713, 9118038.075882, SpatialReference(3857));
  Viewpoint initialViewpoint(env);
  m_map->setInitialViewpoint(initialViewpoint);

  // create the class breaks renderer
  createClassBreaksRenderer();

  // Set map to map view
  m_mapView->setMap(m_map);
}

void ChangeSublayerRenderer::createClassBreaksRenderer()
{
  // create class breaks renderer
  m_classBreaksRenderer = new ClassBreaksRenderer(this);
  m_classBreaksRenderer->setFieldName(QStringLiteral("POP2007"));

  // create and append class breaks
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(227, 235, 207), -99, 8560));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(150, 194, 191), 8560, 18109));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(97, 166, 181), 18109, 35501));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(69, 125, 150), 35501, 86100));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(41, 84, 120), 86100, 10110975));
}

// helper function to create class breaks for the renderer
ClassBreak* ChangeSublayerRenderer::createClassBreak(const QColor &color, double min, double max)
{
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(153, 153, 153), 1.0f /*width*/, this);
  SimpleFillSymbol* sfs1 = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, color, outline, this);
  const QString description = QString("> %1 to %2").arg(QString::number(min), QString::number(max));
  const QString label = description;
  return new ClassBreak(description, label, min, max, sfs1);
}

// apply the class breaks renderer
void ChangeSublayerRenderer::applyRenderer()
{
  if (!m_sublayer || !m_classBreaksRenderer)
    return;

  m_sublayer->setRenderer(m_classBreaksRenderer);
}

// reset to the original renderer
void ChangeSublayerRenderer::resetRenderer()
{
  if (!m_sublayer || !m_originalRenderer)
    return;

  m_sublayer->setRenderer(m_originalRenderer);
}
