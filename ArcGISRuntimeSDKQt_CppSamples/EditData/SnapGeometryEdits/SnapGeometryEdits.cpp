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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "SnapGeometryEdits.h"

#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "Geometry.h"
#include "GeometryEditor.h"
#include "GeometryEditorElement.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "Layer.h"
#include "LayerListModel.h"
#include "LoadSettings.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Portal.h"
#include "PortalItem.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SnapGeometryEdits.h"
#include "SnapSettings.h"
#include "SnapSourceSettings.h"
#include "SymbolTypes.h"

#include <QFuture>

using namespace Esri::ArcGISRuntime;

SnapGeometryEdits::SnapGeometryEdits(QObject* parent /* = nullptr */) :
  QObject(parent)
{
  Portal* portal = new Portal(this);
  PortalItem* portalItem = new PortalItem(portal, "b95fe18073bc4f7788f0375af2bb445e", this);
  m_map = new Map(portalItem, this);

  m_geometryEditor = new GeometryEditor(this);
  m_graphicsOverlay = new GraphicsOverlay(this);
  connect (m_map, &Map::doneLoading, this, [this]()
  {
    for (Layer* layer : *m_map->operationalLayers())
    {
      connect (layer, &Layer::doneLoading, this, [this]()
      {
        // Enable snap settings after layers load
        if (m_layersLoaded == false)
        {
          m_layersLoaded = true;
          emit layersLoadedChanged();
        }
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

  m_map->loadSettings()->setFeatureTilingMode(FeatureTilingMode::EnabledWithFullResolutionWhenSupported);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Set the geometry editor on the map view
  m_mapView->setGeometryEditor(m_geometryEditor);
  m_mapView->setMagnifierEnabled(true);

  emit mapViewChanged();
  createInitialSymbols();
  createConnections();
}

// Create symbols used by all graphics
void SnapGeometryEdits::createInitialSymbols()
{
  m_pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 10, this);
  m_multiPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 45, 0), 10, this);
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(255, 45, 0), 2, this);
  m_polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(255, 0, 0, 75),
                      new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 0), 1.0, this), this);
}

void SnapGeometryEdits::createConnections()
{
  // Allow user to edit existing graphics by clicking on them
  connect (m_mapView, &MapQuickView::mouseClicked, this, [this](const QMouseEvent& mouseEvent)
  {
    if (!m_geometryEditor->isStarted())
    {
      m_mapView->identifyGraphicsOverlayAsync(m_graphicsOverlay, mouseEvent.position(), 10 ,false).then(this, [this](IdentifyGraphicsOverlayResult* result)
      {
        // Handle editing selected graphics, if any
        auto identifyResult = std::unique_ptr<IdentifyGraphicsOverlayResult>(result);
        // Return if no graphics were identified
        if (identifyResult->graphics().isEmpty())
          return;

        m_editingGraphic = identifyResult->graphics().first();

        // Hide the graphic currently being edited
        m_editingGraphic->setVisible(false);

        // Start the geometry editor with the graphic's geometry
        m_geometryEditor->start(m_editingGraphic->geometry());

        emit geometryEditorStartedChanged();
      });
    }
    emit canUndoChanged();
    emit isElementSelectedChanged();
  });

  // Enable or disable buttons when mouse is released (ie after a drag operation)
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](const QMouseEvent&)
  {
    emit canUndoChanged();
    emit isElementSelectedChanged();
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

bool SnapGeometryEdits::isElementSelected()
{
  return (m_geometryEditor && m_geometryEditor->selectedElement() && m_geometryEditor->selectedElement()->canDelete());
}

// Toggles snapping using the enabled state from the snap settings
void SnapGeometryEdits::snappingEnabledStatus(bool snappingCheckedState)
{
  m_geometryEditor->snapSettings()->setEnabled(snappingCheckedState);
}

// Toggles snapping for the point layer snap source at [index] using the checked value from the snap settings
void SnapGeometryEdits::pointSourceEnabledStatus(bool snappingCheckedState, int index)
{
  m_pointSourceCheckedState[index] = snappingCheckedState;
  m_pointSourceList[index]->setEnabled(m_pointSourceCheckedState[index]);
}

// Toggles snapping for the polyline layer snap source at [index] using the checked value from the snap settings
void SnapGeometryEdits::polylineSourceEnabledStatus(bool snappingCheckedState, int index)
{
  m_polylineSourceCheckedState[index] = snappingCheckedState;
  m_polylineSourceList[index]->setEnabled(m_polylineSourceCheckedState[index]);
}

// Enable snapping for all the point layer snap sources
void SnapGeometryEdits::onPointLayersEnabled()
{
  for (int index = 0; index < m_pointSourceList.size(); ++index)
  {
    pointSourceEnabledStatus(true, index);
  }
  emit pointSourceCheckedStateChanged();
}

// Enable snapping for all the polyline layer snap sources
void SnapGeometryEdits::onPolylineLayersEnabled()
{
  for (int index = 0; index < m_polylineSourceList.size(); ++index)
  {
    polylineSourceEnabledStatus(true, index);
  }
  emit polylineSourceCheckedStateChanged();
}

// Starts the GeometryEditor using the selected geometry type
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

Symbol* SnapGeometryEdits::determineGeometrySymbol(const Geometry& geometry)
{
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
    break;
  }
  return geometrySymbol;
}

// Stops the GeometryEditor and append the new graphic to the graphics overlay
void SnapGeometryEdits::stopEditing()
{
  const Geometry geometry = m_geometryEditor->stop();
  emit geometryEditorStartedChanged();

  if (m_editingGraphic)
  {
    m_editingGraphic->setGeometry(geometry);

    m_editingGraphic->setVisible(true);
    m_editingGraphic = nullptr;

    return;
  }

  Symbol* geometrySymbol = determineGeometrySymbol(geometry);
  if (geometrySymbol)
  {
    m_graphicsOverlay->graphics()->append(new Graphic(geometry, geometrySymbol, this));
  }
}

// Deletes the selected element
void SnapGeometryEdits::deleteSelection()
{
  m_geometryEditor->deleteSelectedElement();
  emit canUndoChanged();
}

// Reverts the last event on the geometry editor
void SnapGeometryEdits::editorUndo()
{
  m_geometryEditor->undo();
  emit canUndoChanged();
}

void SnapGeometryEdits::displaySnapSources()
{
  // create lists for displaying the snap sources in snap settings
  QList<SnapSourceSettings*> snapSourceList;
  if (m_geometryEditor->snapSettings()->sourceSettings().empty())
  {
    m_geometryEditor->snapSettings()->syncSourceSettings();
    snapSourceList = m_geometryEditor->snapSettings()->sourceSettings();

    for (int index = 0; index < snapSourceList.size()-1; ++index)
    {
      FeatureLayer *featureLayer = static_cast<FeatureLayer*>(snapSourceList[index]->source());
      if (featureLayer->featureTable()->geometryType() == GeometryType::Point)
      {
        m_pointLayers.append(featureLayer->name());
        m_pointSourceCheckedState.append(snapSourceList[index]->isEnabled());
        m_pointSourceList.append(snapSourceList[index]);
      }
      else if (featureLayer->featureTable()->geometryType() == GeometryType::Polyline)
      {
        m_polylineLayers.append(featureLayer->name());
        m_polylineSourceCheckedState.append(snapSourceList[index]->isEnabled());
        m_polylineSourceList.append(snapSourceList[index]);
      }
    }

  emit pointLayersChanged();
  emit polylineLayersChanged();
  }
}
