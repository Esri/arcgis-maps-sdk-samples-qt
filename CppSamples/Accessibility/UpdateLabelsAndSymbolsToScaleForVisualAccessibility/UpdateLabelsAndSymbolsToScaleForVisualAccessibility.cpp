// [WriteFile Name=UpdateLabelsAndSymbolsToScaleForVisualAccessibility, Category=Accessibility]
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
#include "UpdateLabelsAndSymbolsToScaleForVisualAccessibility.h"

// ArcGIS Maps SDK headers
#include "AttributeListModel.h"
#include "CalloutData.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureLayer.h"
#include "GeometryEngine.h"
#include "IdentifyLayerResult.h"
#include "LabelDefinition.h"
#include "LabelDefinitionListModel.h"
#include "LabelingTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MultilayerPointSymbol.h"
#include "Point.h"
#include "ServiceFeatureTable.h"
#include "SimpleLabelExpression.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolStyle.h"
#include "TextSymbol.h"
#include "Viewpoint.h"

// qt_common headers
#include "SystemTextScaleMonitor/SystemTextScaleMonitor.h"

// Qt headers
#include <QBuffer>
#include <QFuture>
#include <QImage>
#include <QMouseEvent>

using namespace Esri::ArcGISRuntime;

UpdateLabelsAndSymbolsToScaleForVisualAccessibility::UpdateLabelsAndSymbolsToScaleForVisualAccessibility(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISLightGray, this))
{
  m_systemTextScaleMonitor = AccessibilitySample::SystemTextScaleMonitor::sharedInstance();
  connect(m_systemTextScaleMonitor.get(), &AccessibilitySample::SystemTextScaleMonitor::textScaleChanged, this, [this](float)
  {
    updateSymbolSize();
  });
  updateSymbolSize();

  m_map->setInitialViewpoint(Viewpoint(Point(-117.1793, 34.0556, SpatialReference::wgs84()), 2500));

  // Create a feature layer of restaurants in Redlands and add it to the map
  ServiceFeatureTable* restaurantsTable =
    new ServiceFeatureTable(QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/redlands_food/FeatureServer/0"), this);
  m_restaurantsLayer = new FeatureLayer(restaurantsTable, this);
  m_map->operationalLayers()->append(m_restaurantsLayer);

  // Once the feature layer has loaded, label the restaurants with their names
  connect(m_restaurantsLayer, &FeatureLayer::doneLoading, this, [this](const Error& error)
  {
    if (error.isEmpty())
    {
      addLabels();
    }
  });

  loadRestaurantSymbol();
}

UpdateLabelsAndSymbolsToScaleForVisualAccessibility::~UpdateLabelsAndSymbolsToScaleForVisualAccessibility() = default;

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<UpdateLabelsAndSymbolsToScaleForVisualAccessibility>("Esri.Samples", 1, 0,
                                                                       "UpdateLabelsAndSymbolsToScaleForVisualAccessibilitySample");
}

MapQuickView* UpdateLabelsAndSymbolsToScaleForVisualAccessibility::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::mouseClicked, this, &UpdateLabelsAndSymbolsToScaleForVisualAccessibility::identifyRestaurant);

  emit mapViewChanged();
}

QUrl UpdateLabelsAndSymbolsToScaleForVisualAccessibility::symbolSwatchUrl() const
{
  return m_symbolSwatchUrl;
}

double UpdateLabelsAndSymbolsToScaleForVisualAccessibility::symbolSize() const
{
  return m_symbolSize;
}

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::setSymbolSize(double symbolSize)
{
  if (symbolSize <= 0 || qFuzzyCompare(m_symbolSize, symbolSize))
  {
    return;
  }

  m_symbolSize = symbolSize;
  if (m_restaurantSymbol)
  {
    m_restaurantSymbol->setSize(m_symbolSize);
  }

  emit symbolSizeChanged();
}

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::updateSymbolSize()
{
  const double textScale = m_systemTextScaleMonitor->textScale();
  setSymbolSize(24.0 * textScale);
}

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::loadRestaurantSymbol()
{
  // Get the restaurant symbol from Esri's 2D point symbol web style
  SymbolStyle* symbolStyle = new SymbolStyle("Esri2DPointSymbolsStyle", nullptr, this);
  symbolStyle->fetchSymbolAsync({"restaurant"}, this)
    .then(this, [this](Symbol* symbol)
  {
    MultilayerPointSymbol* restaurantSymbol = qobject_cast<MultilayerPointSymbol*>(symbol);
    if (!restaurantSymbol)
    {
      return;
    }

    m_restaurantSymbol = restaurantSymbol;
    restaurantSymbol->setSize(m_symbolSize);

    // Display the restaurants with the symbol
    m_restaurantsLayer->setRenderer(new SimpleRenderer(restaurantSymbol, this));

    // Create a high resolution swatch of the symbol for the legend so it stays sharp as the text size grows
    restaurantSymbol->createSwatchAsync(3).then(this, [this](const QImage& swatch)
    {
      QByteArray swatchBytes;
      QBuffer buffer(&swatchBytes);
      buffer.open(QIODevice::WriteOnly);
      swatch.save(&buffer, "PNG");

      m_symbolSwatchUrl = QUrl("data:image/png;base64," + swatchBytes.toBase64());
      emit symbolSwatchUrlChanged();
    });
  });
}

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::addLabels()
{
  // Create a text symbol for the labels
  TextSymbol* textSymbol = new TextSymbol(this);
  textSymbol->setColor(QColor(31, 35, 40));
  textSymbol->setHaloColor(Qt::white);
  textSymbol->setHaloWidth(2);
  textSymbol->setSize(12);

  // Label each restaurant with its name, above the restaurant symbol
  LabelDefinition* labelDefinition = new LabelDefinition(new SimpleLabelExpression("[name]", this), textSymbol, this);
  labelDefinition->setDeconflictionStrategy(LabelDeconflictionStrategy::None);
  labelDefinition->setPlacement(LabelingPlacement::PointAboveCenter);

  m_restaurantsLayer->labelDefinitions()->append(labelDefinition);
  m_restaurantsLayer->setLabelsEnabled(true);
}

void UpdateLabelsAndSymbolsToScaleForVisualAccessibility::identifyRestaurant(const QMouseEvent& mouseEvent)
{
  // Clear any previous selection and callout
  m_restaurantsLayer->clearSelection();
  m_mapView->calloutData()->setVisible(false);

  // Identify at most one restaurant near the selected screen position
  m_mapView->identifyLayerAsync(m_restaurantsLayer, mouseEvent.position(), 12, false, 1, this)
    .then(this, [this](IdentifyLayerResult* result)
  {
    if (!result)
    {
      return;
    }

    result->deleteLater();

    if (result->geoElements().isEmpty())
    {
      return;
    }

    Feature* restaurant = static_cast<Feature*>(result->geoElements().constFirst());
    const QString name = restaurant->attributes()->attributeValue("name").toString().trimmed();

    // Project the restaurant location to WGS 84 to get its latitude and longitude
    const Point location(restaurant->geometry());
    const Point wgs84Location(GeometryEngine::project(location, SpatialReference::wgs84()));

    // Select the restaurant and show its name and coordinates in a callout
    m_restaurantsLayer->selectFeature(restaurant);

    CalloutData* calloutData = m_mapView->calloutData();
    calloutData->setLocation(location);
    calloutData->setTitle(name.isEmpty() ? tr("Restaurant") : name);
    calloutData->setDetail(tr("Lat: %1, Lon: %2").arg(wgs84Location.y(), 0, 'f', 5).arg(wgs84Location.x(), 0, 'f', 5));
    calloutData->setVisible(true);
  });
}
