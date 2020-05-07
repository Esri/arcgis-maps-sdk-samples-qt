// [WriteFile Name=Picture_Marker_Symbol, Category=DisplayInformation]
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

#include "Picture_Marker_Symbol.h"

#include "Map.h"
#include "Point.h"
#include "Graphic.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "SpatialReference.h"
#include "PictureMarkerSymbol.h"

#include <QUrl>
#include <QString>
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

Picture_Marker_Symbol::Picture_Marker_Symbol(QQuickItem* parent) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data")
{
}

Picture_Marker_Symbol::~Picture_Marker_Symbol() = default;

void Picture_Marker_Symbol::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<Picture_Marker_Symbol>("Esri.Samples", 1, 0, "Picture_Marker_SymbolSample");
}

void Picture_Marker_Symbol::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // set initial viewpoint
  m_map->setInitialViewpoint(Viewpoint(Envelope(-229100, 6550700, -223300, 6552100, SpatialReference::webMercator())));

  // create graphics overlay
  m_graphicsOverlay = new GraphicsOverlay(this);

  // create a campsite symbol from a URL
  PictureMarkerSymbol* campSymbol = new PictureMarkerSymbol(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0/images/e82f744ebb069bb35b234b3fea46deae"), this);
  setWidthAndHeight(campSymbol, 38.0f);
  Point campPoint(-228835, 6550763, SpatialReference::webMercator());
  addGraphic(campPoint, campSymbol);

  // create a blue symbol from a local resource
  PictureMarkerSymbol* blueSymbol = new PictureMarkerSymbol(QUrl("qrc:/Samples/DisplayInformation/Picture_Marker_Symbol/blue_symbol.png"), this);
  setWidthAndHeight(blueSymbol, 80.0f);
  Point blueSymbolPoint(-223560, 6552021, SpatialReference::webMercator());
  addGraphic(blueSymbolPoint, blueSymbol);

  // create an orange symbol from a file path
  PictureMarkerSymbol* orangeSymbol = new PictureMarkerSymbol(QUrl(m_dataPath + "/symbol/orange_symbol.png"), this);
  setWidthAndHeight(orangeSymbol, 64.0f);
  Point orangeSymbolPoint(-226773, 6550477, SpatialReference::webMercator());
  addGraphic(orangeSymbolPoint, orangeSymbol);

  // add GraphicsOverlay to MapView
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
}

void Picture_Marker_Symbol::addGraphic(Point &point, PictureMarkerSymbol* symbol)
{
  // create graphic
  Graphic* graphic = new Graphic(point, symbol, this);
  // append to graphicsoverlay
  m_graphicsOverlay->graphics()->append(graphic);
}

void Picture_Marker_Symbol::setWidthAndHeight(PictureMarkerSymbol* symbol, float size)
{
  symbol->setWidth(size);
  symbol->setHeight(size);
}
