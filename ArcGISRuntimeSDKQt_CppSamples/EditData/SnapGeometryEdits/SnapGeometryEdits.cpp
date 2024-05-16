// [WriteFile Name=SnapGeometryEdits, Category=EditData]
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

#include "Geometry.h"
#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "SnapGeometryEdits.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Portal.h"
#include "PortalItem.h"
#include "SnapGeometryEdits.h"
#include "LoadSettings.h"
#include "LayerListModel.h"
#include "Layer.h"
#include "GeometryEditor.h"
#include "GraphicsOverlay.h"
#include "GeometryEditor.h"
#include "GeometryTypes.h"
#include "GraphicsOverlayListModel.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SymbolTypes.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "Graphic.h"
#include "SnapSettings.h"
#include "SnapSourceSettings.h"
#include "GraphicListModel.h"
#include "GeometryEditorElement.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

SnapGeometryEdits::SnapGeometryEdits(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_portal(new Portal(this)),
  m_portalItem(new PortalItem(m_portal, "b95fe18073bc4f7788f0375af2bb445e", this))
{
  m_map = new Map(m_portalItem, this);
  m_map->loadSettings()->setFeatureTilingMode(FeatureTilingMode::EnabledWhenSupported);
  m_geometryEditor = new GeometryEditor(this);
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_tempGraphicsParent = new QObject(this);
  connect(m_map, &Map::doneLoading, this, [this]()
          {
            for (Layer* layer : *m_map->operationalLayers())
            {
              connect(layer, &Layer::doneLoading, this, [layer]()
                      {
                qDebug() << "done" << layer->name();
              });
            }
          });
  }




SnapGeometryEdits::~SnapGeometryEdits() = default;

void SnapGeometryEdits::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SnapGeometryEdits>("Esri.Samples", 1, 0, "SnapGeometryEditsSample");
}

MapQuickView* SnapGeometryEdits::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void SnapGeometryEdits::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);

  m_mapView->setMagnifierEnabled(true);


  emit mapViewChanged();
  createInitialSymbols();
  createConnections();

  // showBottomSheet();
}

// Create symbols used by all graphics
void SnapGeometryEdits::createInitialSymbols()
{
  m_pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 20, this);
  m_multiPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 20, this);
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(255, 45, 0), 2, this);
  m_polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75),
  new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 0), 1.0, this), this);
}

void SnapGeometryEdits::createConnections()
{
  qDebug() << "1";

  // Allow user to edit existing graphics by clicking on them
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& mouseEvent)
          {qDebug() << "2";
            if (!m_geometryEditor->isStarted())
            {qDebug() << "3";
              m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10 ,false).then(this,
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
            emit canUndoChanged();
            emit elementIsSelectedChanged();
          });

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](const QMouseEvent&)
          {
            emit canUndoChanged();
            emit elementIsSelectedChanged();
          });
}

bool SnapGeometryEdits::geometryEditorStarted() const
{
  return (m_geometryEditor && m_geometryEditor->isStarted());
}

bool SnapGeometryEdits::canUndo()
{
  return (m_geometryEditor && m_geometryEditor->canUndo());
}

bool SnapGeometryEdits::elementIsSelected()
{
  return (m_geometryEditor && m_geometryEditor->selectedElement() && m_geometryEditor->selectedElement()->canDelete());
}

void SnapGeometryEdits::snappingEnabledStatus(bool snappingCheckedState)
{
  m_geometryEditor->snapSettings()->setEnabled(snappingCheckedState);
}

void SnapGeometryEdits::pointSourceEnabledStatus(bool snappingCheckedState, int index)
{
  m_pointSourceCheckedState[index] = snappingCheckedState;
  m_geometryEditor->snapSettings()->sourceSettings()[index]->setEnabled(m_pointSourceCheckedState[index]);

}

void SnapGeometryEdits::polylineSourceEnabledStatus(bool snappingCheckedState, int index)
{
  m_polylineSourceCheckedState[index] = snappingCheckedState;
  m_geometryEditor->snapSettings()->sourceSettings()[index]->setEnabled(m_polylineSourceCheckedState[index]);

}

void SnapGeometryEdits::onPointLayersEnabled()
{
  for (int index = 0; index < m_snapSourceList.size()-1; ++index)
  {
    FeatureLayer *featureLayer = static_cast<FeatureLayer*>(m_snapSourceList[index]->source());
    if (featureLayer && featureLayer->featureTable() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
    {
      pointSourceEnabledStatus(true, index);
    }
  }
    emit pointSourceCheckedStateChanged();
}

void SnapGeometryEdits::onPolylineLayersEnabled()
{
  for (int index = 0; index < m_snapSourceList.size()-1; ++index)
  {
    FeatureLayer *featureLayer = static_cast<FeatureLayer*>(m_snapSourceList[index]->source());
    if (featureLayer && featureLayer->featureTable() && featureLayer->featureTable()->geometryType() == GeometryType::Polyline)
    {
      polylineSourceEnabledStatus(true, index);
    }
  }
  emit polylineSourceCheckedStateChanged();
}

void SnapGeometryEdits::startEditor(GeometryEditorMode geometryEditorMode)
{
    switch (geometryEditorMode)
    {
    case GeometryEditorMode::PointMode:
      m_geometryEditor->start(Esri::ArcGISRuntime::GeometryType::Point);
      break;

    case GeometryEditorMode::MultipointMode:
      m_geometryEditor->start(Esri::ArcGISRuntime::GeometryType::Multipoint);
      break;

    case GeometryEditorMode::PolylineMode:
      m_geometryEditor->start(Esri::ArcGISRuntime::GeometryType::Polyline);
      break;

    case GeometryEditorMode::PolygonMode:
      m_geometryEditor->start(Esri::ArcGISRuntime::GeometryType::Polygon);
      break;

    default:
      break;
    }

    emit geometryEditorStartedChanged();
}

void SnapGeometryEdits::stopEditor()
{
  const Geometry geometry = m_geometryEditor->stop();
  emit geometryEditorStartedChanged();

  if (m_editingGraphic)
  {
    m_editingGraphic->setGeometry(geometry);

    // Show the graphic and clear the selection
    m_editingGraphic->setVisible(true);
    m_editingGraphic = nullptr;

    return;
  }

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

void SnapGeometryEdits::deleteSelection()
{
  m_geometryEditor->deleteSelectedElement();
  emit canUndoChanged();
}

void SnapGeometryEdits::editorUndo()
{
  // if (m_geometryEditor->canUndo())
    m_geometryEditor->undo();
  emit canUndoChanged();
}

void SnapGeometryEdits::configureSnapping()
{

  if (m_geometryEditor->snapSettings()->sourceSettings().empty())
  {

    m_geometryEditor->snapSettings()->syncSourceSettings();
    m_geometryEditor->snapSettings()->setEnabled(true);

    m_snapSourceList = m_geometryEditor->snapSettings()->sourceSettings();
    qDebug() << m_snapSourceList.size();
    qDebug() << static_cast<FeatureLayer*>(m_snapSourceList[0]->source())->name();
    qDebug() << static_cast<FeatureLayer*>(m_snapSourceList[1]->source())->name();
    qDebug() << static_cast<FeatureLayer*>(m_snapSourceList[2]->source())->name();
    qDebug() << static_cast<FeatureLayer*>(m_snapSourceList[3]->source())->name();
  }

  m_pointLayers.clear();
  m_polylineLayers.clear();
  for (int index = 0; index < m_snapSourceList.size()-1; ++index)
  {
    FeatureLayer *featureLayer = static_cast<FeatureLayer*>(m_snapSourceList[index]->source());
    if (featureLayer->featureTable()->geometryType() == GeometryType::Point)
    {
      m_pointLayers.append(featureLayer->name());
      m_pointSourceCheckedState.append(m_snapSourceList[index]->isEnabled());
    }
    else if (featureLayer->featureTable()->geometryType() == GeometryType::Polyline)
    {
      m_polylineLayers.append(featureLayer->name());
      m_polylineSourceCheckedState.append(m_snapSourceList[index]->isEnabled());
    }
  }

  emit pointLayersChanged();
  emit polylineLayersChanged();
}
