// [WriteFile Name=CreateAndEditGeometries, Category=Geometry]
// [Legal]
// Copyright 2023 Esri.

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

#include "CreateAndEditGeometries.h"

#include "FreehandTool.h"
#include "GeometryEditor.h"
#include "GeometryEditorElement.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Multipoint.h"
#include "MultipointBuilder.h"
#include "Point.h"
#include "PointCollection.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TaskWatcher.h"
#include "VertexTool.h"

#include <QUuid>

using namespace Esri::ArcGISRuntime;

CreateAndEditGeometries::CreateAndEditGeometries(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this))
{
  m_geometryEditor = new GeometryEditor(this);
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_tempGraphicsParent = new QObject(this);
}

CreateAndEditGeometries::~CreateAndEditGeometries() = default;

void CreateAndEditGeometries::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateAndEditGeometries>("Esri.Samples", 1, 0, "CreateAndEditGeometriesSample");
}

// Set the view (created in QML)
void CreateAndEditGeometries::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointCenter(Point(-9.5920, 53.08230, SpatialReference::wgs84()), 5000);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);

  emit mapViewChanged();

  createInitialSymbols();
  createInitialGraphics();
  createConnections();
}

// Button methods

void CreateAndEditGeometries::startGeometryEditorWithGeometryType(GeometryEditorMode geometryEditorMode)
{
  switch (geometryEditorMode)
  {
  case GeometryEditorMode::PointMode:
    m_geometryEditor->start(GeometryType::Point);
    break;
  case GeometryEditorMode::MultipointMode:
    m_geometryEditor->start(GeometryType::Multipoint);
    break;
  case GeometryEditorMode::PolylineMode:
    m_geometryEditor->start(GeometryType::Polyline);
    break;
  case GeometryEditorMode::PolygonMode:
    m_geometryEditor->start(GeometryType::Polygon);
    break;
  default:
    break;
  }

  emit geometryEditorStartedChanged();
}

void CreateAndEditGeometries::stopEditing(bool saveGeometry)
{
  // Calling stop on the geometry editor returns the currently drawn geometry
  const Geometry geometry = m_geometryEditor->stop();
  emit geometryEditorStartedChanged();

  // If the user wants to discard edits
  if (!saveGeometry)
  {
    // If the user was editing an existing geometry, show the original graphic and clear the selection
    if (m_editingGraphic)
    {
      m_editingGraphic->setVisible(true);
      m_editingGraphic = nullptr;
    }
    return;
  }

  // If the user wants to stop editing and save the geometry
  // If the user was editing an existing graphic, update its geometry
  if (m_editingGraphic)
  {
    m_editingGraphic->setGeometry(geometry);

    // Show the graphic and clear the selection
    m_editingGraphic->setVisible(true);
    m_editingGraphic = nullptr;

    return;
  }

  // If the user was not editing an existing graphic, create a new graphic with the geometry and a symbol
  // Determine what symbology to use for the graphic
  Symbol* geometrySymbol = nullptr;
  switch (geometry.geometryType())
  {
  case GeometryType::Point:
    geometrySymbol = m_pointSymbol;
    break;

  case GeometryType::Multipoint:
    geometrySymbol = m_multiPointSymbol;
    break;

  case GeometryType::Polyline:
    geometrySymbol = m_lineSymbol;
    break;

  case GeometryType::Polygon:
    geometrySymbol = m_polygonSymbol;
    break;

  default:
    return;
  }

  // Append the new graphic to the graphics overlay
  m_graphicsOverlay->graphics()->append(new Graphic(geometry, geometrySymbol, m_tempGraphicsParent));
}

void CreateAndEditGeometries::setTool(GeometryEditorToolType toolType)
{
  if (toolType == GeometryEditorToolType::Freehand)
    m_geometryEditor->setTool(new FreehandTool(this));
  else // toolType == GeometryEditorToolType::Vertex
    m_geometryEditor->setTool(new VertexTool(this));
}

void CreateAndEditGeometries::clearGraphics()
{
  m_graphicsOverlay->graphics()->clear();

  // Clearing the graphics overlay does not delete the graphics it contains
  // We can delete the graphics' shared parent to easily delete all of them and prevent memory leaks
  delete m_tempGraphicsParent;
  m_tempGraphicsParent = new QObject(this);
}

void CreateAndEditGeometries::undo()
{
  if (m_geometryEditor->canUndo())
    m_geometryEditor->undo();
  emit canUndoOrRedoChanged();
}

void CreateAndEditGeometries::redo()
{
  if (m_geometryEditor->canRedo())
    m_geometryEditor->redo();
  emit canUndoOrRedoChanged();
}

void CreateAndEditGeometries::deleteSelectedElement()
{
  m_geometryEditor->deleteSelectedElement();
  emit canUndoOrRedoChanged();
}

MapQuickView* CreateAndEditGeometries::mapView() const
{
  return m_mapView;
}

// Create slots for asynchronous signals
void CreateAndEditGeometries::createConnections()
{
  // Allow user to edit existing graphics by clicking on them
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& mouseEvent)
          {
            if (!m_geometryEditor->isStarted())
            {
              m_mapView->identifyGraphicsOverlay(m_graphicsOverlay, mouseEvent.position().x(), mouseEvent.position().y(), 5 ,false);
            }
            emit canUndoOrRedoChanged();
            emit elementIsSelectedChanged();
          });

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](const QMouseEvent&)
          {
            emit canUndoOrRedoChanged();
            emit elementIsSelectedChanged();
          });

  // Handle editing selected graphics, if any
  connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, [this](QUuid, IdentifyGraphicsOverlayResult* rawResult)
          {
            auto identifyResult = std::unique_ptr<IdentifyGraphicsOverlayResult>(rawResult);

            // Return if no graphics were identified
            if (identifyResult->graphics().isEmpty())
              return;

            m_editingGraphic = identifyResult->graphics().first();

            // Hide the graphic currently being edited
            m_editingGraphic->setVisible(false);

            // Start the geometry editor with the graphic's geometry. This does not directly affect the graphic.
            m_geometryEditor->start(m_editingGraphic->geometry());

            emit geometryEditorStartedChanged();
          });
}

// State checks to control when buttons are enabled
bool CreateAndEditGeometries::geometryEditorStarted() const
{
  return (m_geometryEditor && m_geometryEditor->isStarted());
}

bool CreateAndEditGeometries::canUndo()
{
  return (m_geometryEditor && m_geometryEditor->canUndo());
}

bool CreateAndEditGeometries::canRedo()
{
  return (m_geometryEditor && m_geometryEditor->canRedo());
}

bool CreateAndEditGeometries::elementIsSelected()
{
  return (m_geometryEditor && m_geometryEditor->selectedElement() && m_geometryEditor->selectedElement()->canDelete());
}

// Create symbols used by all graphics
void CreateAndEditGeometries::createInitialSymbols()
{
  m_pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 10, this);
  m_multiPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(255, 255, 0), 5, this);
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(0, 0, 255), 2, this);
  m_polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75),
                                         new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 0), 1.0, this), this);
}

// Create graphics that are present upon sample instantiation
void CreateAndEditGeometries::createInitialGraphics()
{
  // Create a temporary parent to give all the builders so we can easily delete them later
  QObject* tempParent = new QObject(this);

  // Create point geometry
  const Point house(-1067898.59, 6998366.62, SpatialReference::webMercator());

  // Create multipoint geometry
  MultipointBuilder* multipointBuilder = new MultipointBuilder(SpatialReference::webMercator(), tempParent);
  PointCollection* pointCollection = new PointCollection(SpatialReference::webMercator(), tempParent);
  pointCollection->addPoints(
      {
          Point(-1067984.26, 6998346.28), Point(-1067966.80, 6998244.84),
          Point(-1067921.88, 6998284.65), Point(-1067934.36, 6998340.74),
          Point(-1067917.93, 6998373.97), Point(-1067828.30, 6998355.28),
          Point(-1067832.25, 6998339.70), Point(-1067823.10, 6998336.93),
          Point(-1067873.22, 6998386.78), Point(-1067896.72, 6998244.49)
      });
  multipointBuilder->setPoints(pointCollection);
  const Multipoint outbuildings(multipointBuilder->toMultipoint());

  // Create polyline geometries
  PolylineBuilder* polylineBuilder = new PolylineBuilder(SpatialReference::webMercator(), tempParent);
  polylineBuilder->addPoints(
      {
          Point(-1068095.40, 6998123.52), Point(-1068086.16, 6998134.60),
          Point(-1068083.20, 6998160.44), Point(-1068104.27, 6998205.37),
          Point(-1068070.63, 6998255.22), Point(-1068014.44, 6998291.54),
          Point(-1067952.33, 6998351.85), Point(-1067927.93, 6998386.93),
          Point(-1067907.97, 6998396.78), Point(-1067889.86, 6998406.63),
          Point(-1067848.08, 6998495.26), Point(-1067832.92, 6998521.11)
      });

  const Polyline road1(polylineBuilder->toPolyline());
  delete polylineBuilder;

  polylineBuilder = new PolylineBuilder(SpatialReference::webMercator(), tempParent);
  polylineBuilder->addPoints(
      {
          Point(-1067999.28, 6998061.97), Point(-1067994.48, 6998086.59),
          Point(-1067964.53, 6998125.37), Point(-1067952.70, 6998215.84),
          Point(-1067923.13, 6998347.54), Point(-1067903.90, 6998391.86),
          Point(-1067895.40, 6998422.02), Point(-1067891.70, 6998460.18),
          Point(-1067889.49, 6998483.56), Point(-1067880.98, 6998527.26)
      });

  const Polyline road2(polylineBuilder->toPolyline());

  // Create polygon geometry
  PolygonBuilder* polygonBuilder = new PolygonBuilder(SpatialReference::webMercator(), tempParent);
  polygonBuilder->addPoints(
      {
          Point(-1067943.67, 6998403.86), Point(-1067938.17, 6998427.60),
          Point(-1067898.77, 6998415.86), Point(-1067888.26, 6998398.80),
          Point(-1067800.85, 6998372.93), Point(-1067799.61, 6998342.81),
          Point(-1067809.38, 6998330.00), Point(-1067817.07, 6998307.85),
          Point(-1067838.07, 6998285.34), Point(-1067849.10, 6998250.38),
          Point(-1067874.02, 6998256.00), Point(-1067879.87, 6998235.95),
          Point(-1067913.41, 6998245.03), Point(-1067934.84, 6998291.34),
          Point(-1067948.41, 6998251.90), Point(-1067961.18, 6998186.68),
          Point(-1068008.59, 6998199.49), Point(-1068052.89, 6998225.45),
          Point(-1068039.37, 6998261.11), Point(-1068064.12, 6998265.26),
          Point(-1068043.32, 6998299.88), Point(-1068036.25, 6998327.93),
          Point(-1068004.43, 6998409.28), Point(-1067943.67, 6998403.86)
      });

  const Polygon boundary(polygonBuilder->toPolygon());

  // Delete the builders' parent to clean up all objects used in creating the graphics
  delete tempParent;

  // Create graphics with the geometries
  m_graphicsOverlay->graphics()->append(
      {
          new Graphic(boundary, m_polygonSymbol, m_tempGraphicsParent),
          new Graphic(road2, m_lineSymbol, m_tempGraphicsParent),
          new Graphic(road1, m_lineSymbol, m_tempGraphicsParent),
          new Graphic(outbuildings, m_multiPointSymbol, m_tempGraphicsParent),
          new Graphic(house, m_pointSymbol, m_tempGraphicsParent)
      });
}
