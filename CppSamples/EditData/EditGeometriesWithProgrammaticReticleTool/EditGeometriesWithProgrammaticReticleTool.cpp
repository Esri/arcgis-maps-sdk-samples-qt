// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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
#include "EditGeometriesWithProgrammaticReticleTool.h"

// ArcGIS Maps SDK headers
#include "Envelope.h"
#include "GeometryEditor.h"
#include "GeometryEditorElement.h"
#include "GeometryEditorGrowEffect.h"
#include "GeometryEditorMidVertex.h"
#include "GeometryEditorStyle.h"
#include "GeometryEditorTypes.h"
#include "GeometryEditorVertex.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGeometryEditorResult.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "ImmutablePartCollection.h"
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
#include "ProgrammaticReticleTool.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

// Hold references to the geometry editor and programmatic reticle tool.
EditGeometriesWithProgrammaticReticleTool::EditGeometriesWithProgrammaticReticleTool(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this)),
  m_graphicsOverlay(new GraphicsOverlay(this)),
  m_pointSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 10, this)),
  m_multiPointSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(255, 255, 0), 5, this)),
  m_lineSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(0, 0, 255), 2, this)),
  m_polygonSymbol(new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75),
                                       new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 0), 1.0, this), this)),
  m_geometryEditor(new GeometryEditor(this)),
  m_reticleTool(new ProgrammaticReticleTool(this))
{
}

EditGeometriesWithProgrammaticReticleTool::~EditGeometriesWithProgrammaticReticleTool() = default;

void EditGeometriesWithProgrammaticReticleTool::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<EditGeometriesWithProgrammaticReticleTool>("Esri.Samples", 1, 0, "EditGeometriesWithProgrammaticReticleToolSample");
}

// Set the view (created in QML)
void EditGeometriesWithProgrammaticReticleTool::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->setViewpointCenterAsync(Point(-0.775395, 51.523806, SpatialReference::wgs84()), 50000);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);

  m_geometryEditor->setTool(m_reticleTool);

  emit mapViewChanged();

  createInitialGraphics();
  createConnections();
}

// Create graphics that are present upon sample instantiation
void EditGeometriesWithProgrammaticReticleTool::createInitialGraphics()
{
  // Create a temporary parent to give all the builders so we can easily delete them later
  auto* tempParent = new QObject(this);

  // Polygon (Pinkneys Green) - WGS84 coordinates
  auto* polygonBuilder = new PolygonBuilder(SpatialReference::wgs84(), tempParent);
  polygonBuilder->addPoints({
    Point(-0.775395, 51.523806), Point(-0.789000, 51.532000),
    Point(-0.783000, 51.535000), Point(-0.779000, 51.536000),
    Point(-0.777000, 51.535000), Point(-0.776970, 51.531000),
    Point(-0.774000, 51.526000), Point(-0.773000, 51.524000),
    Point(-0.775395, 51.523806)
  });
  const Polygon pinkneysGreen(polygonBuilder->toPolygon());

  // Polyline (Beech Lodge Boundary) - WGS84 coordinates
  auto* polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), tempParent);
  polylineBuilder->addPoints({
    Point(-0.810000, 51.525000), Point(-0.812000, 51.526000),
    Point(-0.811700, 51.530000), Point(-0.806000, 51.528000),
    Point(-0.803000, 51.527000), Point(-0.802990, 51.526000)
  });
  const Polyline beechLodgeBoundary(polylineBuilder->toPolyline());

  // Multipoint (Tree Markers) - WGS84 coordinates
  auto* multipointBuilder = new MultipointBuilder(SpatialReference::wgs84(), tempParent);
  auto* pointCollection = new PointCollection(SpatialReference::wgs84(), tempParent);
  pointCollection->addPoints({
    Point(-0.803000, 51.523800), Point(-0.805000, 51.520000),
    Point(-0.816000, 51.527000), Point(-0.815000, 51.527300),
    Point(-0.804000, 51.527200), Point(-0.804400, 51.527170),
    Point(-0.803500, 51.523000)
  });
  multipointBuilder->setPoints(pointCollection);
  const Multipoint treeMarkers(multipointBuilder->toMultipoint());

  // Point (house) - use first tree marker as example, WGS84
  const Point house(-0.803000, 51.523800, SpatialReference::wgs84());

  // Add graphics
  m_graphicsOverlay->graphics()->append({
    new Graphic(pinkneysGreen, m_polygonSymbol, this),
    new Graphic(beechLodgeBoundary, m_lineSymbol, this),
    new Graphic(treeMarkers, m_multiPointSymbol, this),
    new Graphic(house, m_pointSymbol, this)
  });
}

void EditGeometriesWithProgrammaticReticleTool::createConnections()
{
  // Allow user to edit existing graphics by clicking on them
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& mouseEvent)
  {
    handleMapTap(mouseEvent);
  });

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](const QMouseEvent&)
  {
    setMultifunctionButtonState();
  });

  connect(m_geometryEditor, &GeometryEditor::hoveredElementChanged, this, [this]()
  {
    setMultifunctionButtonState();
  });

  connect(m_geometryEditor, &GeometryEditor::pickedUpElementChanged, this, [this]()
  {
    setMultifunctionButtonState();
  });
}

void EditGeometriesWithProgrammaticReticleTool::handleMapTap(const QMouseEvent& mouseEvent)
{
  // If the geometry editor is started, identify geometry editor elements at the clicked position
  if (m_geometryEditor->isStarted())
  {
    m_mapView->identifyGeometryEditorAsync(mouseEvent.position(), 10).then(this, [this](IdentifyGeometryEditorResult* result)
    {
      std::unique_ptr<IdentifyGeometryEditorResult> identifyResult(result);
      if (identifyResult && !identifyResult->elements().isEmpty())
      {
        auto* element = identifyResult->elements().first();
        // If the element is a vertex or mid-vertex, zoom to it and select it
        if (auto* vertex = dynamic_cast<GeometryEditorVertex*>(element); vertex)
        {
          m_mapView->setViewpointAsync(vertex->point(), m_mapView->mapScale());
          m_geometryEditor->selectVertex(vertex->partIndex(), vertex->vertexIndex());
        }
        else if (auto* midVertex = dynamic_cast<GeometryEditorMidVertex*>(element); midVertex)
        {
          if (m_vertexCreationAllowed)
          {
            m_mapView->setViewpointAsync(vertex->point(), m_mapView->mapScale());
            m_geometryEditor->selectMidVertex(midVertex->partIndex(), midVertex->segmentIndex());
          }
        }
      }
    });
    return;
  }

  // If geometry editor is not started, identify graphics in the overlay
  m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10, false)
    .then(this, [this](IdentifyGraphicsOverlayResult* rawResult)
  {
    auto identifyResult = std::unique_ptr<IdentifyGraphicsOverlayResult>(rawResult);
    if (!identifyResult || identifyResult->graphics().isEmpty())
    {
      m_editingGraphic = nullptr;
      return;
    }
    m_editingGraphic = identifyResult->graphics().first();
    m_editingGraphic->setSelected(true);

    // Hide the selected graphic and start editing
    m_editingGraphic->setVisible(false);

    // Hide the selected graphic and start an editing session with a copy of it.
    m_geometryEditor->start(m_editingGraphic->geometry());
    emit geometryEditorStartedChanged();

    // Optionally zoom to the selected graphic
    if (m_vertexCreationAllowed)
    {
      m_mapView->setViewpointCenterAsync(m_editingGraphic->geometry().extent().center(), m_mapView->mapScale());
    }
    else
    {
      m_mapView->setViewpointAsync(m_editingGraphic->geometry().extent().center(), m_mapView->mapScale());
      //TODO set the viewpoint to the end point of the first part of the geometry.
    }
  });
  //on Error resetFromEditingSession();
  // Set the button text to indicate that the editor is active.
  setMultifunctionButtonState();
}

void EditGeometriesWithProgrammaticReticleTool::handleMultifunctionButton()
{
  // If the geometry editor is not started, start it with the selected geometry type
  if (!m_geometryEditor->isStarted())
  {
    switch (m_selectedGeometryEditorType)
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
    setMultifunctionButtonState();
    return;
  }

  if (m_vertexCreationAllowed)
  {
    switch (m_reticleState)
    {
      case ReticleState::Default:
      case ReticleState::PickedUp:
        m_reticleTool->placeElementAtReticle();
        break;
      case ReticleState::HoveringVertex:
      case ReticleState::HoveringMidVertex:
        m_reticleTool->selectElementAtReticle();
        m_reticleTool->pickUpSelectedElement();
        break;
    }
  }
  else
  {
    switch (m_reticleState)
    {
      case ReticleState::PickedUp:
        m_reticleTool->placeElementAtReticle();
        break;
      case ReticleState::HoveringVertex:
        m_reticleTool->selectElementAtReticle();
        m_reticleTool->pickUpSelectedElement();
        break;
      default:
        // In edit mode, only picked up vertices can be placed and only vertices can be picked up.
        break;
    }
  }
  setMultifunctionButtonState();
}

void EditGeometriesWithProgrammaticReticleTool::saveEdits()
{
  // Stop the geometry editor and get the resulting geometry
  const auto geometry = m_geometryEditor->stop();
  emit geometryEditorStartedChanged();

  if (!geometry.isEmpty())
  {
    if (m_editingGraphic)
    {
      // Update the geometry of the graphic being edited and make it visible again
      m_editingGraphic->setGeometry(geometry);
      m_editingGraphic->setVisible(true);
      m_editingGraphic = nullptr;
    }
    else
    {
      Symbol* geometrySymbol = getSymbol(geometry.geometryType());
      if (geometrySymbol)
      {
        m_graphicsOverlay->graphics()->append(new Graphic(geometry, geometrySymbol, this));
      }
    }
  }
  resetFromEditingSession();
}

Symbol* EditGeometriesWithProgrammaticReticleTool::getSymbol(GeometryType type) const
{
  switch (type)
  {
    case GeometryType::Point:
      return m_pointSymbol;
    case GeometryType::Multipoint:
      return m_multiPointSymbol;
    case GeometryType::Polyline:
      return m_lineSymbol;
    case GeometryType::Polygon:
      return m_polygonSymbol;
    default:
      return nullptr;
  }
}

void EditGeometriesWithProgrammaticReticleTool::discardEdits()
{
  m_geometryEditor->stop();
  resetFromEditingSession();
}

void EditGeometriesWithProgrammaticReticleTool::resetFromEditingSession()
{
  // Reset the selected graphic
  if (m_editingGraphic)
  {
    m_editingGraphic->setSelected(false);
    m_editingGraphic->setVisible(true);
  }
  m_editingGraphic = nullptr;

  // Update the multifunction button text and enable it
  m_multifunctionButtonText = "Start geometry editor";
  m_multifunctionButtonEnabled = true;

  emit multifunctionButtonTextChanged();
  emit multifunctionButtonEnabledChanged();
  emit canUndoOrRedoChanged();
  emit selectedElementCanDeleteChanged();
}

bool EditGeometriesWithProgrammaticReticleTool::vertexCreationAllowed() const
{
  return m_vertexCreationAllowed;
}

void EditGeometriesWithProgrammaticReticleTool::setVertexCreationAllowed(bool allowed)
{
  if (m_vertexCreationAllowed != allowed)
  {
    m_vertexCreationAllowed = allowed;
    // Toggle the reticle tool's vertex creation preview and grow effect for mid-vertices
    if (m_reticleTool)
    {
      m_reticleTool->setVertexCreationPreviewEnabled(allowed);
      if (auto* style = m_reticleTool->style(); style)
      {
        if (auto* growEffect = style->growEffect(); growEffect)
        {
          growEffect->setApplyToMidVertices(allowed);
        }
      }
    }
    emit vertexCreationAllowedChanged();
  }
}

void EditGeometriesWithProgrammaticReticleTool::setSelectedGeometryType(GeometryEditorMode mode)
{
  if (m_selectedGeometryEditorType != mode)
  {
    m_selectedGeometryEditorType = mode;
    emit selectedGeometryTypeChanged();
  }
}

bool EditGeometriesWithProgrammaticReticleTool::selectedElementCanDelete() const
{
  return m_mapView && m_mapView->geometryEditor() && m_mapView->geometryEditor()->selectedElement() &&
         m_mapView->geometryEditor()->selectedElement()->canDelete();
}

void EditGeometriesWithProgrammaticReticleTool::deleteSelectedElement()
{
  m_geometryEditor->deleteSelectedElement();
  emit selectedElementCanDeleteChanged();
  emit canUndoOrRedoChanged();
}

EditGeometriesWithProgrammaticReticleTool::GeometryEditorMode EditGeometriesWithProgrammaticReticleTool::selectedGeometryType() const
{
  return m_selectedGeometryEditorType;
}

MapQuickView* EditGeometriesWithProgrammaticReticleTool::mapView() const
{
  return m_mapView;
}

// State checks to control when buttons are enabled
bool EditGeometriesWithProgrammaticReticleTool::geometryEditorStarted() const
{
  return (m_mapView && m_mapView->geometryEditor() && m_mapView->geometryEditor()->isStarted());
}

bool EditGeometriesWithProgrammaticReticleTool::canUndo()
{
  return (m_mapView && m_mapView->geometryEditor() && m_mapView->geometryEditor()->canUndo());
}

bool EditGeometriesWithProgrammaticReticleTool::canRedo()
{
  return (m_mapView && m_mapView->geometryEditor() && m_mapView->geometryEditor()->canRedo());
}

void EditGeometriesWithProgrammaticReticleTool::undoOrCancel()
{
  if (m_geometryEditor->pickedUpElement())
  {
    m_geometryEditor->cancelCurrentAction();
  }
  else if (m_geometryEditor->canUndo())
  {
    m_geometryEditor->undo();
  }
  emit canUndoOrRedoChanged();
}

void EditGeometriesWithProgrammaticReticleTool::redo()
{
  if (m_geometryEditor->canRedo())
  {
    m_geometryEditor->redo();
    emit canUndoOrRedoChanged();
  }
}

QString EditGeometriesWithProgrammaticReticleTool::multifunctionButtonText() const
{
  return m_multifunctionButtonText;
}

bool EditGeometriesWithProgrammaticReticleTool::multifunctionButtonEnabled() const
{
  return m_multifunctionButtonEnabled;
}

void EditGeometriesWithProgrammaticReticleTool::setMultifunctionButtonState()
{
  QString newText;
  bool newEnabled = true;
  ReticleState newReticleState = ReticleState::Default;

  if (!m_geometryEditor->isStarted())
  {
    newText = "Start geometry editor";
  }
  else if (m_geometryEditor->pickedUpElement())
  {
    newText = "Drop point";
    newReticleState = ReticleState::PickedUp;
  }
  else if (m_vertexCreationAllowed)
  {
    auto* hovered = dynamic_cast<GeometryEditorElement*>(m_geometryEditor->hoveredElement());
    if (hovered)
    {
      auto type = hovered->geometryEditorElementType();
      if (type == GeometryEditorElementType::GeometryEditorVertex)
      {
        newText = "Pick up point";
        newReticleState = ReticleState::HoveringVertex;
      }
      else if (type == GeometryEditorElementType::GeometryEditorMidVertex)
      {
        newText = "Insert point";
        newReticleState = ReticleState::HoveringMidVertex;
      }
      else
      {
        newText = "Insert point";
        newEnabled = false;
      }
    }
    else
    {
      newText = "Insert point";
    }
  }
  else
  {
    auto* hovered = dynamic_cast<GeometryEditorElement*>(m_geometryEditor->hoveredElement());
    if (hovered && hovered->geometryEditorElementType() == GeometryEditorElementType::GeometryEditorVertex)
    {
      newText = "Pick up point";
      newReticleState = ReticleState::HoveringVertex;
    }
    else
    {
      newText = "Insert point";
    }
  }

  if (m_multifunctionButtonText != newText)
  {
    m_multifunctionButtonText = newText;
    emit multifunctionButtonTextChanged();
  }

  if (m_multifunctionButtonEnabled != newEnabled)
  {
    m_multifunctionButtonEnabled = newEnabled;
    emit multifunctionButtonEnabledChanged();
  }

  m_reticleState = newReticleState;
  emit selectedElementCanDeleteChanged();
  emit canUndoOrRedoChanged();
}
