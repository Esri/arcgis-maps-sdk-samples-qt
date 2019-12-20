// [WriteFile Name=GODictionaryRenderer, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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

#include "GODictionaryRenderer.h"

#include "DictionaryRenderer.h"
#include "GraphicListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolygonBuilder.h"
#include "MultipointBuilder.h"
#include "DictionarySymbolStyle.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

  #ifdef Q_OS_ANDROID
    dataPath = "/sdcard";
  #elif defined Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  #else
    dataPath = QDir::homePath();
  #endif

    return dataPath;
  }
} // namespace

const QString GODictionaryRenderer::FIELD_CONTROL_POINTS = QStringLiteral("_control_points");
const QString GODictionaryRenderer::FIELD_WKID = QStringLiteral("_wkid");

GODictionaryRenderer::GODictionaryRenderer(QQuickItem* parent) :
  QQuickItem(parent),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data")
{
}

GODictionaryRenderer::~GODictionaryRenderer() = default;

void GODictionaryRenderer::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GODictionaryRenderer>("Esri.Samples", 1, 0, "GODictionaryRendererSample");
}

bool GODictionaryRenderer::graphicsLoaded() const
{
  return m_graphicsLoaded;
}

void GODictionaryRenderer::componentComplete()
{
  QQuickItem::componentComplete();    

  //! [Apply Dictionary Renderer Graphics Overlay Cpp]
  // Create graphics overlay
  m_graphicsOverlay = new GraphicsOverlay(this);

  // Create dictionary renderer and apply to the graphics overlay
  const QString specType = QStringLiteral("mil2525d");
  const QString styleLocation = m_dataPath + "/styles/arcade_style/mil2525d.stylx";
  DictionarySymbolStyle* dictionarySymbolStyle = DictionarySymbolStyle::createFromFile(styleLocation, this);
  DictionaryRenderer* renderer = new DictionaryRenderer(dictionarySymbolStyle, this);
  m_graphicsOverlay->setRenderer(renderer);
  //! [Apply Dictionary Renderer Graphics Overlay Cpp]

  // Create a map and give it to the MapView
  m_mapView = findChild<MapQuickView*>("mapView");
  m_map = new Map(Basemap::topographic(this), this);

  parseXmlFile();
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

  // The GraphicsOverlay will not have a valid extent until it is part of
  // a MapQuickView with a valid spatial referenence
  connect(m_mapView, &MapQuickView::spatialReferenceChanged, this, [this]()
  {
    zoomToGraphics();
  });

  m_mapView->setMap(m_map);
}

void GODictionaryRenderer::parseXmlFile()
{
  bool readingMessage = false;
  QVariantMap elementValues;
  QString currentElementName;

  QFile xmlFile(m_dataPath + "/xml/arcade_style/Mil2525DMessages.xml");
  // Open the file for reading
  if (xmlFile.isOpen())
  {
    xmlFile.reset();
  }
  else
  {
    xmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
  }
  m_xmlParser.setDevice(&xmlFile);

  // Traverse the XML in a loop
  while (!m_xmlParser.atEnd())
  {
    m_xmlParser.readNext();

    // Is this the start or end of a message element?
    if (m_xmlParser.name() == "message")
    {
      if (!readingMessage)
      {
        // This is the start of a message element.
        elementValues.clear();
      }
      else
      {
        /**
                 * This is the end of a message element. Here we have a complete message that defines
                 * a military feature to display on the map. Create a graphic from its attributes.
                 */
        createGraphic(elementValues);
      }
      // Either we just started reading a message, or we just finished reading a message.
      readingMessage = !readingMessage;
    }
    // Are we already inside a message element?
    else if (readingMessage)
    {
      // Is this the start of an element inside a message?
      if (m_xmlParser.isStartElement())
      {
        // Remember which element we're reading
        currentElementName = m_xmlParser.name().toString();
      }
      // Is this text?
      else if (m_xmlParser.isCharacters())
      {
        // Is this text inside an element?
        if (!currentElementName.isEmpty())
        {
          // Get the text and store it as the current element's value
          QStringRef trimmedText = m_xmlParser.text().trimmed();
          if (!trimmedText.isEmpty())
          {
            elementValues[currentElementName] = trimmedText.toString();
          }
        }
      }
    }
  }

  emit graphicsLoadedChanged();
}

void GODictionaryRenderer::createGraphic(QVariantMap rawAttributes)
{
  // If _wkid was absent, use WGS 1984 (4326) by default.
  int wkid = rawAttributes.count(FIELD_WKID) > 0 ? rawAttributes[FIELD_WKID].toInt() : 4326;
  SpatialReference sr(wkid);
  Geometry geom;
  QStringList pointStrings = rawAttributes[FIELD_CONTROL_POINTS].toString().split(";");
  if (pointStrings.length() == 1)
  {
    // It's a point
    QStringList coords = pointStrings[0].split(",");
    geom = Point(coords[0].toDouble(), coords[1].toDouble(), sr);
  }
  else {
    // It's a multipoint
    MultipointBuilder* builder = new MultipointBuilder(sr, this);
    PointCollection* collection = new PointCollection(sr, this);
    for (const QString& pointString : pointStrings)
    {
      QStringList coords = pointString.split(",");
      if (coords.length() >= 2)
        collection->addPoint(coords[0].toDouble(), coords[1].toDouble());
    }
    builder->setPoints(collection);
    geom = builder->toGeometry();
  }

  if (!geom.isEmpty())
  {
    // Get rid of _control_points and _wkid. They are not needed in the graphic's
    // attributes.
    rawAttributes.remove(FIELD_CONTROL_POINTS);
    rawAttributes.remove(FIELD_WKID);

    Graphic* graphic = new Graphic(geom, rawAttributes, this);
    m_graphicsOverlay->graphics()->append(graphic);
  }
}

void GODictionaryRenderer::zoomToGraphics()
{
  if (m_graphicsOverlay)
    m_mapView->setViewpointGeometry(m_graphicsOverlay->extent(), 20);
}
