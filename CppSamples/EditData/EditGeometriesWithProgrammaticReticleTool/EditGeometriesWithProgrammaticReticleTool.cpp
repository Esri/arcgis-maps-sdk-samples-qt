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
#include "ErrorException.h"
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
#include "ImmutablePart.h"
#include "ImmutablePartCollection.h"
#include "ImmutablePointCollection.h"
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

  m_mapView->setViewpointCenterAsync(Point(-0.775395, 51.523806, SpatialReference::wgs84()), 20000);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);

  m_geometryEditor->setTool(m_reticleTool);

  emit mapViewChanged();

  createInitialGraphics();
  createConnections();
}

MapQuickView* EditGeometriesWithProgrammaticReticleTool::mapView() const
{
  return m_mapView;
}

void EditGeometriesWithProgrammaticReticleTool::createInitialGraphics()
{
  const QString pinkneysGreenJson = R"({"rings":[[[-84843.262719916485,6713749.9329888355],[-85833.376589175183,6714679.7122141244], [-85406.822347959576,6715063.9827222107],[-85184.329997390232,6715219.6195847588], [-85092.653857582554,6715119.5391713539],[-85090.446872787768,6714792.7656492386], [-84915.369168906298,6714297.8798246197],[-84854.295522911285,6714080.907587287], [-84843.262719916485,6713749.9329888355]]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";
  const QString beechLodgeBoundaryJson = R"({"paths":[[[-87090.652708065536,6714158.9244240439],[-87247.362370337316,6714232.880689906], [-87226.314032974493,6714605.4697726099],[-86910.499335316243,6714488.006312645], [-86750.82198052686,6714401.1768307304],[-86749.846825938366,6714305.8450344801]]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";
  const QString treeMarkersJson = R"({"points":[[-86750.751150056443,6713749.4529355941],[-86879.381793060631,6713437.3335486846], [-87596.503104619667,6714381.7342108283],[-87553.257569537804,6714402.0910389507], [-86831.019903597829,6714398.4128562529],[-86854.105933315877,6714396.1957954112], [-86800.624094892439,6713992.3374453448]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";

  const Polygon pinkneysGreen = geometry_cast<Polygon>(Polygon::fromJson(pinkneysGreenJson));
  const Polyline beechLodgeBoundary = geometry_cast<Polyline>(Polyline::fromJson(beechLodgeBoundaryJson));
  const Multipoint treeMarkers = geometry_cast<Multipoint>(Multipoint::fromJson(treeMarkersJson));

  // Add graphics
  m_graphicsOverlay->graphics()->append({
    new Graphic(pinkneysGreen, m_polygonSymbol, this),
    new Graphic(beechLodgeBoundary, m_lineSymbol, this),
    new Graphic(treeMarkers, m_multiPointSymbol, this),
  });
}

void EditGeometriesWithProgrammaticReticleTool::createConnections()
{
  // Allow user to edit existing graphics by clicking on them
  connect(m_mapView, &MapQuickView::mouseClicked, this, &EditGeometriesWithProgrammaticReticleTool::handleMapTap);

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, &EditGeometriesWithProgrammaticReticleTool::setMultifunctionButtonState);

  connect(m_geometryEditor, &GeometryEditor::hoveredElementChanged, this, &EditGeometriesWithProgrammaticReticleTool::setMultifunctionButtonState);
  connect(m_geometryEditor, &GeometryEditor::pickedUpElementChanged, this, &EditGeometriesWithProgrammaticReticleTool::setMultifunctionButtonState);

}

void EditGeometriesWithProgrammaticReticleTool::handleMapTap(const QMouseEvent& mouseEvent)
{
  // If the geometry editor is started, identify the geometry editor result at the tapped position.
  if (m_geometryEditor->isStarted())
  {
    // Identify the geometry editor result at the tapped position.
    m_mapView->identifyGeometryEditorAsync(mouseEvent.position(), 10, this).then(this, [this](IdentifyGeometryEditorResult* result)
    {
      std::unique_ptr<IdentifyGeometryEditorResult> identifyResult(result);
      if (!identifyResult || identifyResult->elements().isEmpty())
      {
        return;
      }
      // Get the first element from the result.
      GeometryEditorElement* element = identifyResult->elements().first();
      element->setParent(this);

      // If the element is a vertex or mid-vertex, set the viewpoint to its position and select it.
      if (GeometryEditorVertex* vertex = dynamic_cast<GeometryEditorVertex*>(element); vertex)
      {
        m_mapView->setViewpointAsync(Viewpoint(Point(vertex->point().x(), vertex->point().y(), vertex->point().spatialReference())));
        m_geometryEditor->selectVertex(vertex->partIndex(), vertex->vertexIndex());
      }
      else if (GeometryEditorMidVertex* midVertex = dynamic_cast<GeometryEditorMidVertex*>(element); midVertex && m_vertexCreationAllowed)
      {
        m_mapView->setViewpointAsync(Viewpoint(Point(midVertex->point().x(), midVertex->point().y(), midVertex->point().spatialReference())));
        m_geometryEditor->selectMidVertex(midVertex->partIndex(), midVertex->segmentIndex());
      }
    }).onFailed(this, [this](const ErrorException& error)
    {
      qDebug() << "Error editing! Identify geometry editor failed with error " << error.error().message();
      resetFromEditingSession();
    });
  }
  else
  {
    // Identify graphics in the graphics overlay using the tapped position.
    m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10, false, this)
      .then(this, [this](IdentifyGraphicsOverlayResult* rawResult)
    {
      auto identifyResult = std::unique_ptr<IdentifyGraphicsOverlayResult>(rawResult);
      if (!identifyResult || identifyResult->graphics().isEmpty())
      {
        m_editingGraphic = nullptr;
        return;
      }

      // Try to get the first graphic from the first result.
      m_editingGraphic = identifyResult->graphics().first();

      // Return if no graphic was selected.
      if (m_editingGraphic == nullptr)
      {
        return;
      }

      m_editingGraphic->setSelected(true);

      // Hide the selected graphic and start an editing session with a copy of it.
      m_geometryEditor->start(m_editingGraphic->geometry());
      emit geometryEditorStartedChanged();

      // Set the button text to indicate that the editor is active.
      setMultifunctionButtonState();

      // If vertex creation is allowed, set the viewpoint to the center of the selected graphic's geometry.
      // Otherwise, set the viewpoint to the end point of the first part of the geometry.
      if (m_vertexCreationAllowed)
      {
        m_mapView->setViewpointCenterAsync(m_editingGraphic->geometry().extent().center(), m_mapView->mapScale());
      }
      else
      {
        const Geometry geometry = m_editingGraphic->geometry();
        Point targetPoint;
        switch (geometry.geometryType())
        {
          case GeometryType::Polygon:
          {
            if (const Polygon polygon = geometry_cast<Polygon>(geometry); !polygon.parts().isEmpty())
            {
              targetPoint = polygon.parts().part(0).endPoint();
            }
            break;
          }
          case GeometryType::Polyline:
          {
            if (const Polyline polyline = geometry_cast<Polyline>(geometry); !polyline.parts().isEmpty())
            {
              targetPoint = polyline.parts().part(0).endPoint();
            }
            break;
          }
          case GeometryType::Multipoint:
          {
            if (const Multipoint multiPoint = geometry_cast<Multipoint>(geometry); !multiPoint.points().isEmpty())
            {
              targetPoint = multiPoint.points().point(multiPoint.points().size() - 1);
            }
            break;
          }
          case GeometryType::Point:
          {
            targetPoint = geometry_cast<Point>(geometry);
            break;
          }
          default:
            break;
        }
        m_mapView->setViewpointAsync(!targetPoint.isEmpty() ? targetPoint : geometry.extent().center(), m_mapView->mapScale());

        // Hide the selected graphic while editing.
        m_editingGraphic->setVisible(false);
      }
    }).onFailed(this, [this](const ErrorException& error)
    {
      qDebug() << "Error editing! Identify failed with error " << error.error().message();
      resetFromEditingSession();
    });
  }
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
    // Set the button text to indicate that the editor is active.
    setMultifunctionButtonState();
    return;
  }

  if (m_vertexCreationAllowed)
  {
    // When vertex creation is allowed vertices and mid-vertices can be picked up, new vertices can be inserted.
    switch (m_reticleState)
    {
      case ReticleState::Default: [[fallthrough]];
      case ReticleState::PickedUp:
        m_reticleTool->placeElementAtReticle();
        break;
      case ReticleState::HoveringVertex: [[fallthrough]];
      case ReticleState::HoveringMidVertex:
        m_reticleTool->selectElementAtReticle();
        m_reticleTool->pickUpSelectedElement();
        break;
    }
  }
  else
  {
    // When vertex creation is not allowed functionality is limited to picking up and moving existing vertices, mid-vertices cannot be picked up.
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
  const Geometry geometry = m_geometryEditor->stop();
  emit geometryEditorStartedChanged();

  if (!geometry.isEmpty())
  {
    if (m_editingGraphic)
    {
      // Update the geometry of the graphic being edited and make it visible again
      m_editingGraphic->setGeometry(geometry);
      m_editingGraphic->setVisible(true);
    }
    else
    {
      Symbol* geometrySymbol = getSymbol(geometry.geometryType());
      if (geometrySymbol)
      {
        // Create a new graphic based on the geometry and add it to the graphics overlay.
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
  // Stop the geometry editor and discard the geometry.
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
    m_editingGraphic = nullptr;
  }

  // Update the multifunction button text and enable it
  m_multifunctionButtonText = "Start geometry editor";
  m_multifunctionButtonEnabled = true;

  emit multifunctionButtonTextChanged();
  emit multifunctionButtonEnabledChanged();
  emit canUndoOrRedoChanged();
  emit selectedElementCanDeleteChanged();
  emit geometryEditorStartedChanged();
}

bool EditGeometriesWithProgrammaticReticleTool::vertexCreationAllowed() const
{
  return m_vertexCreationAllowed;
}

void EditGeometriesWithProgrammaticReticleTool::setVertexCreationAllowed(bool allowed)
{
  if (m_vertexCreationAllowed == allowed)
  {
    return;
  }

  m_vertexCreationAllowed = allowed;
  emit vertexCreationAllowedChanged();

  // Toggle the reticle tool's vertex creation preview and grow effect for mid-vertices
  if (m_reticleTool)
  {
    // Update the programmatic reticle tool and geometry editor settings based on the switch state.
    m_reticleTool->setVertexCreationPreviewEnabled(allowed);
    if (GeometryEditorStyle* style = m_reticleTool->style(); style)
    {
      if (GeometryEditorGrowEffect* growEffect = style->growEffect(); growEffect)
      {
        growEffect->setApplyToMidVertices(allowed);
      }
    }
  }

  // If the geometry editor is started, update the button text to reflect the new state.
  if (geometryEditorStarted())
  {
    setMultifunctionButtonState();
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
  return m_geometryEditor->selectedElement() && m_geometryEditor->selectedElement()->canDelete();
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

// State checks to control when buttons are enabled
bool EditGeometriesWithProgrammaticReticleTool::geometryEditorStarted() const
{
  return m_geometryEditor->isStarted();
}

bool EditGeometriesWithProgrammaticReticleTool::canUndo() const
{
  return (m_geometryEditor->canUndo() || m_geometryEditor->pickedUpElement());
}

bool EditGeometriesWithProgrammaticReticleTool::canRedo() const
{
  return m_geometryEditor->canRedo();
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
  emit selectedElementCanDeleteChanged();
}

void EditGeometriesWithProgrammaticReticleTool::redo()
{
  if (m_geometryEditor->canRedo())
  {
    m_geometryEditor->redo();
    emit canUndoOrRedoChanged();
    emit selectedElementCanDeleteChanged();
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
  // Update the multifunction button text and state based on the geometry editor state and hovered/picked up elements.
  QString newText;
  bool newEnabled = true;
  ReticleState newReticleState = ReticleState::Default;

  // Picked up elements can be dropped, so the button text indicates that a point can be dropped.
  if (m_geometryEditor->pickedUpElement())
  {
    newText = "Drop point";
    newReticleState = ReticleState::PickedUp;
  }
  // When vertex creation is allowed, the button text changes based on the hovered or picked up element. Vertices and mid-vertices can be picked up.
  else if (m_vertexCreationAllowed)
  {
    if (m_geometryEditor->pickedUpElement())
    {
      newText = "Drop point";
      newReticleState = ReticleState::PickedUp;
    }
    else if (GeometryEditorElement* hovered = dynamic_cast<GeometryEditorElement*>(m_geometryEditor->hoveredElement());
             hovered && (hovered->geometryEditorElementType() == GeometryEditorElementType::GeometryEditorVertex ||
                         hovered->geometryEditorElementType() == GeometryEditorElementType::GeometryEditorMidVertex))
    {
      newText = "Pick up point";
      newReticleState = (hovered->geometryEditorElementType() == GeometryEditorElementType::GeometryEditorVertex) ?
                          ReticleState::HoveringVertex : ReticleState::HoveringMidVertex;
    }
    else
    {
      newText = "Insert point";
      newReticleState = ReticleState::Default;
    }
  }
  // When vertex creation is not allowed, the button text changes based on the hovered element only.
  else
  {
    GeometryEditorElement* hovered = dynamic_cast<GeometryEditorElement*>(m_geometryEditor->hoveredElement());
    if (hovered && hovered->geometryEditorElementType() == GeometryEditorElementType::GeometryEditorVertex)
    {
      newText = "Pick up point";
      newReticleState = ReticleState::HoveringVertex;
    }
    else
    {
      newText = "Pick up point";
      newEnabled = false;
      newReticleState = ReticleState::HoveringMidVertex;
    }
  }

  // If the geometry editor is not started, override with "Start geometry editor"
  if (!m_geometryEditor->isStarted())
  {
    newText = "Start geometry editor";
    newEnabled = true;
    newReticleState = ReticleState::Default;
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
