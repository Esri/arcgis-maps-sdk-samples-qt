// Copyright 2022 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "ClassBreaksWithAlternateSymbols.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "ClassBreaksRenderer.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "ArcGISMapImageLayer.h"
#include "ArcGISMapImageSublayer.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

ClassBreaksWithAlternateSymbols::ClassBreaksWithAlternateSymbols(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this))
{
}

ClassBreaksWithAlternateSymbols::~ClassBreaksWithAlternateSymbols()
{
}

MapQuickView* ClassBreaksWithAlternateSymbols::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ClassBreaksWithAlternateSymbols::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  // create the feature table
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"), this);
  // create the feature layer using the feature table
  m_featureLayer = new FeatureLayer(m_featureTable, this);

  connect(m_featureLayer, &FeatureLayer::loadStatusChanged, this, [this](LoadStatus loadStatus)
  {
    loadStatus == LoadStatus::Loaded ? m_initialized = true : m_initialized = false;
  });

  // add the feature layer to the map
  m_map->operationalLayers()->append(m_featureLayer);

  emit mapViewChanged();
}

void ClassBreaksWithAlternateSymbols::createClassBreaksRenderer()
{
  // create class breaks renderer
  m_classBreaksRenderer = new ClassBreaksRenderer(this);
  m_classBreaksRenderer->setFieldName(QStringLiteral("POP2007"));

  // create and append class breaks
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(227, 235, 207), -99, 8560));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(150, 194, 191), 8560, 18109));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(97, 166, 181), 18109, 35501));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(69, 125, 150), 35501, 86100));
  m_classBreaksRenderer->classBreaks()->append(createClassBreak(QColor(41, 84, 120), 86100, 10110975));
}

// helper function to create class breaks for the renderer
ClassBreak* ClassBreaksWithAlternateSymbols::createClassBreak(const QColor &color, double min, double max)
{
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(153, 153, 153), 1.0f /*width*/, this);
  SimpleFillSymbol* sfs1 = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, color, outline, this);
  const QString description = QString("> %1 to %2").arg(QString::number(min), QString::number(max));
  const QString label = description;
  return new ClassBreak(description, label, min, max, sfs1);
}

// apply the class breaks renderer
void ClassBreaksWithAlternateSymbols::applyRenderer()
{
  if (!m_sublayer || !m_classBreaksRenderer)
    return;

  m_sublayer->setRenderer(m_classBreaksRenderer);
}

// reset to the original renderer
void ClassBreaksWithAlternateSymbols::resetRenderer()
{
  if (!m_sublayer || !m_originalRenderer)
    return;

  m_sublayer->setRenderer(m_originalRenderer);
}
