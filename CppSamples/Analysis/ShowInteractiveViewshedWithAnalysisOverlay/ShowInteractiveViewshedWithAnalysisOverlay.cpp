// [WriteFile Name=ShowInteractiveViewshedWithAnalysisOverlay, Category=Analysis]
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

#include "ShowInteractiveViewshedWithAnalysisOverlay.h"

#include "AnalysisOverlay.h"
#include "AnalysisListModel.h"
#include "AnalysisOverlayListModel.h"
#include "Colormap.h"
#include "ColormapRenderer.h"
#include "ContinuousField.h"
#include "ContinuousFieldFunction.h"
#include "FieldAnalysis.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"
#include "ViewshedFunction.h"
#include "ViewshedParameters.h"

#include <QStandardPaths>
#include <QFuture>

using namespace Esri::ArcGISRuntime;

namespace
{
  QString defaultDataPath()
  {
#ifdef Q_OS_IOS
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
  }
} // namespace

ShowInteractiveViewshedWithAnalysisOverlay::ShowInteractiveViewshedWithAnalysisOverlay(QObject* parent /* = nullptr */) :
  QObject(parent),
  // Create a map with the imagery style.
  m_map(new Map(BasemapStyle::ArcGISImagery, this)),
  // The elevation file used in the analysis.
  m_elevationFilePath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/arran.tif")
{
}

ShowInteractiveViewshedWithAnalysisOverlay::~ShowInteractiveViewshedWithAnalysisOverlay() = default;

void ShowInteractiveViewshedWithAnalysisOverlay::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowInteractiveViewshedWithAnalysisOverlay>("Esri.Samples", 1, 0, "ShowInteractiveViewshedWithAnalysisOverlaySample");
}

MapQuickView* ShowInteractiveViewshedWithAnalysisOverlay::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowInteractiveViewshedWithAnalysisOverlay::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  setInitialViewpoint();

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  emit mapViewChanged();

  addOverlays();
  connectSignals();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setInitialViewpoint()
{
  // Set an initial viewpoint centered over the observer position.
  const double latitude  = 55.610000;
  const double longitude = -5.200346;
  const double scale = 100000;
  const Viewpoint initialViewpoint(latitude, longitude, scale);

  m_map->setInitialViewpoint(initialViewpoint);
}

void ShowInteractiveViewshedWithAnalysisOverlay::connectSignals()
{
  // Tap to reposition observer.
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!m_initialized)
    {
      return;
    }

    updateObserverPositionFromScreen(event.position().x(), event.position().y());
  });

  // Long-press starts observer drag.
  connect(m_mapView, &MapQuickView::mousePressedAndHeld, this, [this](QMouseEvent& event)
  {
    if (!m_initialized)
    {
      return;
    }

    startObserverDrag(event.position().x(), event.position().y());
  });

  // Update observer while dragging.
  connect(m_mapView, &MapQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (!m_initialized || !m_isDraggingObserver)
    {
      return;
    }

    dragObserver(event.position().x(), event.position().y());
  });

  // End observer drag.
  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](QMouseEvent& event)
  {
    if (!m_initialized || !m_isDraggingObserver)
    {
      return;
    }

    endObserverDrag(event.position().x(), event.position().y());
  });
}

void ShowInteractiveViewshedWithAnalysisOverlay::addOverlays()
{
  // Create and add a graphics overlay to the map view.
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Create and add an analysis overlay to the map view.
  m_analysisOverlay = new AnalysisOverlay(this);
  m_mapView->analysisOverlays()->append(m_analysisOverlay);

  createContinuousField();
}

void ShowInteractiveViewshedWithAnalysisOverlay::createContinuousField()
{
  // Create the continuous field from the elevation file.
  const QStringList elevationFilePaths{m_elevationFilePath};
  ContinuousField::createFromFilesAsync(elevationFilePaths, 0, this)
    .then(this,
          [this](ContinuousField* continuousField)
  {
    if (!continuousField)
    {
      return;
    }

    m_continuousField = continuousField;

    initializeViewshed();
  });
}

void ShowInteractiveViewshedWithAnalysisOverlay::initializeViewshed()
{
  // Set the initial observer position
  m_observerPositionPoint = Point(-579246.504, // x
                                  7479619.947, // y
                                  m_observerPointZ, // z
                                  SpatialReference::webMercator()); // spatialReference

  // Synchronize the viewshed parameters and graphic to the initial observer position.
  syncObserverGraphic();

  // Initialize the viewshed parameters.
  m_viewshedParameters = new ViewshedParameters(this);
  m_viewshedParameters->setObserverPosition(m_observerPositionPoint);
  m_viewshedParameters->setTargetHeight(m_targetHeight);
  m_viewshedParameters->setMaxRadius(m_maxRadius);
  m_viewshedParameters->setFieldOfView(m_fieldOfView);
  m_viewshedParameters->setHeading(m_headingValue);
  m_viewshedParameters->setElevationSamplingInterval(m_elevationSamplingInterval);

  m_viewshedFunction = ViewshedFunction::create(m_continuousField, m_viewshedParameters, this);

  // Convert the viewshed function to a discrete field function.
  m_discreteViewshed = m_viewshedFunction->toDiscreteFieldFunction();

  // Create colormap renderer for displaying viewshed result.
  const QList<QColor> colors{QColor(100, 100, 100, 160),QColor(90, 170, 90, 160)};
  Colormap* colormap = Colormap::create(colors, this);
  ColormapRenderer* colormapRenderer = new ColormapRenderer(colormap, this);

  FieldAnalysis* analysis = FieldAnalysis::create(m_discreteViewshed, colormapRenderer, this);
  m_analysisOverlay->analyses()->append(analysis);

  m_initialized = true;
  emit initializedChanged();
  emit observerElevationChanged();
  emit targetHeightChanged();
  emit maxRadiusChanged();
  emit fieldOfViewChanged();
  emit headingChanged();
  emit elevationSamplingIntervalChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::syncObserverGraphic()
{
  // Update the observer graphic geometry to the current observer position.
  if (!m_observerGraphic)
  {
    m_observerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(Qt::blue), 10.0f /*size*/, this);
    m_observerGraphic = new Graphic(m_observerPositionPoint, m_observerSymbol, this);

    m_graphicsOverlay->graphics()->append(m_observerGraphic);
    return;
  }

  m_observerGraphic->setGeometry(m_observerPositionPoint);
}

void ShowInteractiveViewshedWithAnalysisOverlay::updateObserverPositionFromScreen(double x, double y)
{
  if (!m_mapView || !m_viewshedParameters)
  {
    return;
  }

  // Find the map position corresponding to the tapped or dragged screen point.
  const Point mapPoint = m_mapView->screenToLocation(x, y);
  // Update the observer position while preserving the current elevation.
  m_observerPositionPoint = Point(mapPoint.x(), mapPoint.y(), m_observerPointZ, SpatialReference::webMercator());
  // Update the viewshed parameters.
  m_viewshedParameters->setObserverPosition(m_observerPositionPoint);
  syncObserverGraphic();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setObserverElevation(double value)
{
  m_observerPointZ = value;
  if (!m_initialized)
  {
    return;
  }

  // Update the observer elevation and sync analysis and graphic.
  m_observerPositionPoint = Point(m_observerPositionPoint.x(), m_observerPositionPoint.y(), m_observerPointZ, SpatialReference::webMercator());
  m_viewshedParameters->setObserverPosition(m_observerPositionPoint);
  syncObserverGraphic();
  emit observerElevationChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setTargetHeight(double value)
{
  if (m_targetHeight == value)
  {
    return;
  }

  m_targetHeight = value;
  if (m_viewshedParameters)
  {
    m_viewshedParameters->setTargetHeight(value);
  }

  emit targetHeightChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setMaxRadius(double value)
{
  if (m_maxRadius == value)
  {
    return;
  }

  m_maxRadius = value;
  if (m_viewshedParameters)
  {
    m_viewshedParameters->setMaxRadius(value);
  }

  emit maxRadiusChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setFieldOfView(double value)
{
  if (m_fieldOfView == value)
  {
    return;
  }

  m_fieldOfView = value;
  if (m_viewshedParameters)
  {
    m_viewshedParameters->setFieldOfView(value);
  }

  emit fieldOfViewChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setHeading(double value)
{
  if (m_headingValue == value)
  {
    return;
  }

  m_headingValue = value;
  if (m_viewshedParameters)
  {
    m_viewshedParameters->setHeading(value);
  }

  emit headingChanged();
}

void ShowInteractiveViewshedWithAnalysisOverlay::setElevationSamplingInterval(double value)
{
  std::optional<double> newInterval = (value == 0.0) ? std::nullopt : std::optional<double>(value);

  if (m_elevationSamplingInterval == newInterval)
  {
    return;
  }

  m_elevationSamplingInterval = newInterval;
  if (m_viewshedParameters)
  {
    m_viewshedParameters->setElevationSamplingInterval(m_elevationSamplingInterval);
  }

  emit elevationSamplingIntervalChanged();
}

double ShowInteractiveViewshedWithAnalysisOverlay::elevationSamplingInterval() const
{
  return m_elevationSamplingInterval.value_or(0.0);
}

bool ShowInteractiveViewshedWithAnalysisOverlay::initialized() const
{
  return m_initialized;
}

void ShowInteractiveViewshedWithAnalysisOverlay::startObserverDrag(double x, double y)
{
  if (!m_initialized)
  {
    return;
  }

  if (m_observerSymbol)
  {
    // Change the observer graphic color to indicate it is being moved.
    m_observerSymbol->setColor(QColor(Qt::yellow));
  }

  m_isDraggingObserver = true;
  updateObserverPositionFromScreen(x, y);
}

void ShowInteractiveViewshedWithAnalysisOverlay::dragObserver(double x, double y)
{
  if (!m_initialized || !m_isDraggingObserver) {
    return;
  }

  updateObserverPositionFromScreen(x, y);
}

void ShowInteractiveViewshedWithAnalysisOverlay::endObserverDrag(double x, double y)
{
  if (!m_initialized)
  {
    return;
  }

  updateObserverPositionFromScreen(x, y);

  if (m_observerSymbol)
  {
    // Change the observer graphic color back when dragging ends.
    m_observerSymbol->setColor(QColor(Qt::blue));
  }

  m_isDraggingObserver = false;
}
