// [WriteFile Name=GetFieldFunctionValueFromAPoint, Category=Analysis]
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
#include "GetFieldFunctionValueFromAPoint.h"

// ArcGIS Maps SDK headers
#include "Analysis.h"
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "AnalysisViewState.h"
#include "BooleanFieldFunction.h"
#include "BooleanFieldSample.h"
#include "BooleanFieldSampleFunction.h"
#include "CalloutData.h"
#include "ColorRamp.h"
#include "Colormap.h"
#include "ColormapRenderer.h"
#include "ContinuousField.h"
#include "ContinuousFieldFunction.h"
#include "ContinuousFieldSample.h"
#include "ContinuousFieldSampleFunction.h"
#include "ContinuousFieldSupplier.h"
#include "Envelope.h"
#include "Error.h"
#include "ErrorException.h"
#include "FieldAnalysis.h"
#include "FloatSupplier.h"
#include "GeoModel.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "MinMaxStretchParameters.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "SpatialReference.h"
#include "StretchRenderer.h"

// Qt headers
#include <QColor>
#include <QFuture>
#include <QImage>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QtGlobal>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
static QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return dataPath;
}

GetFieldFunctionValueFromAPoint::GetFieldFunctionValueFromAPoint(QObject* parent /* = nullptr */) :
  QObject(parent),
  // Create a map with the ArcGIS hillshade dark basemap style.
  m_map(new Map(BasemapStyle::ArcGISHillshadeDark, this)),
  m_elevationFilePath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/Meribel.tif")
{
}

GetFieldFunctionValueFromAPoint::~GetFieldFunctionValueFromAPoint() = default;

void GetFieldFunctionValueFromAPoint::init()
{
  // Register the map view for QML.
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GetFieldFunctionValueFromAPoint>("Esri.Samples", 1, 0, "GetFieldFunctionValueFromAPointSample");
}

MapQuickView* GetFieldFunctionValueFromAPoint::mapView() const
{
  return m_mapView;
}

// Set the view created in QML.
void GetFieldFunctionValueFromAPoint::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  connect(m_map, &GeoModel::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      setLoading(false);
      return;
    }

    // Wait for the map spatial reference before creating field analyses.
    m_mapLoaded = true;
    initializeFieldAnalysis();
  });

  m_mapView->setMap(m_map);
  emit mapViewChanged();

  addOverlays();
  connectSignals();
  createContinuousField();
}

void GetFieldFunctionValueFromAPoint::addOverlays()
{
  // Create a graphics overlay to display tapped point on the map.
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // Create an analysis overlay to display field analyses on the map.
  m_analysisOverlay = new AnalysisOverlay(this);
  m_mapView->analysisOverlays()->append(m_analysisOverlay);
}

void GetFieldFunctionValueFromAPoint::connectSignals()
{
  // Convert map-view mouse input into map locations for sampling.
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!m_initialized || m_isDraggingPoint)
    {
      return;
    }

    requestSampleAtPoint(m_mapView->screenToLocation(event.position().x(), event.position().y()));
  });

  connect(m_mapView, &MapQuickView::mousePressedAndHeld, this, [this](QMouseEvent& event)
  {
    if (!m_initialized)
    {
      return;
    }

    m_isDraggingPoint = true;
    requestSampleAtPoint(m_mapView->screenToLocation(event.position().x(), event.position().y()));
  });

  connect(m_mapView, &MapQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (!m_initialized || !m_isDraggingPoint)
    {
      return;
    }

    requestSampleAtPoint(m_mapView->screenToLocation(event.position().x(), event.position().y()));
  });

  connect(m_mapView, &MapQuickView::mouseReleased, this, [this](QMouseEvent& event)
  {
    if (!m_isDraggingPoint)
    {
      return;
    }

    requestSampleAtPoint(m_mapView->screenToLocation(event.position().x(), event.position().y()));
    m_isDraggingPoint = false;
  });

  connect(m_mapView, &MapQuickView::analysisViewStateChanged, this, [this](Analysis* analysis, const AnalysisViewState& viewState)
  {
    const auto* analyses = m_analysisOverlay->analyses();
    if (!analyses->contains(analysis))
    {
      return;
    }

    bool updating = false;
    for (int i = 0; i < analyses->size(); ++i)
    {
      Analysis* current = analyses->at(i);
      const auto status = current == analysis ? viewState.status() : m_mapView->analysisViewState(current).status();
      if (status == AnalysisViewStatus::Updating)
      {
        updating = true;
        break;
      }
    }

    if (m_analysisUpdating != updating)
    {
      m_analysisUpdating = updating;
      emit analysisUpdatingChanged();
    }
  });
}

void GetFieldFunctionValueFromAPoint::createContinuousField()
{
  // Load the elevation raster's first band to create the continuous field.
  ContinuousField::createFromFilesAsync({m_elevationFilePath}, 0, this)
    .then(this,
          [this](ContinuousField* continuousField)
  {
    onContinuousFieldCreated(continuousField);
  })
    .onFailed(this, [this](const ErrorException&)
  {
    setLoading(false);
  });
}

void GetFieldFunctionValueFromAPoint::onContinuousFieldCreated(ContinuousField* continuousField)
{
  if (!continuousField)
  {
    setLoading(false);
    return;
  }

  m_continuousField = continuousField;
  initializeFieldAnalysis();
}

void GetFieldFunctionValueFromAPoint::initializeFieldAnalysis()
{
  if (!m_mapLoaded || !m_continuousField || m_initialized)
  {
    return;
  }

  // Create the continuous field function used to sample elevation values.
  ContinuousFieldSupplier* elevationFunction = new ContinuousFieldSupplier(m_continuousField, this);
  m_aircraftAltitudeSupplier = new FloatSupplier(std::make_optional(m_aircraftAltitude), this);
  // The boolean function is true where terrain elevation is below the supplied aircraft altitude.
  BooleanFieldFunction* clearanceFunction = elevationFunction->isLessThan(m_aircraftAltitudeSupplier);

  // Place the initial sample at the center of the elevation field.
  const Point fieldCenter = m_continuousField->extent().center();
  // The sample point's z-coordinate represents the aircraft altitude.
  m_samplePoint = Point(fieldCenter.x(), fieldCenter.y(), m_aircraftAltitude, fieldCenter.spatialReference());
  PictureMarkerSymbol* aircraftSymbol =
    new PictureMarkerSymbol(QImage(QStringLiteral(":/Samples/Analysis/GetFieldFunctionValueFromAPoint/helicopter.png")), this);
  aircraftSymbol->setWidth(28.0f);
  aircraftSymbol->setHeight(28.0f);
  m_samplePointGraphic = new Graphic(m_samplePoint, aircraftSymbol, this);
  m_graphicsOverlay->graphics()->append(m_samplePointGraphic);

  // Create sample functions for elevation and ground clearance at the selected point.
  m_elevationSampleFunction = ContinuousFieldSampleFunction::create(elevationFunction, this);
  m_clearanceSampleFunction = BooleanFieldSampleFunction::create(clearanceFunction, this);
  if (!m_elevationSampleFunction || !m_clearanceSampleFunction)
  {
    setLoading(false);
    return;
  }

  // Add rendered analyses after the map and raster are both ready.
  if (!createFieldAnalysis(elevationFunction, clearanceFunction))
  {
    setLoading(false);
    return;
  }

  // Set the initial map viewpoint to the center of the elevation field.
  m_mapView->setViewpointCenterAsync(fieldCenter, 50000.0);

  m_initialized = true;
  setLoading(false);
  emit initializedChanged();
  // Sample the initial point to populate the callout.
  requestSampleAtPoint(m_samplePoint);
}

bool GetFieldFunctionValueFromAPoint::createFieldAnalysis(ContinuousFieldFunction* elevationFunction, BooleanFieldFunction* clearanceFunction)
{
  // Render elevation using a stretch renderer and a purple-gray-to-white color ramp.
  const MinMaxStretchParameters stretchParameters({1199.0}, {2740.0});
  ColorRamp* colorRamp = ColorRamp::createFromColors({QColor(96, 98, 148), QColor(Qt::white)}, 256, this);
  constexpr bool estimateStats = false;
  StretchRenderer* stretchRenderer = new StretchRenderer(stretchParameters, {1.0}, estimateStats, colorRamp, this);

  FieldAnalysis* elevationAnalysis = FieldAnalysis::create(elevationFunction, stretchRenderer, this);
  if (!elevationAnalysis)
  {
    return false;
  }
  elevationAnalysis->setOpacity(0.5f);
  m_analysisOverlay->analyses()->append(elevationAnalysis);

  // Boolean colormap index 0 is false (no clearance), and index 1 is true (clear).
  const QList<QColor> colors{QColor(210, 60, 60), QColor(50, 170, 90, 0)};
  Colormap* colormap = Colormap::create(colors, this);
  ColormapRenderer* colormapRenderer = new ColormapRenderer(colormap, this);
  FieldAnalysis* clearanceAnalysis = FieldAnalysis::create(clearanceFunction, colormapRenderer, this);
  if (!clearanceAnalysis)
  {
    return false;
  }
  clearanceAnalysis->setOpacity(0.5f);
  m_analysisOverlay->analyses()->append(clearanceAnalysis);
  return true;
}

void GetFieldFunctionValueFromAPoint::requestSampleAtPoint(const Point& point)
{
  if (!m_initialized)
  {
    return;
  }

  // Update the marker geometry and sample location while preserving the current altitude.
  m_samplePoint = Point(point.x(), point.y(), m_aircraftAltitude, point.spatialReference());
  m_samplePointGraphic->setGeometry(m_samplePoint);
  ++m_sampleRequestId;
  updateCallout(QStringLiteral("Sampling field functions..."));
  sampleCurrentPoint();
}

void GetFieldFunctionValueFromAPoint::sampleCurrentPoint()
{
  if (m_isSampling || !m_elevationSampleFunction || !m_clearanceSampleFunction)
  {
    return;
  }

  m_isSampling = true;
  const quint64 requestId = m_sampleRequestId;
  // Evaluate both sample functions at the same map location.
  m_elevationSampleFunction->setPosition(m_samplePoint);
  m_clearanceSampleFunction->setPosition(m_samplePoint);

  // Evaluate elevation first so its value can also be used to calculate numeric clearance.
  m_elevationSampleFunction->evaluateAsync(this)
    .then(this,
          [this, requestId](ContinuousFieldSample* rawSample)
  {
    const auto sample = std::unique_ptr<ContinuousFieldSample>(rawSample);
    if (!sample)
    {
      finishSampling(requestId, true);
      return;
    }

    if (requestId != m_sampleRequestId)
    {
      finishSampling(requestId);
      return;
    }

    // An empty optional means the sampled location contains NoData.
    const std::optional<float> elevation = sample->value();
    m_clearanceSampleFunction->evaluateAsync(this)
      .then(this,
            [this, requestId, elevation](BooleanFieldSample* rawClearanceSample)
    {
      const auto clearanceSample = std::unique_ptr<BooleanFieldSample>(rawClearanceSample);
      if (!clearanceSample)
      {
        finishSampling(requestId, true);
        return;
      }

      if (requestId == m_sampleRequestId)
      {
        // Display both sampled values and derived clearance at the current point.
        updateCallout(sampleDetail(elevation, clearanceSample->value()));
      }

      finishSampling(requestId);
    })
      .onFailed(this, [this, requestId](const ErrorException&)
    {
      finishSampling(requestId, true);
    });
  })
    .onFailed(this, [this, requestId](const ErrorException&)
  {
    finishSampling(requestId, true);
  });
}

void GetFieldFunctionValueFromAPoint::finishSampling(quint64 requestId, bool failed)
{
  m_isSampling = false;

  if (requestId != m_sampleRequestId)
  {
    sampleCurrentPoint();
    return;
  }

  if (failed)
  {
    m_mapView->calloutData()->setVisible(false);
  }
}

void GetFieldFunctionValueFromAPoint::setAircraftAltitude(float altitude)
{
  if (m_aircraftAltitude == altitude)
  {
    return;
  }

  m_aircraftAltitude = altitude;
  emit aircraftAltitudeChanged();

  if (!m_initialized)
  {
    return;
  }

  // The supplier updates the analysis; reevaluate the sample functions to refresh the callout.
  m_aircraftAltitudeSupplier->setValue(std::make_optional(m_aircraftAltitude));
  requestSampleAtPoint(m_samplePoint);
}

void GetFieldFunctionValueFromAPoint::setLoading(bool loading)
{
  if (m_loading == loading)
  {
    return;
  }

  m_loading = loading;
  emit loadingChanged();
}

void GetFieldFunctionValueFromAPoint::updateCallout(const QString& detail)
{
  if (!m_mapView)
  {
    return;
  }

  // Display the sampled field values at the current map location.
  CalloutData* calloutData = m_mapView->calloutData();
  calloutData->setTitle(QStringLiteral("Field function values"));
  calloutData->setLocation(m_samplePoint);
  calloutData->setDetail(detail);
  calloutData->setVisible(true);
}

float GetFieldFunctionValueFromAPoint::aircraftAltitude() const
{
  return m_aircraftAltitude;
}

bool GetFieldFunctionValueFromAPoint::analysisUpdating() const
{
  return m_analysisUpdating;
}

QString GetFieldFunctionValueFromAPoint::sampleDetail(std::optional<float> elevation, std::optional<bool> hasClearance) const
{
  // Keep null for NoData, such as locations outside the raster extent.
  const QString elevationText = elevation.has_value() ? QString::number(elevation.value(), 'f', 2) : QStringLiteral("null");
  // Calculate numeric ground clearance as aircraft altitude minus terrain elevation.
  const QString clearanceText = elevation.has_value() ? QString::number(m_aircraftAltitude - elevation.value(), 'f', 2) : QStringLiteral("null");
  const QString clearanceStatus =
    hasClearance.has_value() ? (hasClearance.value() ? QStringLiteral("Clear") : QStringLiteral("Not Clear")) : QStringLiteral("null");
  return QStringLiteral("Elevation (m): %1 \nClearance above the ground (m): %2 \nGround clearance status: %3")
    .arg(elevationText, clearanceText, clearanceStatus);
}

bool GetFieldFunctionValueFromAPoint::loading() const
{
  return m_loading;
}

bool GetFieldFunctionValueFromAPoint::initialized() const
{
  return m_initialized;
}
