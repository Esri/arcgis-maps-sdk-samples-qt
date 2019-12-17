// [WriteFile Name=SymbolizeShapefile, Category=DisplayInformation]
// [Legal]
// Copyright 2017 Esri.

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

#include "SymbolizeShapefile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "ShapefileFeatureTable.h"
#include "FeatureLayer.h"
#include "SimpleRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"

#include <QUrl>
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

SymbolizeShapefile::SymbolizeShapefile(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void SymbolizeShapefile::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SymbolizeShapefile>("Esri.Samples", 1, 0, "SymbolizeShapefileSample");
}

void SymbolizeShapefile::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the streets vector basemap
  m_map = new Map(Basemap::streetsVector(this), this);

  // Set an initial viewpoint
  const Point pt(-11662054, 4818336, SpatialReference(3857));
  const Viewpoint vp(pt, 200000);
  m_map->setInitialViewpoint(vp);

  // Create the ShapefileFeatureTable
  const QString dataPath = defaultDataPath() + "/ArcGIS/Runtime/Data/shp";
  ShapefileFeatureTable* shp = new ShapefileFeatureTable(dataPath + "/Subdivisions.shp", this);

  // Create the FeatureLayer
  m_featureLayer = new FeatureLayer(shp, this);
  m_map->operationalLayers()->append(m_featureLayer);

  // Set map to map view
  m_mapView->setMap(m_map);
}

void SymbolizeShapefile::updateRenderer()
{
  // Create the Symbol
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("yellow"), this);
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 1.0f, this);
  sfs->setOutline(sls);

  // Create the Renderer
  SimpleRenderer* simpleRenderer = new SimpleRenderer(sfs, this);

  // Set the Renderer on the Layer
  m_featureLayer->setRenderer(simpleRenderer);
}
