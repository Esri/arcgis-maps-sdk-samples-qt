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
#include "MultilayerPolygonSymbol.h"
#include "SymbolReferenceProperties.h"
#include "SimpleFillSymbol.h"

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

//  createClassBreaksRenderer();
  setupClassBreaksRenderer_();

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

QList<ClassBreak*> ClassBreaksWithAlternateSymbols::createClassBreaks_(MultilayerPolygonSymbol* mlSym1, const QList<Symbol*>& alternateSymbols)
{
  // create first class break for primary symbol
  auto cb1 = new ClassBreak("CB1", "CB1", 0, 6, mlSym1, alternateSymbols, this);

  // create second class break with scale based symbol (no alternates)
  auto sym2 = new SimpleFillSymbol(this);
  sym2->setColor(QColor("green"));

  auto mlSym2 = sym2->toMultilayerSymbol();

  mlSym2->setReferenceProperties(new SymbolReferenceProperties(m_symbolScale1, m_symbolScale2, this));
  auto cb2 = new ClassBreak("CB2", "CB2", 6, 10, mlSym2, this);

  // create third class break, not scale enabled or with alternate symbols
  auto sym3 = new SimpleFillSymbol(this);
  sym3->setColor(QColor("blue"));
  auto mlSym3 = sym3->toMultilayerSymbol();
  auto cb3 = new ClassBreak("CB3", "CB3", 10, 12, mlSym3, this);

  return {cb1, cb2, cb3};
}

void ClassBreaksWithAlternateSymbols::setupClassBreaksRenderer_()
{
  auto cbr = new ClassBreaksRenderer(this);
  cbr->setFieldName("SmallIntF");

  auto defaultSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("cyan"), this);
  cbr->setDefaultSymbol(defaultSymbol->toMultilayerSymbol());
  cbr->setMinValue(0);

  // create primary symbol to be used by first class break
  auto sfs = new SimpleFillSymbol(this);
  sfs->setColor(QColor("red"));

  // add support for alternate symbols for primary symbol
  auto mlSym1 = sfs->toMultilayerSymbol();
  mlSym1->setReferenceProperties(new SymbolReferenceProperties(0, m_symbolScale1, this));

  auto alternateSymbols = createAlternateSymbols_(sfs);
//  addToRenderer_<ClassBreak*>(createClassBreaks_(mlSym1, alternateSymbols)), cbr);

  QList<ClassBreak*> class_breaks = createClassBreaks_(mlSym1, alternateSymbols);

  cbr->classBreaks()->append(qobject_cast<ClassBreak*>(class_breaks.at(0)));
  cbr->classBreaks()->append(qobject_cast<ClassBreak*>(class_breaks.at(1)));
  cbr->classBreaks()->append(qobject_cast<ClassBreak*>(class_breaks.at(2)));

  m_sublayer->setRenderer(cbr);
}

QList<Symbol*> ClassBreaksWithAlternateSymbols::createAlternateSymbols_(const SimpleFillSymbol* sfs)
{
  // create first alternate symbol
  auto mlAltSym1 = sfs->toMultilayerSymbol();
  mlAltSym1->setColor(QColor("yellow"));
  mlAltSym1->setReferenceProperties(new SymbolReferenceProperties(m_symbolScale1, m_symbolScale2, this));

  // create second alternate symbol
  auto mlAltSym2 = sfs->toMultilayerSymbol();
  mlAltSym2->setColor(QColor("magenta"));
  mlAltSym2->setReferenceProperties(new SymbolReferenceProperties(m_symbolScale2, 0, this));

  return {mlAltSym1, mlAltSym2};
}
