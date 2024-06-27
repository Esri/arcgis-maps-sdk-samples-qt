// [WriteFile Name=ShowGrid, Category=DisplayInformation]
// [Legal]
// Copyright 2024 Esri.

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

#include "ShowGrid.h"

#include "ArcGISTiledElevationSource.h"
#include "ElevationSourceListModel.h"
#include "Map.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "MapQuickView.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"
#include "LatitudeLongitudeGrid.h"
#include "MGRSGrid.h"
#include "Point.h"
#include "UTMGrid.h"
#include "USNGGrid.h"
#include "SimpleLineSymbol.h"
#include "TextSymbol.h"
#include "SpatialReference.h"
#include "Viewpoint.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

ShowGrid::ShowGrid(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this)),
  m_scene(new Scene(BasemapStyle::ArcGISImagery, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  double targetScale = 6450785;
  m_map->setInitialViewpoint(Viewpoint(Point(-10336141.70018318, 5418213.05332071, SpatialReference::webMercator()), targetScale));

  m_grid = new LatitudeLongitudeGrid(this);
}

ShowGrid::~ShowGrid() = default;

void ShowGrid::init()
{
  // Register classes for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ShowGrid>("Esri.Samples", 1, 0, "ShowGridSample");
}

void ShowGrid::setGridVisible(bool visible)
{
  m_grid->setVisible(visible);
}

void ShowGrid::setLabelsVisible(bool visible)
{
  m_grid->setLabelsVisible(visible);
}

void ShowGrid::setViewType(const QString& viewType)
{
  m_currentViewType = viewType;
  if (viewType == "MapView")
  {
    m_mapView->setViewpointAsync(m_sceneView->currentViewpoint(ViewpointType::BoundingGeometry), 0);

    m_mapView->setVisible(true);
    m_sceneView->setVisible(false);

    m_sceneView->setGrid(nullptr);
    m_mapView->setGrid(m_grid);
  }
  else if (viewType == "SceneView")
  {
    m_sceneView->setViewpointAsync(m_mapView->currentViewpoint(ViewpointType::BoundingGeometry), 0);

    m_mapView->setVisible(false);
    m_sceneView->setVisible(true);

    m_mapView->setGrid(nullptr);
    m_sceneView->setGrid(m_grid);
  }

  emit propertiesChanged();
}

void ShowGrid::setGridType(const QString& gridType)
{
  delete m_grid;
  m_grid = nullptr;

  m_currentGridType = gridType;

  if (gridType == "LatLong")
    m_grid = new LatitudeLongitudeGrid(this);
  else if (gridType == "MGRS")
    m_grid = new MGRSGrid(this);
  else if (gridType == "UTM")
    m_grid = new UTMGrid(this);
  else if (gridType == "USNG")
    m_grid = new USNGGrid(this);

  if (m_mapView->isVisible())
    m_mapView->setGrid(m_grid);
  else
    m_sceneView->setGrid(m_grid);

  setLineColor(m_currentLineColor);
  setLabelColor(m_currentLabelColor);
  setLabelPosition(m_currentLabelPosition);
  setLabelFormat(m_currentLabelFormat);

  emit propertiesChanged();
}

void ShowGrid::setLineColor(const QString& lineColor)
{
  m_currentLineColor = lineColor;
  SimpleLineSymbol* lineSymbol = static_cast<SimpleLineSymbol*>(m_grid->lineSymbol(0));
  lineSymbol->setColor(lineColor.toLower());

  for (int level = 0; level < m_grid->levelCount(); ++level)
    m_grid->setLineSymbol(level, lineSymbol);

  emit propertiesChanged();
}

void ShowGrid::setLabelColor(const QString& labelColor)
{
  m_currentLabelColor = labelColor;
  TextSymbol* labelSymbol = static_cast<TextSymbol*>(m_grid->textSymbol(0));
  labelSymbol->setColor(labelColor.toLower());

  for (int level = 0; level < m_grid->levelCount(); ++level)
    m_grid->setTextSymbol(level, labelSymbol);

  emit propertiesChanged();
}

void ShowGrid::setLabelPosition(const QString& labelPosition)
{
  m_currentLabelPosition = labelPosition;
  if (labelPosition == "Geographic")
    m_grid->setLabelPosition(GridLabelPosition::Geographic);
  else if (labelPosition == "Bottom Left")
    m_grid->setLabelPosition(GridLabelPosition::BottomLeft);
  else if (labelPosition == "Bottom Right")
    m_grid->setLabelPosition(GridLabelPosition::BottomRight);
  else if (labelPosition == "Top Left")
    m_grid->setLabelPosition(GridLabelPosition::TopLeft);
  else if (labelPosition == "Top Right")
    m_grid->setLabelPosition(GridLabelPosition::TopRight);
  else if (labelPosition == "Center")
    m_grid->setLabelPosition(GridLabelPosition::Center);
  else if (labelPosition == "All sides")
    m_grid->setLabelPosition(GridLabelPosition::AllSides);

  emit propertiesChanged();
}

void ShowGrid::setLabelFormat(const QString& labelFormat)
{
  if (m_grid->gridType() != GridType::LatitudeLongitudeGrid)
    return;

  m_currentLabelFormat = labelFormat;
  if (labelFormat == "Decimal Degrees")
    static_cast<LatitudeLongitudeGrid*>(m_grid)->setLabelFormat(LatitudeLongitudeGridLabelFormat::DecimalDegrees);
  else if (labelFormat == "Degrees Minutes Seconds")
    static_cast<LatitudeLongitudeGrid*>(m_grid)->setLabelFormat(LatitudeLongitudeGridLabelFormat::DegreesMinutesSeconds);

  emit propertiesChanged();
}

SceneQuickView* ShowGrid::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ShowGrid::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit viewChanged();
}

MapQuickView* ShowGrid::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowGrid::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setGrid(m_grid);

  emit viewChanged();
}

