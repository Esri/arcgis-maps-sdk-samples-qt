// [WriteFile Name=RasterFunctionFile, Category=Layers]
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

#include "RasterFunctionFile.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "RasterFunction.h"
#include "Envelope.h"
#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

RasterFunctionFile::RasterFunctionFile(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void RasterFunctionFile::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<RasterFunctionFile>("Esri.Samples", 1, 0, "RasterFunctionFileSample");
}

void RasterFunctionFile::componentComplete()
{
  QQuickItem::componentComplete();

  // get data path
  m_dataPath = QUrl(QQmlProperty::read(this, "dataPath").toString()).toLocalFile();
  m_rasterPath = m_dataPath + "Shasta_Elevation.tif";

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the imagery basemap
  m_map = new Map(Basemap::imagery(this), this);

  // Add the Raster Layer
  Raster* raster = new Raster(m_rasterPath, this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  rasterLayer->setOpacity(0.5);
  m_map->operationalLayers()->append(rasterLayer);

  // Set initial Extent
  Envelope env = Envelope::fromJson("{\"spatialReference\": {\"latestWkid\": 3857, \"wkid\": 102100 },\"xmax\": -13591503.517810356,\"xmin\": -13606233.44023646,\"ymax\": 4982810.138527592,\"ymin\": 4971762.696708013}");
  m_map->setInitialViewpoint(Viewpoint(env));

  // Set map to map view
  m_mapView->setMap(m_map);
}

void RasterFunctionFile::applyRasterFunction()
{
  // create raster function
  RasterFunction* rasterFunction = createRasterFunction();

  // create the raster from the raster function
  Raster* raster = new Raster(rasterFunction, this);

  // create raster layer from raster
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  rasterLayer->setOpacity(0.5);

  // add raster to map
  m_map->operationalLayers()->clear();
  m_map->operationalLayers()->append(rasterLayer);
}

RasterFunction* RasterFunctionFile::createRasterFunction()
{
  // create a RasterFunction
  RasterFunction* rasterFunction = new RasterFunction(m_dataPath + "/color.json");

  // set the number of rasters required - 2 in this case
  Raster* rasterArg1 = new Raster(m_rasterPath);
  Raster* rasterArg2 = new Raster(m_rasterPath);
  if (rasterFunction)
  {
    rasterFunction->arguments()->setRaster("raster", rasterArg1);
    rasterFunction->arguments()->setRaster("raster", rasterArg2);
  }

  return rasterFunction;
}
