// [WriteFile Name=ReadSymbolsFromMobileStyle, Category=DisplayInformation]
// [Legal]
// Copyright 2019 Esri.

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

#include "ReadSymbolsFromMobileStyle.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SymbolStyle.h"
#include "MultilayerSymbol.h"
#include "Point.h"
#include "MultilayerPointSymbol.h"

#include "SymbolImageProvider.h"

#include <QDir>
#include <QObject>
#include <QQmlContext>
#include <QTemporaryDir>
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

ReadSymbolsFromMobileStyle::ReadSymbolsFromMobileStyle(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(Basemap::topographic(this), this)),
  m_graphicParent(new QObject())
{
  m_symbolStyle = new SymbolStyle(defaultDataPath() + "/ArcGIS/Runtime/Data/styles/emoji-mobile.stylx", this);

  // Connect to the search completed signal of the style
  connect(m_symbolStyle, &SymbolStyle::searchSymbolsCompleted, this, [this](QUuid id, SymbolStyleSearchResultListModel* results)
  {
    const int index = m_taskIds.indexOf(id);
    m_models[index] = results;

    emit symbolResultsChanged();
    updateSymbol(0, 0, 0, QColor(Qt::yellow), 40);
  });

  // Load the style
  connect(m_symbolStyle, &SymbolStyle::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
      return;

    // search for hat symbol layers
    SymbolStyleSearchParameters hatParams;
    hatParams.setCategories({"Hat"});
    TaskWatcher hatWatcher = m_symbolStyle->searchSymbols(hatParams);
    m_taskIds.append(hatWatcher.taskId());

    // search for mouth symbol layers
    SymbolStyleSearchParameters mouthParams;
    mouthParams.setCategories({"Mouth"});
    TaskWatcher mouthWatcher = m_symbolStyle->searchSymbols(mouthParams);
    m_taskIds.append(mouthWatcher.taskId());

    // search for eyes symbol layers
    SymbolStyleSearchParameters eyeParams;
    eyeParams.setCategories({"Eyes"});
    TaskWatcher eyeWatcher = m_symbolStyle->searchSymbols(eyeParams);
    m_taskIds.append(eyeWatcher.taskId());

    // search for face symbol layers
    SymbolStyleSearchParameters faceParams;
    faceParams.setCategories({"Face"});
    TaskWatcher faceWatcher = m_symbolStyle->searchSymbols(faceParams);
    m_taskIds.append(faceWatcher.taskId());
  });

  m_symbolStyle->load();

  // Connect to fetchSymbol completed signal
  connect(m_symbolStyle, &SymbolStyle::fetchSymbolCompleted, this, [this](QUuid, Symbol* symbol)
  {
    if (m_currentSymbol)
      delete m_currentSymbol;

    // store the resulting symbol
    m_currentSymbol = static_cast<MultilayerPointSymbol*>(symbol);

    // ensure cast was successful
    if (!m_currentSymbol)
      return;

    // set the size
    m_currentSymbol->setSize(m_symbolSize);

    // set the color preferences per layer
    for (SymbolLayer* lyr : *(m_currentSymbol->symbolLayers()))
    {
      lyr->setColorLocked(true);
    }

    m_currentSymbol->symbolLayers()->at(0)->setColorLocked(false);

    // set the color
    m_currentSymbol->setColor(m_currentColor);

    // request symbol swatch
    connect(m_currentSymbol, &MultilayerPointSymbol::createSwatchCompleted, m_currentSymbol, [this](QUuid id, QImage img)
    {
      if (!m_symbolImageProvider)
        return;

      // convert the QUuid into a QString
      const QString imageId = id.toString().remove("{").remove("}");

      // add the image to the provider
      m_symbolImageProvider->addImage(imageId, img);

      // update the URL with the unique id
      m_symbolImageUrl = QString("image://%1/%2").arg(SymbolImageProvider::imageProviderId(), imageId);

      // emit the signal to trigger the QML Image to update
      emit symbolImageUrlChanged();
    });

    m_currentSymbol->createSwatch();
  });
}

ReadSymbolsFromMobileStyle::~ReadSymbolsFromMobileStyle() = default;

void ReadSymbolsFromMobileStyle::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ReadSymbolsFromMobileStyle>("Esri.Samples", 1, 0, "ReadSymbolsFromMobileStyleSample");
  qmlRegisterUncreatableType<SymbolStyleSearchResultListModel>("Esri.Samples", 1, 0, "SymbolStyleSearchResultListModel", "SymbolStyleSearchResultListModel is uncreateable");
}

MapQuickView* ReadSymbolsFromMobileStyle::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ReadSymbolsFromMobileStyle::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // Get the image provider from the QML Engine
  QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
  engine->addImageProvider(SymbolImageProvider::imageProviderId(), new SymbolImageProvider);
  m_symbolImageProvider = static_cast<SymbolImageProvider*>(engine->imageProvider(SymbolImageProvider::imageProviderId()));

  // add a graphics overlay
  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);

  // connect to mouse clicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this, overlay](QMouseEvent mouseEvent)
  {
    if (!m_currentSymbol)
      return;

    const Point clickedPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    Graphic* graphic = new Graphic(clickedPoint, m_currentSymbol, m_graphicParent.get());
    overlay->graphics()->append(graphic);
  });

  emit mapViewChanged();
}

// Clear the graphics from the overlay and delete each object
void ReadSymbolsFromMobileStyle::clearGraphics()
{
  if (!m_mapView)
    return;

  GraphicsOverlay* overlay = m_mapView->graphicsOverlays()->first();
  if (!overlay)
    return;

  // clear the list model
  overlay->graphics()->clear();

  // reset m_graphicsParent to delete all children
  m_graphicParent.reset(new QObject());
}

void ReadSymbolsFromMobileStyle::updateSymbol(int hatIndex, int mouthIndex, int eyeIndex, QColor color, int size)
{
  if (!m_symbolStyle || !hatResults() || !mouthResults() || !eyeResults() || !faceResults())
    return;

  // set the color and size members
  m_currentColor = color;
  m_symbolSize = size;

  // fetch the new symbol based on keys
  QStringList keys;
  keys.append(faceResults()->searchResults().at(0).key());
  keys.append(eyeResults()->searchResults().at(eyeIndex).key());
  keys.append(mouthResults()->searchResults().at(mouthIndex).key());
  keys.append(hatResults()->searchResults().at(hatIndex).key());
  m_symbolStyle->fetchSymbol(keys);
}

SymbolStyleSearchResultListModel* ReadSymbolsFromMobileStyle::hatResults() const
{
  return m_models[0];
}

SymbolStyleSearchResultListModel* ReadSymbolsFromMobileStyle::mouthResults() const
{
  return m_models[1];
}

SymbolStyleSearchResultListModel* ReadSymbolsFromMobileStyle::eyeResults() const
{
  return m_models[2];
}

SymbolStyleSearchResultListModel* ReadSymbolsFromMobileStyle::faceResults() const
{
  return m_models[3];
}
