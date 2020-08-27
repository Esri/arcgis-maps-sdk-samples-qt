// [WriteFile Name=ListKmlContents, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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

#include "ListKmlContents.h"

#include "ArcGISTiledElevationSource.h"
#include "KmlContainer.h"
#include "KmlDataset.h"
#include "KmlLayer.h"
#include "KmlNetworkLink.h"
#include "KmlNode.h"
#include "KmlNodeListModel.h"
#include "Scene.h"
#include "SceneQuickView.h"

#include <QDir>
#include <algorithm>

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

ListKmlContents::ListKmlContents(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imageryWithLabels(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  const QString filepath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/esri_test_data.kmz";

  // create new KML layer
  m_kmlDataset = new KmlDataset(filepath, this);
  KmlLayer* kmlLayer = new KmlLayer(m_kmlDataset, this);
  m_scene->operationalLayers()->append(kmlLayer);

  connect(m_kmlDataset, &KmlDataset::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }

    // recursively build tree to display KML contents
    for (KmlNode* node : m_kmlDataset->rootNodes())
    {
      m_kmlNodesList << node;
      buildTree(node);
    }

    // if at top node, then display children
    if (!m_kmlNodesList.isEmpty() && m_kmlNodesList[0]->parentNode() == nullptr)
    {
      m_currentNode = m_kmlNodesList[0];
      emit currentNodeChanged();
      emit levelNodeNamesChanged();
    }
  });
}

QString ListKmlContents::getKmlNodeType(KmlNode *node)
{
  if (node == nullptr)
    return "";

  QString type = "";
  switch (node->kmlNodeType()) {
  case KmlNodeType::KmlDocument:
    type = "KmlDocument";
    break;
  case KmlNodeType::KmlFolder:
    type = "KmlFolder";
    break;
  case KmlNodeType::KmlNetworkLink:
    type = "KmlNetworkLink";
    break;
  case KmlNodeType::KmlPlacemark:
    type = "KmlPlacemark";
    break;
  case KmlNodeType::KmlPhotoOverlay:
    type = "KmlPhotoOverlay";
    break;
  case KmlNodeType::KmlGroundOverlay:
    type = "KmlGroundOverlay";
    break;
  case KmlNodeType::KmlScreenOverlay:
    type = "KmlScreenOverlay";
    break;
  case KmlNodeType::KmlTour:
    type = "KmlTour";
    break;
  default:
    return "";
  }
  return type;
}

// recursively build string to indicate node's ancestors
QStringList ListKmlContents::buildPathLabel(KmlNode* node) const
{
  if (node != nullptr)
    return buildPathLabel(node->parentNode()) << node->name();
  else
    return QStringList {};
}

void ListKmlContents::displayPreviousLevel()
{
  KmlNode* parentNode = m_currentNode->parentNode();
  if (parentNode == nullptr)
    return;

  KmlNode* grandparentNode = parentNode->parentNode();

  if (grandparentNode != nullptr)
  {
    m_currentNode = grandparentNode;
    emit currentNodeChanged();
    emit labelTextChanged();
    emit levelNodeNamesChanged();
  }
  // if grandparent node is nullptr, then at top of tree
  else
  {
    m_currentNode = parentNode;
    emit currentNodeChanged();
    emit labelTextChanged();
    emit levelNodeNamesChanged();
  }
}

// display selected node on sceneview and show its children
void ListKmlContents::processSelectedNode(const QString& nodeName)
{
  // extract the node's name from the string, formatted "name - nodeType"
  QString extractedNodeName = nodeName;
  if (nodeName.contains(" - "))
  {
    int ind = nodeName.lastIndexOf(" - ");
    extractedNodeName = nodeName.left(ind);
  }

  // find node in the list
  for (KmlNode* node : m_kmlNodesList)
  {
    if (extractedNodeName == node->name())
    {
      // update current node
      m_currentNode = node;
      emit currentNodeChanged();
      emit labelTextChanged();

      m_viewpoint = Viewpoint();
      getViewpointFromKmlViewpoint(node);
      if (m_needsAltitudeFixed)
      {
        getAltitudeAdjustedViewpoint(node);
      }
      else
      {
        if (!m_viewpoint.isEmpty() && !m_viewpoint.targetGeometry().isEmpty())
          m_sceneView->setViewpoint(m_viewpoint);
      }

      // reset m_lastLevel before levelNodeNames() is called
      emit levelNodeNamesChanged();

      // if displaying end-nodes, change m_currentNode to first end-node for correct behavior of back button
      if (noGrandchildren(m_currentNode))
      {
        m_currentNode = static_cast<KmlContainer*>(m_currentNode)->childNodesListModel()->at(0);
        emit currentNodeChanged();
      }
      break;
    }
  }
}

void ListKmlContents::getViewpointFromKmlViewpoint(KmlNode* node)
{
  const KmlViewpoint kmlViewpoint = node->viewpoint();

  if (!kmlViewpoint.isEmpty())
  {
    // altitude adjustment is needed for all but Absolute altitude mode
    m_needsAltitudeFixed = (kmlViewpoint.altitudeMode() != KmlAltitudeMode::Absolute);

    switch (kmlViewpoint.type())
    {
    case KmlViewpointType::LookAt:
      m_viewpoint = Viewpoint(kmlViewpoint.location(), Camera(kmlViewpoint.location(), kmlViewpoint.range(),
                                                              kmlViewpoint.heading(), kmlViewpoint.pitch(), kmlViewpoint.roll()));
      return;
    case KmlViewpointType::Camera:
      m_viewpoint = Viewpoint(kmlViewpoint.location(), Camera(kmlViewpoint.location(),
                                                              kmlViewpoint.heading(), kmlViewpoint.pitch(), kmlViewpoint.roll()));
      return;
    default:
      qWarning("Unexpected KmlViewpointType");
      return;
    }
  }

  // if viewpoint was empty, then use node's extent
  const Envelope nodeExtent = node->extent();
  if (nodeExtent.isValid() && !nodeExtent.isEmpty())
  {
    // when no altitude is specified, assume elevation needs to be adjusted
    m_needsAltitudeFixed = true;

    if (nodeExtent.width() == 0 && nodeExtent.height() == 0)
    {
      // default values: distance = 1000m, pitch = 45 degrees
      m_viewpoint = Viewpoint(nodeExtent);
      m_sceneView->setViewpointCameraAndWait(Camera(nodeExtent.center(), 1000, 0, 45, 0));
      return;
    }
    else
    {
      // add padding to extent
      double bufferDistance = qMax(nodeExtent.width(), nodeExtent.height()) / 20;
      Envelope bufferedExtent = Envelope(nodeExtent.xMin() - bufferDistance, nodeExtent.yMin() - bufferDistance,
                                         nodeExtent.xMax() + bufferDistance, nodeExtent.yMax() + bufferDistance,
                                         nodeExtent.zMin() - bufferDistance, nodeExtent.zMax() + bufferDistance,
                                         SpatialReference::wgs84());
      m_viewpoint = Viewpoint(bufferedExtent);
    }
  }
  else
  {
    // can't show viewpoint
    m_needsAltitudeFixed = false;
    m_viewpoint = Viewpoint();
  }
}

void ListKmlContents::getAltitudeAdjustedViewpoint(KmlNode* node)
{
  // assume altitude mode is clamp-to-ground if not specified
  KmlAltitudeMode altMode = KmlAltitudeMode::ClampToGround;
  KmlViewpoint kmlViewpoint = node->viewpoint();

  if (!kmlViewpoint.isEmpty())
  {
    altMode = kmlViewpoint.altitudeMode();
  }

  // if altitude mode is Absolute, viewpoint doesn't need adjustment
  if (altMode == KmlAltitudeMode::Absolute)
  {
    m_sceneView->setViewpoint(m_viewpoint);
    return;
  }

  const Envelope lookAtExtent = geometry_cast<Envelope>(m_viewpoint.targetGeometry());
  const Point lookAtPoint = geometry_cast<Point>(m_viewpoint.targetGeometry());

  if (lookAtExtent.isValid())
  {
    m_scene->baseSurface()->locationToElevation(lookAtExtent.center());
  }
  else if (lookAtPoint.isValid())
  {
    m_scene->baseSurface()->locationToElevation(lookAtPoint);
  }
}

// recursively build list of all KML nodes
void ListKmlContents::buildTree(KmlNode* parentNode)
{
  auto addNode = [this](KmlNode* node)
  {
    // some nodes have default visibility set to false
    node->setVisible(true);

    m_kmlNodesList << node;
    buildTree(node);
  };

  if (KmlContainer* container = dynamic_cast<KmlContainer*>(parentNode))
  {
    const KmlNodeListModel& childNodes = *container->childNodesListModel();
    std::for_each(std::begin(childNodes), std::end(childNodes), addNode);
  }
  else if (KmlNetworkLink* networkLink = dynamic_cast<KmlNetworkLink*>(parentNode))
  {
    const QList<KmlNode*> childNodes = networkLink->childNodes();
    std::for_each(std::begin(childNodes), std::end(childNodes), addNode);
  }
}

ListKmlContents::~ListKmlContents() = default;

void ListKmlContents::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ListKmlContents>("Esri.Samples", 1, 0, "ListKmlContentsSample");
}

SceneQuickView* ListKmlContents::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ListKmlContents::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  connect(m_scene->baseSurface(), &Surface::locationToElevationCompleted, this, [this](QUuid, double elevation)
  {
    // assume altitude mode is clamp-to-ground if not specified
    KmlAltitudeMode altMode = KmlAltitudeMode::ClampToGround;
    KmlViewpoint kmlViewpoint = m_currentNode->viewpoint();

    if (!kmlViewpoint.isEmpty())
    {
      altMode = kmlViewpoint.altitudeMode();
    }
    // if altitude mode is Absolute, viewpoint doesn't need adjustment
    if (altMode == KmlAltitudeMode::Absolute)
      return;

    const Envelope lookAtExtent = geometry_cast<Envelope>(m_viewpoint.targetGeometry());
    const Point lookAtPoint = geometry_cast<Point>(m_viewpoint.targetGeometry());

    if (lookAtExtent.isValid())
    {
      Envelope  target;
      if (altMode == KmlAltitudeMode::ClampToGround)
      {
        // if depth of extent is 0, add 100m to the elevation to get zMax
        if (lookAtExtent.depth() == 0)
        {
          target = Envelope(lookAtExtent.xMin(), lookAtExtent.yMin(),
                            lookAtExtent.xMax(), lookAtExtent.yMax(),
                            elevation, elevation + 100,
                            lookAtExtent.spatialReference());
        }
        else
        {
          // add buffer of 1m to zMax
          target = Envelope(lookAtExtent.xMin(), lookAtExtent.yMin(),
                            lookAtExtent.xMax(), lookAtExtent.yMax(),
                            elevation, lookAtExtent.depth() + elevation + 1,
                            lookAtExtent.spatialReference());
        }
      }
      else
      {
        target = Envelope(lookAtExtent.xMin(), lookAtExtent.yMin(),
                          lookAtExtent.xMax(), lookAtExtent.yMax(),
                          lookAtExtent.zMin() + elevation, lookAtExtent.zMax() + elevation,
                          lookAtExtent.spatialReference());
      }

      // if node has viewpoint specified, return adjusted geometry with adjusted camera
      if (!kmlViewpoint.isEmpty())
      {
        m_sceneView->setViewpointCameraAndWait(m_viewpoint.camera().elevate(elevation));
        m_viewpoint = Viewpoint(target);
        m_sceneView->setViewpoint(m_viewpoint);
        return;
      }
      else
      {
        m_viewpoint = Viewpoint(target);
        m_sceneView->setViewpoint(m_viewpoint);
        return;
      }
    }
    else if (lookAtPoint.isValid())
    {
      Point target;
      if (altMode == KmlAltitudeMode::ClampToGround)
      {
        target = Point(lookAtPoint.x(), lookAtPoint.y(), elevation, lookAtPoint.spatialReference());
      }
      else
      {
        target = Point(lookAtPoint.x(), lookAtPoint.y(), lookAtPoint.z() + elevation, lookAtPoint.spatialReference());
      }

      // set the viewpoint using the adjusted target
      if (!kmlViewpoint.isEmpty())
      {
        m_sceneView->setViewpointCameraAndWait(m_viewpoint.camera().elevate(elevation));
        m_viewpoint = Viewpoint(target);
        m_sceneView->setViewpoint(m_viewpoint);
        return;
      }
      else
      {
        // use default values to set camera: distance = 1000m, pitch = 45 degrees
        m_viewpoint = Viewpoint(target);
        m_sceneView->setViewpoint(m_viewpoint);
        m_sceneView->setViewpointCameraAndWait(Camera(target, 1000, 0, 45, 0));
        return;
      }
    }
  });

  emit sceneViewChanged();
}

QStringList ListKmlContents::levelNodeNames()
{
  if (m_currentNode == nullptr)
    return {};

  QStringList nodeNames = {};

  // if node is not a container, m_levelNodeNames will be unchanged
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(m_currentNode))
  {
    // for current level, get names of child nodes
    for (KmlNode* node: *(container->childNodesListModel()))
    {
      QString str = node->name() + " - " + getKmlNodeType(node);

      // if node has children, add ">" to indicate further levels
      if (!node->children().isEmpty())
      {
        str.append(" >");
      }
      nodeNames << str;
    }
    m_levelNodeNames = nodeNames;
  }
  return m_levelNodeNames;
}

bool ListKmlContents::noGrandchildren(KmlNode *currentNode) const
{
  bool hasNoGrandchildren = false;

  if (KmlContainer* container = dynamic_cast<KmlContainer*>(currentNode))
  {
    hasNoGrandchildren = true;

    // for current level, get names of child nodes
    for (KmlNode* node: *(container->childNodesListModel()))
    {
      // if node has children, add ">" to indicate further levels
      if (!node->children().isEmpty())
      {
        hasNoGrandchildren = false;
      }
    }
  }

  return hasNoGrandchildren;
}

QString ListKmlContents::labelText() const
{
  return buildPathLabel(m_currentNode).join(" > ");
}

bool ListKmlContents::isTopLevel() const
{
  if (m_currentNode == nullptr)
    return true;
  else if (m_currentNode->parentNode() == nullptr)
    return true;
  else if (m_currentNode->name() == "")
    return true;
  else
    return false;
}
