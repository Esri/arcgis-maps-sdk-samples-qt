// [WriteFile Name=GraphicsOverlayDictionaryRenderer, Category=DisplayInformation]
// [Legal]
// Copyright 2015-2016 Esri.

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

#include "GraphicsOverlayDictionaryRenderer.h"

#include <QQmlProperty>

#include "DictionaryRenderer.h"
#include "GeometryEngine.h"
#include "GraphicListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "SymbolDictionary.h"

using namespace Esri::ArcGISRuntime;

const QString GraphicsOverlayDictionaryRenderer::FIELD_CONTROL_POINTS = "_control_points";
const QString GraphicsOverlayDictionaryRenderer::FIELD_WKID = "_wkid";

GraphicsOverlayDictionaryRenderer::GraphicsOverlayDictionaryRenderer(QQuickItem* parent) :
    QQuickItem(parent),
    m_mapView(nullptr),
    m_graphicsOverlay(new GraphicsOverlay(this))
{

}

GraphicsOverlayDictionaryRenderer::~GraphicsOverlayDictionaryRenderer()
{

}

void GraphicsOverlayDictionaryRenderer::componentComplete()
{
    QQuickItem::componentComplete();

    // QML properties
    m_dataPath = QQmlProperty::read(this, "dataPath").toString();
    m_scaleFactor = QQmlProperty::read(this, "scaleFactor").toDouble();

    // Set up DictionaryRenderer
    SymbolDictionary* symbolDictionary = new SymbolDictionary("mil2525d", m_dataPath + "/styles/mil2525d.stylx", this);
    DictionaryRenderer* renderer = new DictionaryRenderer(symbolDictionary, this);
    m_graphicsOverlay->setRenderer(renderer);

    // Create a map and give it to the MapView
    m_mapView = findChild<MapQuickView*>("mapView");
    Map* map = new Map(Basemap::topographic(this), this);
    m_mapView->setMap(map);
    m_mapView->graphicsOverlays()->append(m_graphicsOverlay);

    parseXmlFile();

    emit graphicsLoaded();
}

void GraphicsOverlayDictionaryRenderer::parseXmlFile()
{
    bool readingMessage = false;
    QVariantMap elementValues;
    QString currentElementName;

    QFile xmlFile(m_dataPath + "/xml/Mil2525DMessages.xml");
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
    while (!m_xmlParser.atEnd())
    {
        m_xmlParser.readNext();
        if (m_xmlParser.name() == "message")
        {
            readingMessage = !readingMessage;
            if (readingMessage)
            {
                elementValues.clear();
            }
            else
            {
                /**
                 * Here we have a complete message that defines a military feature to display on the
                 * map. Create a graphic from its attributes.
                 */
                createGraphic(elementValues);
            }
        }
        else if (readingMessage)
        {
            if (m_xmlParser.isStartElement())
            {
                currentElementName = m_xmlParser.name().toString();
            }
            else if (m_xmlParser.isCharacters())
            {
                if (!currentElementName.isEmpty())
                {
                    QStringRef trimmedText = m_xmlParser.text().trimmed();
                    if (!trimmedText.isEmpty())
                    {
                        elementValues[currentElementName] = trimmedText.toString();
                    }
                }
            }
        }
    }
}

void GraphicsOverlayDictionaryRenderer::createGraphic(QVariantMap rawAttributes)
{
    // If _wkid was absent, use WGS 1984 (4326) by default.
    int wkid = rawAttributes.keys().contains(FIELD_WKID) ? rawAttributes[FIELD_WKID].toInt() : 4326;
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
        MultipartBuilder* builder = nullptr;
        if (pointStrings.length() >= 3 && pointStrings[0] == pointStrings[pointStrings.length() - 1])
        {
            /**
             * If there are at least three points and the first and last points are
             * equivalent, assume it's a polygon.
             */
            builder = new PolygonBuilder(sr);
        }
        else
        {
            // Apparently it's a line
            builder = new PolylineBuilder(sr);
        }
        foreach (auto pointString, pointStrings)
        {
            QStringList coords = pointString.split(",");
            if (coords.length() >= 2)
            {
                builder->addPoint(coords[0].toDouble(), coords[1].toDouble());
            }
        }
        geom = builder->toGeometry();
        delete builder;
    }

    if (!geom.isEmpty())
    {
        /**
         * Get rid of _control_points and _wkid. They are not needed in the graphic's
         * attributes.
         */
        rawAttributes.remove(FIELD_CONTROL_POINTS);
        rawAttributes.remove(FIELD_WKID);

        Graphic* graphic = new Graphic(geom, rawAttributes, this);
        m_graphicsOverlay->graphics()->append(graphic);

        m_bbox = m_bbox.isEmpty() ? geom.extent() : GeometryEngine::unionOf(m_bbox, geom).extent();
    }
}

void GraphicsOverlayDictionaryRenderer::zoomToGraphics()
{
    m_mapView->setViewpointGeometry(m_bbox.extent(), 300 * m_scaleFactor);
}
