// [WriteFile Name=NavigateMapViewAndIdentifyFeaturesWithKeyboard, Category=Accessibility]
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
#include "NavigateMapViewAndIdentifyFeaturesWithKeyboard.h"

// ArcGIS Maps SDK headers
#include "AttributeListModel.h"
#include "CalloutData.h"
#include "CoreTypes.h"
#include "Envelope.h"
#include "Feature.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "GeodatabaseTypes.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Point.h"
#include "QueryParameters.h"
#include "SelectionProperties.h"
#include "ServiceFeatureTable.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TextSymbol.h"
#include "Viewpoint.h"

// Qt headers
#include <QColor>
#include <QFuture>
#include <QPointF>
#include <QUrl>

// STL headers
#include <algorithm>

using namespace Esri::ArcGISRuntime;

// Colors used for the marker, selection halo, and label.
static constexpr QColor markerFill{11, 79, 138};
static constexpr QColor selectionHalo{190, 24, 93};
static constexpr QColor labelText{31, 35, 40};

NavigateMapViewAndIdentifyFeaturesWithKeyboard::NavigateMapViewAndIdentifyFeaturesWithKeyboard(QObject* parent) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISLightGray, this)),
  m_restaurantsTable(new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/"
                                                  "redlands_food/FeatureServer/0"),
                                             this)),
  m_restaurantsLayer(new FeatureLayer(m_restaurantsTable, this)),
  m_labelsOverlay(new GraphicsOverlay(this))
{
  SimpleMarkerSymbol* restaurantSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, markerFill, 12.0F, this);
  SimpleLineSymbol* restaurantOutline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::white, 1.5F, this);
  restaurantSymbol->setOutline(restaurantOutline);
  m_restaurantsLayer->setRenderer(new SimpleRenderer(restaurantSymbol, this));

  const Point redlandsCenter(-117.1825, 34.0556, SpatialReference::wgs84());
  m_map->setInitialViewpoint(Viewpoint(redlandsCenter, 2500.0));
  m_map->operationalLayers()->append(m_restaurantsLayer);
}

NavigateMapViewAndIdentifyFeaturesWithKeyboard::~NavigateMapViewAndIdentifyFeaturesWithKeyboard() = default;

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<NavigateMapViewAndIdentifyFeaturesWithKeyboard>("Esri.Samples", 1, 0, "NavigateMapViewAndIdentifyFeaturesWithKeyboardSample");
}

MapQuickView* NavigateMapViewAndIdentifyFeaturesWithKeyboard::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void NavigateMapViewAndIdentifyFeaturesWithKeyboard::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setSelectionProperties(SelectionProperties(selectionHalo));
  m_mapView->graphicsOverlays()->append(m_labelsOverlay);

  // Wait until navigation stops before finding features in the updated view.
  connect(m_mapView, &MapQuickView::navigatingChanged, this, [this]()
  {
    if (m_mapView->isNavigating())
    {
      if (m_queryInProgress)
      {
        m_refreshPending = true;
      }
      return;
    }

    if (calloutIsVisible())
    {
      m_refreshPending = true;
      if (!calloutIsInView())
      {
        dismissCallout();
      }
      return;
    }

    identifyFeatures();
  });
  // Run the first query after the map has finished drawing.
  connect(m_mapView, &MapQuickView::drawStatusChanged, this, [this](DrawStatus drawStatus)
  {
    if (drawStatus == DrawStatus::Completed && !m_initialDrawCompleted)
    {
      m_initialDrawCompleted = true;
      identifyFeatures();
    }
  });
  emit mapViewChanged();
}

double NavigateMapViewAndIdentifyFeaturesWithKeyboard::areaOfInterestSize() const
{
  return m_areaOfInterestSize;
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::setAreaOfInterestSize(double size)
{
  if (qFuzzyCompare(m_areaOfInterestSize, size))
  {
    return;
  }

  m_areaOfInterestSize = size;
  emit areaOfInterestSizeChanged();
  identifyFeatures();
}

bool NavigateMapViewAndIdentifyFeaturesWithKeyboard::overflowVisible() const
{
  return m_overflowVisible;
}

QString NavigateMapViewAndIdentifyFeaturesWithKeyboard::errorMessage() const
{
  return m_errorMessage;
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::identifyFeatures()
{
  if (!m_mapView || m_mapView->width() <= 0.0 || m_mapView->height() <= 0.0 || m_areaOfInterestSize <= 0.0)
  {
    return;
  }

  // Keep the current callout stable and refresh after it is dismissed.
  if (calloutIsVisible())
  {
    m_refreshPending = true;
    return;
  }

  // Queue one refresh instead of starting overlapping queries.
  if (m_queryInProgress)
  {
    m_refreshPending = true;
    return;
  }

  // Convert the on-screen square into a map envelope for the feature query.
  const double halfSize = m_areaOfInterestSize / 2.0;
  const double centerX = m_mapView->width() / 2.0;
  const double centerY = m_mapView->height() / 2.0;
  const Point center = m_mapView->screenToLocation(centerX, centerY);
  const Point horizontalEdge = m_mapView->screenToLocation(centerX + halfSize, centerY);
  const Point verticalEdge = m_mapView->screenToLocation(centerX, centerY + halfSize);
  if (!center.isValid() || !horizontalEdge.isValid() || !verticalEdge.isValid())
  {
    return;
  }

  const double width = GeometryEngine::distance(center, horizontalEdge) * 2.0;
  const double height = GeometryEngine::distance(center, verticalEdge) * 2.0;
  const Envelope areaOfInterest(center, width, height);

  QueryParameters parameters;
  parameters.setGeometry(GeometryEngine::normalizeCentralMeridian(areaOfInterest));
  parameters.setSpatialRelationship(SpatialRelationship::Intersects);
  parameters.setReturnGeometry(true);

  clearSelection();
  m_queryInProgress = true;
  setErrorMessage(QString());
  QFuture<FeatureQueryResult*> queryFuture = m_restaurantsTable->queryFeaturesAsync(parameters, QueryFeatureFields::LoadAll, this);
  queryFuture
    .then(this,
          [this](FeatureQueryResult* queryResult)
  {
    if (calloutIsVisible())
    {
      m_queryInProgress = false;
      m_refreshPending = true;
      if (queryResult)
      {
        queryResult->deleteLater();
      }
      return;
    }
    if (m_refreshPending)
    {
      // Discard results for an old view and query the latest view instead.
      m_queryInProgress = false;
      m_refreshPending = false;
      if (queryResult)
      {
        queryResult->deleteLater();
      }
      identifyFeatures();
      return;
    }

    processQueryResult(queryResult);
    m_queryInProgress = false;
    if (m_refreshPending)
    {
      m_refreshPending = false;
      identifyFeatures();
    }
  })
    .onFailed(this, [this](const std::exception& exception)
  {
    m_queryInProgress = false;
    if (calloutIsVisible())
    {
      m_refreshPending = true;
      return;
    }
    if (m_refreshPending)
    {
      m_refreshPending = false;
      identifyFeatures();
      return;
    }
    setErrorMessage(QString::fromUtf8(exception.what()));
  });
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::processQueryResult(FeatureQueryResult* queryResult)
{
  if (!queryResult)
  {
    return;
  }

  struct OrderedFeature
  {
    Feature* feature = nullptr;
    QPointF screenPoint;
  };

  QVector<OrderedFeature> orderedFeatures;
  FeatureIterator iterator = queryResult->iterator();
  while (iterator.hasNext())
  {
    Feature* feature = iterator.next(this);
    const Point point(feature->geometry());
    if (!point.isValid())
    {
      feature->deleteLater();
      continue;
    }

    orderedFeatures.append({feature, m_mapView->locationToScreen(point)});
  }

  // Number features from top to bottom, then from left to right.
  std::sort(orderedFeatures.begin(), orderedFeatures.end(), [](const OrderedFeature& first, const OrderedFeature& second)
  {
    return first.screenPoint.y() == second.screenPoint.y() ? first.screenPoint.x() < second.screenPoint.x() :
                                                             first.screenPoint.y() < second.screenPoint.y();
  });

  const bool overflowVisible = orderedFeatures.size() > 9;
  for (qsizetype index = 0; index < orderedFeatures.size(); ++index)
  {
    Feature* feature = orderedFeatures.at(index).feature;
    m_identifiedFeatures.append(feature);
    m_restaurantsLayer->selectFeature(feature);
    // Only the first nine features can be reached with the number keys.
    if (index >= 9)
    {
      continue;
    }

    const QString name = featureName(feature);
    const QString label = name.isEmpty() ? QString::number(index + 1) : QString("%1: %2").arg(index + 1).arg(name);
    TextSymbol* textSymbol = new TextSymbol(label, labelText, 15.0F, HorizontalAlignment::Center, VerticalAlignment::Top, this);
    textSymbol->setHaloColor(Qt::white);
    textSymbol->setHaloWidth(2.0F);
    textSymbol->setOffsetY(-14.0F);
    Graphic* labelGraphic = new Graphic(feature->geometry(), textSymbol, this);
    textSymbol->setParent(labelGraphic);
    m_labelsOverlay->graphics()->append(labelGraphic);
  }

  if (m_overflowVisible != overflowVisible)
  {
    m_overflowVisible = overflowVisible;
    emit overflowVisibleChanged();
  }

  queryResult->deleteLater();
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::clearSelection()
{
  m_restaurantsLayer->clearSelection();
  // Remove the old number labels before creating labels for the next query.
  QList<Graphic*> labelGraphics;
  labelGraphics.reserve(m_labelsOverlay->graphics()->size());
  for (int index = 0; index < m_labelsOverlay->graphics()->size(); ++index)
  {
    labelGraphics.append(m_labelsOverlay->graphics()->at(index));
  }
  m_labelsOverlay->graphics()->clear();
  for (Graphic* labelGraphic : std::as_const(labelGraphics))
  {
    labelGraphic->deleteLater();
  }

  if (m_overflowVisible)
  {
    m_overflowVisible = false;
    emit overflowVisibleChanged();
  }

  // Delete every feature created from the previous query result.
  for (const QPointer<Feature>& feature : std::as_const(m_identifiedFeatures))
  {
    if (feature)
    {
      feature->deleteLater();
    }
  }
  m_identifiedFeatures.clear();
}

bool NavigateMapViewAndIdentifyFeaturesWithKeyboard::calloutIsVisible() const
{
  return m_mapView && m_mapView->calloutData()->isVisible();
}

bool NavigateMapViewAndIdentifyFeaturesWithKeyboard::calloutIsInView() const
{
  if (!calloutIsVisible())
  {
    return false;
  }

  const QPointF screenPoint = m_mapView->calloutData()->screenPoint();
  return screenPoint.x() >= 0.0 && screenPoint.x() <= m_mapView->width() && screenPoint.y() >= 0.0 && screenPoint.y() <= m_mapView->height();
}

QString NavigateMapViewAndIdentifyFeaturesWithKeyboard::featureName(Feature* feature) const
{
  const QStringList attributeNames = feature->attributes()->attributeNames();
  for (const QString& attributeName : attributeNames)
  {
    if (attributeName.compare("name", Qt::CaseInsensitive) == 0)
    {
      return feature->attributes()->attributeValue(attributeName).toString().trimmed();
    }
  }
  return {};
}

bool NavigateMapViewAndIdentifyFeaturesWithKeyboard::showCallout(int featureNumber)
{
  // Convert keys 1-9 into indexes 0-8 in the sorted feature list.
  const int featureIndex = featureNumber - 1;
  if (!m_mapView || featureIndex < 0 || featureIndex >= 9 || featureIndex >= m_identifiedFeatures.size() || !m_identifiedFeatures.at(featureIndex))
  {
    return false;
  }

  Feature* feature = m_identifiedFeatures.at(featureIndex);
  CalloutData* calloutData = m_mapView->calloutData();
  const QString name = featureName(feature);
  const Point featureLocation(feature->geometry());
  const Point location(GeometryEngine::project(feature->geometry(), SpatialReference::wgs84()));
  if (!featureLocation.isValid() || !location.isValid())
  {
    return false;
  }

  // Move the leader slightly above the marker so the callout does not cover it.
  const QPointF screenPoint = m_mapView->locationToScreen(featureLocation);
  const Point leaderLocation = m_mapView->screenToLocation(screenPoint.x(), screenPoint.y() - 4.0);
  calloutData->setLocation(leaderLocation.isValid() ? leaderLocation : featureLocation);
  calloutData->setTitle(name.isEmpty() ? tr("Restaurant") : name);
  calloutData->setDetail(tr("Lat: %1\nLon: %2").arg(location.y(), 0, 'f', 6).arg(location.x(), 0, 'f', 6));
  calloutData->setVisible(true);
  return true;
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::dismissCallout()
{
  if (!m_mapView)
  {
    return;
  }

  m_mapView->calloutData()->setVisible(false);
  if (m_refreshPending)
  {
    m_refreshPending = false;
    identifyFeatures();
  }
}

void NavigateMapViewAndIdentifyFeaturesWithKeyboard::setErrorMessage(const QString& message)
{
  if (m_errorMessage == message)
  {
    return;
  }

  m_errorMessage = message;
  emit errorMessageChanged();
}
