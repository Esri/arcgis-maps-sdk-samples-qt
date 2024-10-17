// [WriteFile Name=CreateAndEditGeometries, Category=Geometry]
// [Legal]
// Copyright 2023 Esri.
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
#include "CreateAndEditGeometries.h"

// ArcGIS Maps SDK headers
#include "FreehandTool.h"
#include "GeometryEditor.h"
#include "GeometryEditorElement.h"
#include "GeometryEditorTypes.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "InteractionConfiguration.h"
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
#include "ReticleVertexTool.h"
#include "ShapeTool.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "VertexTool.h"

// Qt headers
#include <QFuture>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

CreateAndEditGeometries::CreateAndEditGeometries(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this))
{
  m_geometryEditor = new GeometryEditor(this);
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_tempGraphicsParent = new QObject(this);

  // Create the tools to toggle between
  m_vertexTool = new VertexTool(this);
  m_freehandTool = new FreehandTool(this);
  m_arrowTool = ShapeTool::create(ShapeToolType::Arrow, this);
  m_ellipseTool = ShapeTool::create(ShapeToolType::Ellipse, this);
  m_rectangleTool = ShapeTool::create(ShapeToolType::Rectangle, this);
  m_triangleTool = ShapeTool::create(ShapeToolType::Triangle, this);
  m_reticleTool = new ReticleVertexTool(this);
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

  m_mapView->setViewpointCenterAsync(Point(-9.5920, 53.08230, SpatialReference::wgs84()), 5000);

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
  switch (toolType)
  {
    case GeometryEditorToolType::Vertex:
      m_geometryEditor->setTool(m_vertexTool);
      break;

    case GeometryEditorToolType::Freehand:
      m_geometryEditor->setTool(m_freehandTool);
      break;

    case GeometryEditorToolType::Arrow:
      m_geometryEditor->setTool(m_arrowTool);
      break;

    case GeometryEditorToolType::Ellipse:
      m_geometryEditor->setTool(m_ellipseTool);
      break;

    case GeometryEditorToolType::Rectangle:
      m_geometryEditor->setTool(m_rectangleTool);
      break;

    case GeometryEditorToolType::Triangle:
      m_geometryEditor->setTool(m_triangleTool);
      break;

    case GeometryEditorToolType::Reticle:
      m_geometryEditor->setTool(m_reticleTool);

    default:
      break;
  }
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

void CreateAndEditGeometries::setScaleMode(ScaleMode scaleMode)
{
  const GeometryEditorScaleMode toolScaleMode = (scaleMode == ScaleMode::UniformScaleMode) ? GeometryEditorScaleMode::Uniform : GeometryEditorScaleMode::Stretch;

  // Set the scale mode on all available tools
  m_vertexTool->configuration()->setScaleMode(toolScaleMode);
  m_freehandTool->configuration()->setScaleMode(toolScaleMode);
  m_arrowTool->configuration()->setScaleMode(toolScaleMode);
  m_ellipseTool->configuration()->setScaleMode(toolScaleMode);
  m_rectangleTool->configuration()->setScaleMode(toolScaleMode);
  m_triangleTool->configuration()->setScaleMode(toolScaleMode);
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
      m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 5 ,false).then(this,
      [this](IdentifyGraphicsOverlayResult* rawResult){
        // Handle editing selected graphics, if any

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
    emit canUndoOrRedoChanged();
    emit elementIsSelectedChanged();
  });

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](const QMouseEvent&)
  {
    emit canUndoOrRedoChanged();
    emit elementIsSelectedChanged();
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
  const Point house(-9.59309629, 53.0830063, SpatialReference::wgs84());

  // Create multipoint geometry
  MultipointBuilder* multipointBuilder = new MultipointBuilder(SpatialReference::wgs84(), tempParent);
  PointCollection* pointCollection = new PointCollection(SpatialReference::wgs84(), tempParent);
  pointCollection->addPoints(
        {
          Point(-9.59386587, 53.08289651), Point(-9.59330546, 53.08256400),
          Point(-9.59326997, 53.08304595), Point(-9.59250034, 53.08286101),
          Point(-9.59286835, 53.08311506), Point(-9.59370896, 53.08234917),
          Point(-9.59341755, 53.08286662), Point(-9.59246485, 53.08294507),
          Point(-9.59241815, 53.08284607), Point(-9.59307943, 53.08234731)
        });
  multipointBuilder->setPoints(pointCollection);
  const Multipoint outbuildings(multipointBuilder->toMultipoint());

  // Create polyline geometries
  PolylineBuilder* polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), tempParent);
  polylineBuilder->addPoints(
        {
          Point(-9.59486423, 53.08169453), Point(-9.5947812, 53.081754310),
          Point(-9.59475464, 53.08189379), Point(-9.59494393, 53.08213622),
          Point(-9.59464173, 53.08240521), Point(-9.59413694, 53.08260115),
          Point(-9.59357903, 53.08292660), Point(-9.59335984, 53.08311589),
          Point(-9.59318051, 53.08316903), Point(-9.59301779, 53.08322216),
          Point(-9.59264252, 53.08370038), Point(-9.59250636, 53.08383986)
        });

  const Polyline road1(polylineBuilder->toPolyline());
  delete polylineBuilder;

  polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), tempParent);
  polylineBuilder->addPoints(
        {
          Point(-9.59400079, 53.08136244), Point(-9.59395761, 53.08149528),
          Point(-9.59368862, 53.08170450), Point(-9.59358235, 53.08219267),
          Point(-9.59331667, 53.08290335), Point(-9.59314398, 53.08314246),
          Point(-9.59306760, 53.08330519), Point(-9.59303439, 53.08351109),
          Point(-9.59301447, 53.08363728), Point(-9.59293809, 53.08387307)
        });

  const Polyline road2(polylineBuilder->toPolyline());

  // Create polygon geometry
  PolygonBuilder* polygonBuilder = new PolygonBuilder(SpatialReference::wgs84(), tempParent);
  polygonBuilder->addPoints(
        {
          Point(-9.59350122, 53.08320723), Point(-9.59345177, 53.08333534),
          Point(-9.59309789, 53.08327198), Point(-9.59300344, 53.08317992),
          Point(-9.59221827, 53.08304034), Point(-9.59220706, 53.08287782),
          Point(-9.59229486, 53.08280871), Point(-9.59236398, 53.08268915),
          Point(-9.59255263, 53.08256769), Point(-9.59265165, 53.08237906),
          Point(-9.59287552, 53.08241478), Point(-9.59292812, 53.08230120),
          Point(-9.59322940, 53.08235022), Point(-9.59342188, 53.08260009),
          Point(-9.59354382, 53.08238728), Point(-9.59365852, 53.08203535),
          Point(-9.59408443, 53.08210446), Point(-9.59448232, 53.08224456),
          Point(-9.59436090, 53.08243697), Point(-9.59458319, 53.08245939),
          Point(-9.59439639, 53.08264619), Point(-9.59433288, 53.08279750),
          Point(-9.59404707, 53.08323649), Point(-9.59350122, 53.08320723)
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
