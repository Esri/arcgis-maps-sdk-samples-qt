// [WriteFile Name=GODictionaryRenderer, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "GODictionaryRenderer.h"

// ArcGIS Maps SDK headers
#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "Envelope.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "PointCollection.h"
#include "SpatialReference.h"
#include "AttributeListModel.h"

// Qt headers
#include <QFuture>
#include <QStandardPaths>
#include <QTimer>

// STL headers
#include <random>

namespace
{
  std::mt19937 m_generator(std::random_device{}());
  const std::vector<std::string> identifications{"0", "1", "2", "3", "4", "5"};
  std::uniform_int_distribution<std::size_t> identification_distribution{0, identifications.size() - 1};
  const std::vector<std::string> mainIcons{"000000", "110000", "110100", "110101", "110102", "110103",
                                           "110104", "110105", "110107", "110116", "110200"};
  std::uniform_int_distribution<std::size_t> mainIcon_distribution{0, mainIcons.size() - 1};
  std::uniform_int_distribution<std::size_t> azimuth_distribution{0, 360};
  const std::vector<std::string> modifiers{"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10"};
  std::uniform_int_distribution<std::size_t> modifiers_distribution{0, modifiers.size() - 1};
} // namespace

using namespace Esri::ArcGISRuntime;

GODictionaryRenderer::GODictionaryRenderer(QQuickItem* parent) :
  QQuickItem(parent)
{
}

GODictionaryRenderer::~GODictionaryRenderer() = default;

void GODictionaryRenderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GODictionaryRenderer>("Esri.Samples", 1, 0, "GODictionaryRendererSample");
}

bool GODictionaryRenderer::graphicsLoaded() const
{
  return m_graphicsLoaded;
}

void GODictionaryRenderer::componentComplete()
{
  QQuickItem::componentComplete();

  // Create graphics overlay
  m_graphicsOverlay = new GraphicsOverlay(this);

  // App6E dictionary
  DictionarySymbolStyle* dictionarySymbolStyle = DictionarySymbolStyle::createDictionarySymbolStyleFromUrl(
    QUrl{"https://www.arcgis.com/home/item.html?id=ab9e2684f07446199f81883577540e4d"}, this);

  DictionaryRenderer* renderer = new DictionaryRenderer(dictionarySymbolStyle, this);
  m_graphicsOverlay->setRenderer(renderer);
  // Create a map and give it to the MapView
  m_mapView = findChild<MapQuickView*>("mapView");
  m_map = new Map(BasemapStyle::ArcGISTopographic, this);

  // add 200 graphics at random positions
  const auto sr = SpatialReference::wgs84();
  for (int i = 0; i < 200; i++)
  {
    // positions don't really matter - it will all be randomized once the loop begins
    m_graphicsOverlay->graphics()->append(new Graphic{Point{100000.0 + static_cast<double>(i), 100000.0 + static_cast<double>(i), sr}, this});
  }

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  connect(m_mapView, &MapQuickView::spatialReferenceChanged, this, [this]()
  {
    // zoomToGraphics();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]
    {
      for (auto* graphic : (*m_graphicsOverlay->graphics()))
      {
        QVariantMap attributes;
        const auto& identification = identifications[identification_distribution(m_generator)];
        const auto& mainIcon = mainIcons[mainIcon_distribution(m_generator)];
        const auto& modifier1 = modifiers[modifiers_distribution(m_generator)];
        const auto& modifier2 = modifiers[modifiers_distribution(m_generator)];
        auto sidc = "140" + identification + "010000" + mainIcon + "00" + modifier1 + modifier2;
        attributes["sidc"] = QString::fromStdString(sidc);
        auto randomX = 8.5417 + (9.2417 - 8.5417) * static_cast<double>(rand()) / RAND_MAX;
        auto randomY = 47.3769 + (46.3769 - 47.3769) * static_cast<double>(rand()) / RAND_MAX;
        // attributes["direction"] = QString::number(static_cast<int>(azimuth_distribution(m_generator)) - 180);
        //  qDebug() << attributes["direction"];
        attributes["direction"] = QString::number(180.0 + (-180.0 - 180.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["speed"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["staffcomment"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["uniquedesignation"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["additionalinformation"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["x"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["y"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["z"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        attributes["z2"] = QString::number(100000.0 + (0.0 - 100000.0) * static_cast<double>(rand()) / RAND_MAX);
        auto point = Point(randomX, randomY, m_mapView->spatialReference());
        graphic->attributes()->setAttributesMap(attributes);
        graphic->setGeometry(point);
      }
      static const bool zoomedOnce = [this]()
      {
        m_mapView->setViewpointGeometryAsync(m_graphicsOverlay->extent(), 20);
        return true;
      }();
      Q_UNUSED(zoomedOnce)
    });
    timer->start(100);
  });

  m_mapView->setMap(m_map);
}
