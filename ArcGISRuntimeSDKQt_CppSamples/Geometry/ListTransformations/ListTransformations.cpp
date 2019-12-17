// [WriteFile Name=ListTransformations, Category=Geometry]
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

#include "ListTransformations.h"

#include "Map.h"
#include "MapQuickView.h"
#include "TransformationCatalog.h"
#include "GeometryEngine.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"
#include "DatumTransformation.h"
#include "GeographicTransformationStep.h"
#include "GeographicTransformation.h"

#include <QDir>
#include <QtCore/qglobal.h>
#include <QUrl>
#include <QVariantMap>

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

ListTransformations::ListTransformations(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void ListTransformations::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ListTransformations>("Esri.Samples", 1, 0, "ListTransformationsSample");
}

void ListTransformations::componentComplete()
{
  QQuickItem::componentComplete();   

  // get data path
  const QUrl dataPath = QUrl(defaultDataPath() + "/ArcGIS/Runtime/Data/PEDataRuntime");

  // connect to TransformationCatalog error signal
  connect(TransformationCatalog::instance(), &TransformationCatalog::errorOccurred, this, [this](Error e)
  {
    if (e.isEmpty())
      return;

    emit showStatusBar(QString("Error setting projection engine directory: %1. %2").arg(e.message(), e.additionalMessage()));
  });

  // Create a geometry located in the Greenwich observatory courtyard in London, UK, the location of the
  // Greenwich prime meridian. This will be projected using the selected transformation.
  const double x = 538985.355;
  const double y = 177329.516;
  m_originalPoint = Point(x, y, SpatialReference(27700));

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the light gray canvas basemap
  m_map = new Map(Basemap::lightGrayCanvas(this), this);

  // Set initial viewpoint of the map
  m_map->setInitialViewpoint(Viewpoint(m_originalPoint, 5000));

  // connect to map loaded signal
  connect(m_map, &Map::doneLoading, this, [this, dataPath]
  {
    // Set the TransformationCatalog path
    TransformationCatalog::setProjectionEngineDirectory(dataPath.toString());
    if (TransformationCatalog::projectionEngineDirectory().length() > 0)
      emit showStatusBar(QString("Projection engine directory set: %1").arg(TransformationCatalog::instance()->projectionEngineDirectory()));

    // get the initial list of transformations
    refreshTransformationList(true);
  });

  // add graphics to the map view
  addGraphics();

  // set map to map view
  m_mapView->setMap(m_map);
}

// Create a GraphicsOverlay with two Graphics - one to hold the default
// transformed geometry, and the other to use the selected transformation
void ListTransformations::addGraphics()
{
  // Create the original graphic
  SimpleMarkerSymbol* blueSquare = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor("blue"), 20.f, this);
  m_originalGraphic = new Graphic(m_originalPoint, blueSquare, this);

  // Create the projected graphic
  SimpleMarkerSymbol* redCross = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, QColor("red"), 20.f, this);
  m_projectedGraphic = new Graphic(m_originalPoint, redCross, this);

  // Create the Overlay
  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);
  overlay->graphics()->append(m_originalGraphic);
  overlay->graphics()->append(m_projectedGraphic);
}


void ListTransformations::refreshTransformationList(bool orderBySuitability)
{
  // get the input/output spatial reference
  SpatialReference inSpatialReference = m_originalGraphic->geometry().spatialReference();
  SpatialReference outSpatialReference = m_map->spatialReference();

  // request the transformations
  if (orderBySuitability)
    m_transformations = TransformationCatalog::transformationsBySuitability(inSpatialReference, outSpatialReference, m_mapView->visibleArea().extent());
  else
    m_transformations = TransformationCatalog::transformationsBySuitability(inSpatialReference, outSpatialReference);

  // update the QML property list
  m_transformationList.clear();
  for (DatumTransformation* transformation : m_transformations)
  {
    QVariantMap transformationMap;
    transformationMap["name"] = transformation->name();
    transformationMap["isMissingProjectionEngineFiles"] = transformation->isMissingProjectionEngineFiles();
    m_transformationList.append(transformationMap);
  }

  // update the property
  emit transformationListChanged();
}

void ListTransformations::updateGraphicTransformation(int index)
{
  GeographicTransformation* transform = static_cast<GeographicTransformation*>(m_transformations.at(index));
  if (transform->isMissingProjectionEngineFiles())
  {
    QString missingFiles = "Missing grid files: ";
    const QList<GeographicTransformationStep> steps = transform->steps();
    for (GeographicTransformationStep step : steps)
    {
      for (const QString& filename : step.projectionEngineFilenames())
      {
        missingFiles += filename;
      }
    }
    emit showStatusBar(missingFiles);
  }
  else
  {
    Geometry projectedPoint = GeometryEngine::project(m_originalPoint, m_map->spatialReference(), transform);
    m_projectedGraphic->setGeometry(projectedPoint);
  }
}
