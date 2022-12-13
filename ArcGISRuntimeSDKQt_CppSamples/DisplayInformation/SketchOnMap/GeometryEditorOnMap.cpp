// [WriteFile Name=GeometryEditorOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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

#include "Envelope.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "GeoElement.h"
#include "GeometryEditor.h"
#include "GeometryEditorConfiguration.h"
#include "GeometryEditorElement.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "GeometryEditorOnMap.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TaskWatcher.h"

#include "QUuid"

using namespace Esri::ArcGISRuntime;

GeometryEditorOnMap::GeometryEditorOnMap(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISImagery, this))
{
  // Create a GraphicsOverlay to save the geometries to
  m_graphicsOverlay = new GraphicsOverlay(this);
  createSymbols();
}

GeometryEditorOnMap::~GeometryEditorOnMap() = default;

void GeometryEditorOnMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>( "Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GeometryEditorOnMap>("Esri.Samples", 1, 0, "GeometryEditorSample");
}

MapQuickView* GeometryEditorOnMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void GeometryEditorOnMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setViewpointCenter(Point(-102.42, 32.05, SpatialReference::wgs84()), 100'000);

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  m_geometryEditor = new GeometryEditor(this);
  m_mapView->setGeometryEditor(m_geometryEditor);

  m_configuration = m_geometryEditor->editConfiguration();

  connectIdentify();
  connectSignals();
  emit mapViewChanged();

  QList<QUrl> urls = {QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/HSEC/FeatureServer/0"),
                      QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/HSEC/FeatureServer/1"),
                      QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/HSEC/FeatureServer/2")};

  foreach (QUrl url, urls)
  {
    ServiceFeatureTable* featureTable = new ServiceFeatureTable(url, this);
    auto featureLayer = new FeatureLayer(featureTable, this);
    m_map->operationalLayers()->append(featureLayer);
    m_featureLayers.append(featureLayer);
  }
}

void GeometryEditorOnMap::setCreationMode(SampleCreationMode creationMode)
{
  switch (creationMode)
  {
  case SampleCreationMode::PointMode:
    m_geometryEditor->start(GeometryEditorCreationMode::Point, m_configuration);
    break;

  case SampleCreationMode::MultipointMode:
    m_geometryEditor->start(GeometryEditorCreationMode::Multipoint, m_configuration);
    break;

  case SampleCreationMode::PolylineMode:
    m_geometryEditor->start(GeometryEditorCreationMode::Polyline, m_configuration);
    break;

  case SampleCreationMode::PolygonMode:
    m_geometryEditor->start(GeometryEditorCreationMode::Polygon, m_configuration);
    break;

  default:
    break;
  }
  emit geometryEditorStartedChanged();
}

void GeometryEditorOnMap::startGeometry()
{
  if (m_graphic)
    m_geometryEditor->start(m_graphic->geometry(), m_configuration);
  else if (m_feature)
    m_geometryEditor->start(m_feature->geometry(), m_configuration);

  deselectGraphics();

  //TODO don't emit these when event is hooked up https://devtopia.esri.com/runtime/bistromath/issues/1557
  emit geometryEditorStartedChanged();
}

void GeometryEditorOnMap::stopGeometryEditor(bool saveGeometry)
{
  if (!m_geometryEditor->isStarted() || !saveGeometry)
  {
    m_geometryEditor->stop();
    emit geometryEditorStartedChanged();
    return;
  }

  // save the geometry
  Symbol* geometrySymbol = nullptr;

  switch (m_geometryEditor->creationMode())
  {
  case GeometryEditorCreationMode::Point:
    geometrySymbol = m_pointSymbol;
    break;

  case GeometryEditorCreationMode::Multipoint:
    geometrySymbol = m_multiPointSymbol;
    break;

  case GeometryEditorCreationMode::Polyline:
    geometrySymbol = m_lineSymbol;
    break;

  case GeometryEditorCreationMode::Polygon:
    geometrySymbol = m_polygonSymbol;
    break;

  default:
    return;
  }

  Geometry editedGeometry = m_geometryEditor->stop();
  Graphic* graphic = new Graphic(editedGeometry, geometrySymbol, this);
  m_graphicsOverlay->graphics()->append(graphic);

  emit geometryEditorStartedChanged();
}

bool GeometryEditorOnMap::geometryEditorStarted() const
{
  if (m_geometryEditor)
    return m_geometryEditor->isStarted();

  return false;
}

bool GeometryEditorOnMap::canUndo() const
{
  if (m_geometryEditor)
    return m_geometryEditor->canUndo();

  return false;
}

bool GeometryEditorOnMap::canRedo() const
{
  if (m_geometryEditor)
    return m_geometryEditor->canRedo();

  return false;
}

bool GeometryEditorOnMap::canDeleteSelection() const
{
  if (m_geometryEditor && m_geometryEditor->selection())
    return m_geometryEditor->selection()->canDelete();

  return false;
}

bool GeometryEditorOnMap::isGeoElementSelected() const
{
  if (m_feature || m_graphic)
    return true;

  return false;
}

void GeometryEditorOnMap::createSymbols()
{
  m_pointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(255, 0, 0), 10, this);
  m_multiPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square, QColor(0, 0, 255), 10, this);
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(144, 238, 144), 3, this);
  m_polygonSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(67, 166, 198, 119),
                                         new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(67, 166, 198), 2.0, this), this);
}

void GeometryEditorOnMap::deleteSelection()
{
  if (m_geometryEditor->geometry().extent().isValid())
    m_geometryEditor->deleteSelection();
}

void GeometryEditorOnMap::clearGraphics()
{
  m_geometryEditor->clearGeometry();
  deselectGraphics();
  m_graphicsOverlay->graphics()->clear();
}

void GeometryEditorOnMap::undo()
{
  m_geometryEditor->undo();
}

void GeometryEditorOnMap::redo()
{
  m_geometryEditor->redo();
}

void GeometryEditorOnMap::connectSignals()
{
  connect(m_geometryEditor, &GeometryEditor::canUndoChanged, this, [this]()
          {
            emit canUndoChanged();
          });

  connect(m_geometryEditor, &GeometryEditor::canRedoChanged, this, [this]()
          {
            emit canRedoChanged();
          });

  connect(m_geometryEditor, &GeometryEditor::selectionChanged, this, [this]()
          {
            emit canDeleteSelectionChanged();
          });

  connect(m_geometryEditor, &GeometryEditor::startedChanged, this, [this]()
          {
            emit geometryEditorStartedChanged(); // TODO this event isn't hooked up https://devtopia.esri.com/runtime/bistromath/issues/1557
          });
}

void GeometryEditorOnMap::connectIdentify()
{
  // GY modified ! [identify feature layer qml api snippet]
  // lambda expression for the mouse press event on the mapview... do an identify operation
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
          {
            if (m_geometryEditor->isStarted())
              return;

            constexpr double tolerance = 10.0;
            constexpr bool returnPopupsOnly = false;
            constexpr int maximumResults = 1;
            const double screenX = mouseEvent.pos().x();
            const double screenY = mouseEvent.pos().y();
            deselectGraphics();
            m_mapView->identifyLayers(screenX, screenY, tolerance, returnPopupsOnly, maximumResults);
            m_mapView->identifyGraphicsOverlay(m_graphicsOverlay, screenX, screenY, tolerance, returnPopupsOnly, maximumResults);
          });

  // once the FeatureLayers identify is done
  connect(m_mapView, &MapQuickView::identifyLayersCompleted, this, [this](QUuid, QList<Esri::ArcGISRuntime::IdentifyLayerResult*> rawIdentifyResult)
          {
            auto identifyResults = rawIdentifyResult;

            if (identifyResults.isEmpty())
              return;

            if (m_graphic)
              return;

            for (int j = 0; j < identifyResults.size(); j++)
            {
              auto identifyResult = identifyResults.at(j);
              for (int i = 0; i < identifyResult->geoElements().size(); i++)
              {
                GeoElement* element = identifyResult->geoElements().at(i);
                if (nullptr != element)
                {
                  // add the element to the list and take ownership of it.
                  m_feature = static_cast<Feature*>(element);
                  m_feature->setParent(this);
                  break;
                }
              }
            }

            if (m_feature)
            {
              // select the identified feature
              foreach (Esri::ArcGISRuntime::FeatureLayer* featureLayer, m_featureLayers)
              {
                featureLayer->selectFeature(m_feature);
              }
              emit isGeoElementSelectedChanged();
            }
          });

  // when the GraphicsOverlay identify is done
  connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, [this](QUuid, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* rawIdentifyResult)
          {
            auto identifyResult = rawIdentifyResult;

            if (!identifyResult || identifyResult->graphics().isEmpty())
              return;

            for (int i = 0; i < identifyResult->graphics().size(); i++)
            {
              // add the element to the list and take ownership of it.
              m_graphic = identifyResult->graphics().at(i);
              m_graphic->setParent(this);
              m_graphic->setSelected(true);
              emit isGeoElementSelectedChanged();
              break;
            }
          });
  //! [identify feature layer qml api snippet]
}

void GeometryEditorOnMap::deselectGraphics()
{
  if (m_graphic)
  {
    m_graphic->setSelected(false);
    m_graphic = nullptr;
  }

  m_feature = nullptr;
  foreach (Esri::ArcGISRuntime::FeatureLayer* featureLayer, m_featureLayers)
    featureLayer->clearSelection();

  emit isGeoElementSelectedChanged();
}

void GeometryEditorOnMap::toggleInteractionEdit(bool checked)
{
  if (checked)
    m_configuration->setInteractionMode(GeometryEditorInteractionMode::InteractionEdit);
  else
    m_configuration->setInteractionMode(GeometryEditorInteractionMode::SelectOnly);

  emit canDeleteSelectionChanged();
}

void GeometryEditorOnMap::toggleAllowSelectionMove(bool checked)
{
  m_configuration->setAllowSelectionMove(checked);
}

void GeometryEditorOnMap::toggleRequireSelection(bool checked)
{
  m_configuration->setRequireSelectionBeforeMove(checked);
}

void GeometryEditorOnMap::toggleVertexSelection(bool checked)
{
  m_configuration->setAllowVertexSelection(checked);
}

void GeometryEditorOnMap::togglePartSelection(bool checked)
{
  m_configuration->setAllowPartSelection(checked);
}

void GeometryEditorOnMap::toggleGeometrySelection(bool checked)
{
  m_configuration->setAllowGeometrySelection(checked);
}

void GeometryEditorOnMap::toggleMidVertexSelection(bool checked)
{
  m_configuration->setAllowMidVertexSelection(checked);
}
