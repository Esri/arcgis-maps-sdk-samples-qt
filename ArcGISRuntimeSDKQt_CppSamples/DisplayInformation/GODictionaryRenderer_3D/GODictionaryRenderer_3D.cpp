// [WriteFile Name=GODictionaryRenderer_3D, Category=DisplayInformation]
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

#include "GODictionaryRenderer_3D.h"

#include <QFileInfo>
#include <QQmlProperty>

#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "DictionaryRenderer.h"
#include "GeometryEngine.h"
#include "GraphicListModel.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Surface.h"
#include "DictionarySymbolStyle.h"

using namespace Esri::ArcGISRuntime;

const QString GODictionaryRenderer_3D::FIELD_CONTROL_POINTS = "_control_points";
const QString GODictionaryRenderer_3D::FIELD_WKID = "_wkid";

GODictionaryRenderer_3D::GODictionaryRenderer_3D(QQuickItem* parent) :
  QQuickItem(parent),
  m_graphicsOverlay(new GraphicsOverlay(this))
{
  connect(m_graphicsOverlay, &GraphicsOverlay::errorOccurred, this, &GODictionaryRenderer_3D::logError);
  m_graphicsOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
}

GODictionaryRenderer_3D::~GODictionaryRenderer_3D() = default;

void GODictionaryRenderer_3D::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<GODictionaryRenderer_3D>("Esri.Samples", 1, 0, "GODictionaryRenderer_3DSample");
}

void GODictionaryRenderer_3D::logError(Error error)
{
  setErrorMessage(QString("%1: %2").arg(error.message(), error.additionalMessage()));
}

void GODictionaryRenderer_3D::componentComplete()
{
  QQuickItem::componentComplete();

  // QML properties
  m_dataPath = QQmlProperty::read(this, "dataPath").toUrl().toLocalFile();

  // Set up DictionaryRenderer
  if (!QFileInfo::exists(m_dataPath + "/styles/mil2525d.stylx"))
    setErrorMessage("mil2525d.stylx not found");

  DictionarySymbolStyle* dictionarySymbolStyle = new DictionarySymbolStyle("mil2525d", m_dataPath + "/styles/mil2525d.stylx", this);
  connect(dictionarySymbolStyle, &DictionarySymbolStyle::errorOccurred, this, &GODictionaryRenderer_3D::logError);

  DictionaryRenderer* renderer = new DictionaryRenderer(dictionarySymbolStyle, this);
  connect(renderer, &DictionaryRenderer::errorOccurred, this, &GODictionaryRenderer_3D::logError);
  m_graphicsOverlay->setRenderer(renderer);

  // Create a scene and give it to the SceneView
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  connect(m_sceneView, &SceneQuickView::errorOccurred, this, &GODictionaryRenderer_3D::logError);

  Scene* scene = new Scene(Basemap::imagery(this), this);
  connect(scene, &Scene::errorOccurred, this, &GODictionaryRenderer_3D::logError);

  Surface* surface = new Surface(this);
  connect(surface, &Surface::errorOccurred, this, &GODictionaryRenderer_3D::logError);
  surface->elevationSources()->append(
        new ArcGISTiledElevationSource(
          QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"),
          this));
  scene->setBaseSurface(surface);
  m_sceneView->setArcGISScene(scene);
  m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);

  parseXmlFile();

  emit graphicsLoaded();

  zoomToGraphics();
}

void GODictionaryRenderer_3D::parseXmlFile()
{
  bool readingMessage = false;
  QVariantMap elementValues;
  QString currentElementName;

  if (!QFileInfo::exists(m_dataPath + "/xml/Mil2525DMessages.xml"))
    setErrorMessage("xml/Mil2525DMessages.xml file is missing");

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
}

void GODictionaryRenderer_3D::createGraphic(QVariantMap rawAttributes)
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

void GODictionaryRenderer_3D::zoomToGraphics()
{
  m_bbox = GeometryEngine::project(m_bbox, m_sceneView->arcGISScene()->spatialReference());

  // Create a camera that looks at the bbox center, height 15000, pitch 70
  Camera camera(m_bbox.extent().center(), 15000, 0, 70, 0);

  m_sceneView->setViewpointCameraAndWait(camera);
}

QString GODictionaryRenderer_3D::errorMessage() const
{
  return m_errorMsg;
}

void GODictionaryRenderer_3D::setErrorMessage(const QString& msg)
{
  m_errorMsg = msg;
  qDebug() << m_errorMsg;
  emit errorMessageChanged();
}
