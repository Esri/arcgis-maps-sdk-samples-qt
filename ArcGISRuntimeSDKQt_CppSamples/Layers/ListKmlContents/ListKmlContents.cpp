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

      Viewpoint vp = getViewpointFromKmlViewpoint(node);
      if (m_needsAltitudeFixed)
      {
        vp = getAltitudeAdjustedViewpoint(node, vp);
      }

      // set the scene view viewpoint to the extent of the selected node
      Envelope nodeExtent = node->extent();
      if (nodeExtent.isValid() && !nodeExtent.isEmpty())
      {
        m_sceneView->setViewpoint(Viewpoint(nodeExtent));
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

Viewpoint ListKmlContents::getViewpointFromKmlViewpoint(KmlNode* node)
{
  KmlViewpoint kmlViewpoint = node->viewpoint();
  Viewpoint viewpoint;

  if (!kmlViewpoint.isEmpty())
  {
    // altitude adjustment is needed for all but Absolute altitude mode
    m_needsAltitudeFixed = (kmlViewpoint.altitudeMode() != KmlAltitudeMode::Absolute);
    qDebug() << "Altitude mode:" << int(kmlViewpoint.altitudeMode());
    switch (kmlViewpoint.type())
    {
    case KmlViewpointType::LookAt:
      return Viewpoint(kmlViewpoint.location(), Camera(kmlViewpoint.location(),
                                                       kmlViewpoint.range(), kmlViewpoint.heading(),
                                                       kmlViewpoint.pitch(), kmlViewpoint.roll()));
    case KmlViewpointType::Camera:
      return Viewpoint(kmlViewpoint.location(), Camera(kmlViewpoint.location(),
                                                       kmlViewpoint.heading(), kmlViewpoint.pitch(),
                                                       kmlViewpoint.roll()));
    default:
      qWarning("Unexpected KmlViewpointType");
    }
  }

  qDebug("viewpoint empty");
  // if viewpoint was empty, then use node's extent
  const Envelope nodeExtent = node->extent();
  if (nodeExtent.isValid() && !nodeExtent.isEmpty())
  {
    // when no altitude is specified, assume elevation needs to be adjusted
    m_needsAltitudeFixed = true;

    if (nodeExtent.width() == 0 && nodeExtent.height() == 0)
    {
      qDebug("height and width = 0");
      // default values based on Google Earth
      return Viewpoint(nodeExtent, Camera(nodeExtent.center(), 1000, 0, 45, 0));
    }
    else
    {
      // add padding to extent
      double bufferDistance = qMax(nodeExtent.width(), nodeExtent.height()) / 20;
      Envelope bufferedExtent = Envelope(nodeExtent.xMin() - bufferDistance, nodeExtent.yMin() - bufferDistance,
                                         nodeExtent.xMax() + bufferDistance, nodeExtent.yMax() + bufferDistance,
                                         nodeExtent.zMin() - bufferDistance, nodeExtent.zMax() + bufferDistance,
                                         SpatialReference::wgs84());
      return Viewpoint(bufferedExtent);
    }
  }
  else
  {
    // can't show viewpoint
    m_needsAltitudeFixed = false;
    return Viewpoint();
  }
}

Viewpoint ListKmlContents::getAltitudeAdjustedViewpoint(KmlNode* node, Viewpoint baseViewpoint)
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
    return baseViewpoint;

  double altitude;
  Envelope lookAtExtent = static_cast<Envelope>(baseViewpoint.targetGeometry());
  Point lookAtPoint = static_cast<Point>(baseViewpoint.targetGeometry());

  if (lookAtExtent.isValid())
  {
    qDebug("extent");
//    connect(m_scene->baseSurface(), &Surface::locationToElevationCompleted, this, [this, &altitude, &altMode, &lookAtExtent, &kmlViewpoint](QUuid, double elevation)
//    {
//      altitude = elevation;
//      qDebug() << "Inside connection: " << altitude;

//      Envelope  target;
//      if (altMode == KmlAltitudeMode::ClampToGround)
//      {
//        target = Envelope(lookAtExtent.xMin(), lookAtExtent.yMin(),
//                          lookAtExtent.xMax(), lookAtExtent.yMax(),
//                          altitude, lookAtExtent.depth() + altitude,
//                          lookAtExtent.spatialReference());
//      }
//      else
//      {
//        target = Envelope(lookAtExtent.xMin(), lookAtExtent.yMin(),
//                          lookAtExtent.xMax(), lookAtExtent.yMax(),
//                          lookAtExtent.zMin() + altitude, lookAtExtent.zMax() + altitude,
//                          lookAtExtent.spatialReference());
//      }

//      if (!kmlViewpoint.isEmpty())
//      {
//        return ;
//      }
//    });
    m_scene->baseSurface()->locationToElevation(lookAtExtent.center());
    qDebug() << "after function call";
//        altitude = await MySceneView.Scene.BaseSurface.GetElevationAsync(lookAtExtent.GetCenter());
  }
  if (lookAtPoint.isValid())
    qDebug("point");

  return Viewpoint();
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
