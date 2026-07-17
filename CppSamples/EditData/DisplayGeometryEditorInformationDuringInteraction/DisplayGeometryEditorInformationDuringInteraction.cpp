// [WriteFile Name=DisplayGeometryEditorInformationDuringInteraction, Category=EditData]
// [Legal]
// Copyright 2026 Esri.
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
#include "DisplayGeometryEditorInformationDuringInteraction.h"

// ArcGIS Maps SDK headers
#include "Envelope.h"
#include "ErrorException.h"
#include "Geometry.h"
#include "GeometryEditor.h"
#include "GeometryEditorInteractionPreview.h"
#include "GeometryEditorTypes.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "ImmutablePart.h"
#include "ImmutablePartCollection.h"
#include "ImmutablePointCollection.h"
#include "InteractionConfiguration.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Multipoint.h"
#include "Point.h"
#include "Polygon.h"
#include "Polyline.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolTypes.h"
#include "VertexTool.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QtMath>

// STL headers
#include <cmath>
#include <memory>

using namespace Esri::ArcGISRuntime;

DisplayGeometryEditorInformationDuringInteraction::DisplayGeometryEditorInformationDuringInteraction(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this)),
  m_graphicsOverlay(new GraphicsOverlay(this)),
  m_geometryEditor(new GeometryEditor(this))
{
  m_map->setInitialViewpoint(Viewpoint::fromJson(
    R"({"rotation":0.0,"scale":17000,"targetGeometry":{"spatialReference":{"wkid":3857},"x":-13045202.018086127,"y":4035612.571361517}})"));

  VertexTool* vertexTool = new VertexTool(this);
  InteractionConfiguration* configuration = new InteractionConfiguration(this);
  configuration->setAllowVertexCreation(false);
  configuration->setAllowMidVertexSelection(false);
  configuration->setAllowDeletingSelectedElement(false);
  configuration->setAllowVertexSelection(false);
  configuration->setAllowPartCreation(false);
  vertexTool->setConfiguration(configuration);
  m_geometryEditor->setTool(vertexTool);

  createInitialGraphics();

  connect(m_geometryEditor, &GeometryEditor::isStartedChanged, this,
          &DisplayGeometryEditorInformationDuringInteraction::geometryEditorStartedChanged);
  connect(m_geometryEditor, &GeometryEditor::canUndoChanged, this, &DisplayGeometryEditorInformationDuringInteraction::canUndoChanged);
  connect(m_geometryEditor, &GeometryEditor::canRedoChanged, this, &DisplayGeometryEditorInformationDuringInteraction::canRedoChanged);
  connect(m_geometryEditor, &GeometryEditor::interactionPreviewChanged, this,
          &DisplayGeometryEditorInformationDuringInteraction::onInteractionPreviewChanged);
}

DisplayGeometryEditorInformationDuringInteraction::~DisplayGeometryEditorInformationDuringInteraction() = default;

void DisplayGeometryEditorInformationDuringInteraction::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayGeometryEditorInformationDuringInteraction>("Esri.Samples", 1, 0, "DisplayGeometryEditorInformationDuringInteractionSample");
}

void DisplayGeometryEditorInformationDuringInteraction::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  m_mapView->setGeometryEditor(m_geometryEditor);

  connect(m_mapView, &MapQuickView::mouseClicked, this, &DisplayGeometryEditorInformationDuringInteraction::onMapClicked);
  emit mapViewChanged();
}

MapQuickView* DisplayGeometryEditorInformationDuringInteraction::mapView() const
{
  return m_mapView;
}

void DisplayGeometryEditorInformationDuringInteraction::createInitialGraphics()
{
  const Polygon polygon = geometry_cast<Polygon>(Geometry::fromJson(
    R"({"rings":[[[-13046991.222211758,4034618.5047884779],[-13046991.222211758,4035962.0723415823],[-13045677.652220398,4035962.0723415823],[-13045677.652220398,4034618.5047884779],[-13046991.222211758,4034618.5047884779]]],"spatialReference":{"wkid":3857}})"));
  const Polyline polyline = geometry_cast<Polyline>(Geometry::fromJson(
    R"({"paths":[[[-13044533.805088846,4034221.5100018946],[-13043597.938505623,4034197.1337576872],[-13043597.938505623,4035135.572073034],[-13044522.634505576,4035170.5449295067]]],"spatialReference":{"wkid":3857}})"));
  const Multipoint multipoint = geometry_cast<Multipoint>(Geometry::fromJson(
    R"({"points":[[-13045283.292102993,4035739.1925106063],[-13045314.922186911,4036533.8852012255],[-13044798.24723932,4036138.7808295386],[-13044354.514637273,4035719.3623426706],[-13044281.57229173,4036473.0999132735]],"spatialReference":{"wkid":3857}})"));

  SimpleLineSymbol* redOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(255, 0, 0), 2, this);
  SimpleFillSymbol* polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75), redOutline, this);
  SimpleMarkerSymbol* pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(0, 0, 255), 8, this);

  m_graphicsOverlay->graphics()->append({
    new Graphic(multipoint, pointSymbol, this),
    new Graphic(polygon, polygonSymbol, this),
    new Graphic(polyline, redOutline, this),
  });
}

void DisplayGeometryEditorInformationDuringInteraction::onMapClicked(const QMouseEvent& mouseEvent)
{
  if (m_geometryEditor->isStarted())
  {
    return;
  }

  m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10, false, this)
    .then(this,
          [this](IdentifyGraphicsOverlayResult* rawResult)
  {
    const std::unique_ptr<IdentifyGraphicsOverlayResult> result(rawResult);
    if (!result || result->graphics().isEmpty())
    {
      return;
    }

    m_editingGraphic = result->graphics().first();
    m_geometryEditor->start(m_editingGraphic->geometry());
    m_geometryEditor->selectGeometry();
    m_editingGraphic->setVisible(false);
  })
    .onFailed(this, [](const ErrorException& error)
  {
    qDebug() << "Identify failed:" << error.error().message();
  });
}

void DisplayGeometryEditorInformationDuringInteraction::onInteractionPreviewChanged(GeometryEditorInteractionPreview* interactionPreview)
{
  if (!interactionPreview)
  {
    clearInteractionInformation();
    return;
  }

  // Clean these up, we don't need them after we've processed the update.
  // They won't leak, but they're owned by the GeometryEditor itself.
  interactionPreview->deleteLater();

  switch (interactionPreview->interactionType())
  {
    case GeometryEditorInteractionType::Scale:
      setScaleInformation(interactionPreview->previewGeometry());
      break;
    case GeometryEditorInteractionType::Rotate:
      setRotationInformation(interactionPreview->previewGeometry());
      break;
    case GeometryEditorInteractionType::Move:
      setMoveInformation(interactionPreview->previewGeometry());
      break;
    case GeometryEditorInteractionType::Create:
      clearInteractionInformation();
      return;
  }

  m_interactionInfoVisible = true;
  emit interactionInfoChanged();
}

void DisplayGeometryEditorInformationDuringInteraction::setScaleInformation(const Geometry& previewGeometry)
{
  const Envelope originalExtent = m_geometryEditor->geometry().extent();
  const Envelope previewExtent = previewGeometry.extent();
  if (qFuzzyIsNull(originalExtent.width()) || qFuzzyIsNull(originalExtent.height()))
  {
    clearInteractionInformation();
    return;
  }

  const double scaleX = previewExtent.width() / originalExtent.width();
  const double scaleY = previewExtent.height() / originalExtent.height();
  m_interactionDescription = tr("Scale factor (X, Y):");
  m_interactionValue = QString("(%1, %2)").arg(QString::number(scaleX, 'f', 2), QString::number(scaleY, 'f', 2));
}

void DisplayGeometryEditorInformationDuringInteraction::setRotationInformation(const Geometry& previewGeometry)
{
  const Point center = m_geometryEditor->geometry().extent().center();
  const Point originalPoint = firstPoint(m_geometryEditor->geometry());
  const Point previewPoint = firstPoint(previewGeometry);
  if (originalPoint.isEmpty() || previewPoint.isEmpty())
  {
    clearInteractionInformation();
    return;
  }

  const double vector1X = originalPoint.x() - center.x();
  const double vector1Y = originalPoint.y() - center.y();
  const double vector2X = previewPoint.x() - center.x();
  const double vector2Y = previewPoint.y() - center.y();
  const double crossProduct = (vector1X * vector2Y) - (vector1Y * vector2X);
  const double dotProduct = (vector1X * vector2X) + (vector1Y * vector2Y);
  const double angle = qRadiansToDegrees(std::atan2(crossProduct, dotProduct));
  const double clockwiseAngle = std::fmod(-angle + 360.0, 360.0);

  m_interactionDescription = tr("Rotation angle (degrees):");
  m_interactionValue = QString::number(clockwiseAngle, 'f', 2);
}

void DisplayGeometryEditorInformationDuringInteraction::setMoveInformation(const Geometry& previewGeometry)
{
  const Point center = previewGeometry.extent().center();
  m_interactionDescription = tr("Center (X, Y):");
  m_interactionValue = QString("(%1, %2)").arg(QString::number(center.x(), 'f', 2), QString::number(center.y(), 'f', 2));
}

Point DisplayGeometryEditorInformationDuringInteraction::firstPoint(const Geometry& geometry)
{
  switch (geometry.geometryType())
  {
    case GeometryType::Polygon:
    {
      const Polygon polygon = geometry_cast<Polygon>(geometry);
      return polygon.parts().isEmpty() ? Point() : polygon.parts().part(0).startPoint();
    }
    case GeometryType::Polyline:
    {
      const Polyline polyline = geometry_cast<Polyline>(geometry);
      return polyline.parts().isEmpty() ? Point() : polyline.parts().part(0).startPoint();
    }
    case GeometryType::Multipoint:
    {
      const Multipoint multipoint = geometry_cast<Multipoint>(geometry);
      return multipoint.points().isEmpty() ? Point() : multipoint.points().point(0);
    }
    default:
      return {};
  }
}

void DisplayGeometryEditorInformationDuringInteraction::undo()
{
  m_geometryEditor->undo();
}

void DisplayGeometryEditorInformationDuringInteraction::redo()
{
  m_geometryEditor->redo();
}

void DisplayGeometryEditorInformationDuringInteraction::cancelEdits()
{
  finishEditing(false);
}

void DisplayGeometryEditorInformationDuringInteraction::saveEdits()
{
  finishEditing(true);
}

void DisplayGeometryEditorInformationDuringInteraction::finishEditing(bool save)
{
  if (!m_geometryEditor->isStarted())
  {
    return;
  }

  const Geometry result = m_geometryEditor->stop();
  if (save && m_editingGraphic && !result.isEmpty())
  {
    m_editingGraphic->setGeometry(result);
  }

  if (m_editingGraphic)
  {
    m_editingGraphic->setVisible(true);
    m_editingGraphic = nullptr;
  }
  clearInteractionInformation();
}

void DisplayGeometryEditorInformationDuringInteraction::clearInteractionInformation()
{
  if (!m_interactionInfoVisible && m_interactionDescription.isEmpty() && m_interactionValue.isEmpty())
  {
    return;
  }

  m_interactionInfoVisible = false;
  m_interactionDescription.clear();
  m_interactionValue.clear();
  emit interactionInfoChanged();
}

bool DisplayGeometryEditorInformationDuringInteraction::geometryEditorStarted() const
{
  return m_geometryEditor->isStarted();
}

bool DisplayGeometryEditorInformationDuringInteraction::canUndo() const
{
  return m_geometryEditor->canUndo();
}

bool DisplayGeometryEditorInformationDuringInteraction::canRedo() const
{
  return m_geometryEditor->canRedo();
}
