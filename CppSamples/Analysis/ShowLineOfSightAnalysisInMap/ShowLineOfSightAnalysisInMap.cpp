// [WriteFile Name=ShowLineOfSightAnalysisInMap, Category=Analysis]
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

#include "ShowLineOfSightAnalysisInMap.h"

#include "AnalysisTypes.h"
#include "AttributeListModel.h"
#include "CalloutData.h"
#include "ContinuousField.h"
#include "Error.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "LineOfSight.h"
#include "LineOfSightFunction.h"
#include "LineOfSightParameters.h"
#include "LineOfSightPosition.h"
#include "LinearUnit.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "ObserverTargetPairs.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "Polyline.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "Viewpoint.h"

#include <QFileInfo>
#include <QFuture>
#include <QMouseEvent>
#include <QStandardPaths>

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

ShowLineOfSightAnalysisInMap::ShowLineOfSightAnalysisInMap(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_elevationFilePath(defaultDataPath() + "/ArcGIS/Runtime/Data/raster/arran.tif"),
  m_map(new Map(BasemapStyle::ArcGISHillshadeDark, this))
{
}

ShowLineOfSightAnalysisInMap::~ShowLineOfSightAnalysisInMap() = default;

void ShowLineOfSightAnalysisInMap::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ShowLineOfSightAnalysisInMap>("Esri.Samples", 1, 0, "ShowLineOfSightAnalysisInMapSample");
}

MapQuickView* ShowLineOfSightAnalysisInMap::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ShowLineOfSightAnalysisInMap::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;

  connect(m_mapView, &MapQuickView::mouseClicked, this, &ShowLineOfSightAnalysisInMap::identifyObserverAt);

  initialize();

  emit mapViewChanged();
}

void ShowLineOfSightAnalysisInMap::setVisibilityFilter(bool visibilityFilter)
{
  if (m_visibilityFilter == visibilityFilter)
  {
    return;
  }

  m_visibilityFilter = visibilityFilter;
  applyVisibilityFilter();
  emit visibilityFilterChanged();
}

void ShowLineOfSightAnalysisInMap::initialize()
{
  if (!m_mapView || m_initialized)
  {
    return;
  }

  m_initialized = true;
  m_isObserverIdentifyEnabled = false;

  m_mapView->setMap(m_map);

  m_resultsGraphicsOverlay = new GraphicsOverlay(this);
  m_observersGraphicsOverlay = new GraphicsOverlay(this);
  m_targetGraphicsOverlay = new GraphicsOverlay(this);

  m_mapView->graphicsOverlays()->append(m_resultsGraphicsOverlay);
  m_mapView->graphicsOverlays()->append(m_observersGraphicsOverlay);
  m_mapView->graphicsOverlays()->append(m_targetGraphicsOverlay);

  PictureMarkerSymbol* beaconSymbol =
    new PictureMarkerSymbol(QUrl(QStringLiteral("qrc:/Samples/Analysis/ShowLineOfSightAnalysisInMap/iconAssets/beacon.png")), this);

  beaconSymbol->setWidth(24.0);
  beaconSymbol->setHeight(24.0);

  m_targetPoint = Point(-577955.365, 7484288.220, 5.0, SpatialReference::webMercator());
  Graphic* targetGraphic = new Graphic(m_targetPoint, beaconSymbol, this);
  m_targetGraphicsOverlay->graphics()->append(targetGraphic);

  m_observers = createObservers();

  for (int i = 0; i < m_observers.size(); ++i)
  {
    const ObserverDefinition& observer = m_observers.at(i);
    SimpleMarkerSymbol* symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, observer.color, 15.0, this);
    Graphic* observerGraphic = new Graphic(observer.position, symbol, this);
    observerGraphic->attributes()->insertAttribute(QStringLiteral("observerIndex"), i);
    m_observersGraphicsOverlay->graphics()->append(observerGraphic);
  }

  m_map->setInitialViewpoint(Viewpoint(m_targetPoint, 150000.0));
  createLineOfSightAnalysis();
}

void ShowLineOfSightAnalysisInMap::createLineOfSightAnalysis()
{
  if (!QFileInfo::exists(m_elevationFilePath))
  {
    m_isObserverIdentifyEnabled = true;
    return;
  }

  ContinuousField::createFromFilesAsync({m_elevationFilePath}, 0, this).then(this, [this](ContinuousField* elevation)
  {
    onElevationFieldCreated(elevation);
  });
}

void ShowLineOfSightAnalysisInMap::onElevationFieldCreated(ContinuousField* elevation)
{
  if (!elevation)
  {
    m_isObserverIdentifyEnabled = true;
    return;
  }

  LineOfSightPosition* targetPosition = new LineOfSightPosition(m_targetPoint, HeightOrigin::Relative, this);
  QList<LineOfSightPosition*> targetPositions{targetPosition};

  QList<LineOfSightPosition*> observerPositions;
  observerPositions.reserve(m_observers.size());
  for (const ObserverDefinition& observer : m_observers)
  {
    observerPositions.append(new LineOfSightPosition(observer.position, HeightOrigin::Relative, this));
  }

  LineOfSightParameters* parameters = new LineOfSightParameters(this);
  parameters->setObserverTargetPairs(ObserverTargetPairs::create(observerPositions, targetPositions, this));

  LineOfSightFunction* lineOfSightFunction = LineOfSightFunction::create(elevation, parameters, this);

  lineOfSightFunction->evaluateAsync(this).then(this, [this](const QList<LineOfSight*>& results)
  {
    onLineOfSightEvaluated(results);
  });
}

void ShowLineOfSightAnalysisInMap::onLineOfSightEvaluated(const QList<LineOfSight*>& results)
{
  if (results.isEmpty())
  {
    return;
  }

  SimpleLineSymbol* visibleLineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::green, 4.0, this);
  SimpleLineSymbol* notVisibleLineSymbol =
    new SimpleLineSymbol(SimpleLineSymbolStyle::LongDash, Qt::gray, 2.0, this);

  m_lineOfSightResults.clear();

  for (int i = 0; i < results.size(); ++i)
  {
    LineOfSight* result = results.at(i);
    const float targetVisibility = result->targetVisibility();
    m_lineOfSightResults.append(result);

    const Polyline visibleLine = result->visibleLine();
    if (!visibleLine.isEmpty())
    {
      Graphic* visibleLineGraphic = new Graphic(visibleLine, visibleLineSymbol, this);
      visibleLineGraphic->attributes()->insertAttribute(QStringLiteral("targetVisibility"), targetVisibility);
      m_resultsGraphicsOverlay->graphics()->append(visibleLineGraphic);
    }

    const Polyline notVisibleLine = result->notVisibleLine();
    if (!notVisibleLine.isEmpty())
    {
      Graphic* notVisibleLineGraphic = new Graphic(notVisibleLine, notVisibleLineSymbol, this);
      notVisibleLineGraphic->attributes()->insertAttribute(QStringLiteral("targetVisibility"), targetVisibility);
      m_resultsGraphicsOverlay->graphics()->append(notVisibleLineGraphic);
    }
  }

  applyVisibilityFilter();
  m_isObserverIdentifyEnabled = true;
}

void ShowLineOfSightAnalysisInMap::identifyObserverAt(QMouseEvent& mouseEvent)
{
  if (!m_isObserverIdentifyEnabled || !m_mapView || !m_observersGraphicsOverlay)
  {
    return;
  }

  m_mapView->calloutData()->setVisible(false);

  m_mapView->identifyGraphicsOverlayAsync(m_observersGraphicsOverlay, mouseEvent.position(), 10.0, false, this)
    .then(this, [this](IdentifyGraphicsOverlayResult* identifyResult)
  {
    onIdentifyObserverCompleted(identifyResult);
  });
}

void ShowLineOfSightAnalysisInMap::onIdentifyObserverCompleted(IdentifyGraphicsOverlayResult* identifyResult)
{
  if (!identifyResult)
  {
    return;
  }

  const QList<Graphic*> graphics = identifyResult->graphics();
  if (graphics.isEmpty())
  {
    return;
  }

  Graphic* observerGraphic = graphics.constFirst();
  const int observerIndex = observerGraphic->attributes()->attributeValue(QStringLiteral("observerIndex")).toInt();
  const QString detailText = lineOfSightDetail(m_lineOfSightResults.value(observerIndex, nullptr));

  CalloutData* calloutData = m_mapView->calloutData();
  const Point observerPoint(observerGraphic->geometry());
  calloutData->setLocation(observerPoint);
  calloutData->setDetail(detailText);
  calloutData->setGeoElement(observerGraphic);
  calloutData->setVisible(true);
}

QString ShowLineOfSightAnalysisInMap::lineOfSightDetail(LineOfSight* result) const
{
  if (!result)
  {
    return QString();
  }

  const Error error = result->error();
  if (!error.isEmpty())
  {
    return error.additionalMessage().isEmpty() ? error.message() : error.additionalMessage();
  }

  const Polyline visibleLine = result->visibleLine();
  const Polyline notVisibleLine = result->notVisibleLine();
  if (visibleLine.isEmpty() && notVisibleLine.isEmpty())
  {
    return QString();
  }

  const double visibleLength =
    visibleLine.isEmpty() ? 0.0 : GeometryEngine::lengthGeodetic(visibleLine, LinearUnit::meters(), GeodeticCurveType::Geodesic);

  if (notVisibleLine.isEmpty())
  {
    return QStringLiteral("Target visible from observer over %1 meters.").arg(visibleLength, 0, 'f', 1);
  }

  return QStringLiteral("Target not visible from observer. Obstructed after %1 meters.").arg(visibleLength, 0, 'f', 1);
}

void ShowLineOfSightAnalysisInMap::applyVisibilityFilter()
{
  if (!m_resultsGraphicsOverlay)
  {
    return;
  }

  for (int i = 0; i < m_resultsGraphicsOverlay->graphics()->size(); ++i)
  {
    Graphic* lineGraphic = m_resultsGraphicsOverlay->graphics()->at(i);
    const float targetVisibility = lineGraphic->attributes()->attributeValue(QStringLiteral("targetVisibility")).toFloat();
    lineGraphic->setVisible(!m_visibilityFilter || targetVisibility == 1.0);
  }
}

QList<ShowLineOfSightAnalysisInMap::ObserverDefinition> ShowLineOfSightAnalysisInMap::createObservers()
{
  const SpatialReference webMercator = SpatialReference::webMercator();
  return {
    {QColor(Qt::green), Point(-580893.546, 7489102.890, 5.0, webMercator)},
    {QColor(Qt::white), Point(-583446.004, 7483567.462, 5.0, webMercator)},
    {QColor(255, 165, 0), Point(-577665.236, 7490792.908, 5.0, webMercator)},
    {QColor(Qt::yellow), Point(-576452.981, 7487071.388, 5.0, webMercator)},
    {QColor(228, 168, 239), Point(-576650.067, 7481479.772, 5.0, webMercator)},
    {QColor(Qt::blue), Point(-571683.896, 7492017.864, 5.0, webMercator)}
  };
}
