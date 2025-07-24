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

  m_mapView->setViewpointCenterAsync(Point(-0.775395, 51.523806, SpatialReference::wgs84()), 50000);
  //m_mapView->setViewpointCenterAsync(Point(51.523806, -0.775395), 20000); //.Net version

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);

  m_geometryEditor->setTool(m_reticleTool);

  emit mapViewChanged();

  createInitialGraphics();
  createConnections();
}

void EditGeometriesWithProgrammaticReticleTool::createInitialGraphics()
{
  const QString pinkneysGreenJson = R"({"rings":[[[-84843.262719916485,6713749.9329888355],[-85833.376589175183,6714679.7122141244], [-85406.822347959576,6715063.9827222107],[-85184.329997390232,6715219.6195847588], [-85092.653857582554,6715119.5391713539],[-85090.446872787768,6714792.7656492386], [-84915.369168906298,6714297.8798246197],[-84854.295522911285,6714080.907587287], [-84843.262719916485,6713749.9329888355]]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";
  const QString beechLodgeBoundaryJson = R"({"paths":[[[-87090.652708065536,6714158.9244240439],[-87247.362370337316,6714232.880689906], [-87226.314032974493,6714605.4697726099],[-86910.499335316243,6714488.006312645], [-86750.82198052686,6714401.1768307304],[-86749.846825938366,6714305.8450344801]]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";
  const QString treeMarkersJson = R"({"points":[[-86750.751150056443,6713749.4529355941],[-86879.381793060631,6713437.3335486846], [-87596.503104619667,6714381.7342108283],[-87553.257569537804,6714402.0910389507], [-86831.019903597829,6714398.4128562529],[-86854.105933315877,6714396.1957954112], [-86800.624094892439,6713992.3374453448]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";

  const auto pinkneysGreen = geometry_cast<Polygon>(Polygon::fromJson(pinkneysGreenJson.toUtf8()));
  const auto beechLodgeBoundary = geometry_cast<Polyline>(Polyline::fromJson(beechLodgeBoundaryJson.toUtf8()));
  const auto treeMarkers = geometry_cast<Multipoint>(Multipoint::fromJson(treeMarkersJson.toUtf8()));

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
  // If the geometry editor is started, identify the geometry editor result at the tapped position.
  if (m_geometryEditor->isStarted())
  {
    // Identify the geometry editor result at the tapped position.
    m_mapView->identifyGeometryEditorAsync(mouseEvent.position(), 10).then(this, [this](IdentifyGeometryEditorResult* result)
    {
      std::unique_ptr<IdentifyGeometryEditorResult> identifyResult(result);
      if (identifyResult && !identifyResult->elements().isEmpty())
      {
        // Get the first element from the result.
        auto* element = identifyResult->elements().first();

        // If the element is a vertex or mid-vertex, set the viewpoint to its position and select it.
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
    }).onFailed(this, [this](const ErrorException&)
    {
      resetFromEditingSession();
    });
  }
  else
  {
    // Identify graphics in the graphics overlay using the tapped position.
    m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10, false)
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
        const auto geometry = m_editingGraphic->geometry();
        Point targetPoint;
        switch (geometry.geometryType())
        {
          case GeometryType::Polygon:
          {
            if (const auto polygon = geometry_cast<Polygon>(geometry); !polygon.parts().isEmpty())
            {
              targetPoint = polygon.parts().part(0).endPoint();
            }
            break;
          }
          case GeometryType::Polyline:
          {
            if (const auto polyline = geometry_cast<Polyline>(geometry); !polyline.parts().isEmpty())
            {
              targetPoint = polyline.parts().part(0).endPoint();
            }
            break;
          }
          case GeometryType::Multipoint:
          {
            if (const auto multiPoint = geometry_cast<Multipoint>(geometry);!multiPoint.points().isEmpty())
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
      }
    }).onFailed(this, [this](const ErrorException& error)
    {
      qDebug() << "Error editing! Identify failed with error " << error.error().message();
      resetFromEditingSession();
    });

    // Hide the selected graphic while editing.
    m_editingGraphic->setVisible(false);
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
  emit geometryEditorStartedChanged();
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
