// [WriteFile Name=ListKmlContents, Category=Layers]
// [Legal]
// Copyright 2020 Esri.
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
#include "ListKmlContents.h"

// ArcGIS Maps SDK headers
#include "ArcGISTiledElevationSource.h"
#include "Camera.h"
#include "ElevationSourceListModel.h"
#include "Envelope.h"
#include "Error.h"
#include "KmlContainer.h"
#include "KmlDataset.h"
#include "KmlLayer.h"
#include "KmlNetworkLink.h"
#include "KmlNode.h"
#include "KmlNodeListModel.h"
#include "KmlViewpoint.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "Point.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SpatialReference.h"
#include "Surface.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QStandardPaths>
#include <QUuid>

// C++ Headers
#include <algorithm>
#include <utility>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }
} // namespace

ListKmlContents::ListKmlContents(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_scene(new Scene(BasemapStyle::ArcGISImagery, this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource =
    new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  const QString filepath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/esri_kml_sample_data.kmz";

  // create new KML layer
  m_kmlDataset = new KmlDataset(filepath, this);
  KmlLayer* kmlLayer = new KmlLayer(m_kmlDataset, this);
  m_scene->operationalLayers()->append(kmlLayer);

  connect(m_kmlDataset, &KmlDataset::doneLoading, this, [this](const Error& loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }

    // recursively build tree to display KML contents
    const QList<KmlNode*> nodes = m_kmlDataset->rootNodes();
    buildTree(nodes);

    // if at top node, then display children
    if (!m_kmlNodesList.isEmpty() && m_kmlNodesList[0]->parentNode() == nullptr)
    {
      m_currentNode = m_kmlNodesList[0];
      emit currentNodeChanged();
      emit levelNodeNamesChanged();
    }
  });
}

QString ListKmlContents::getKmlNodeType(KmlNode* node)
{
  if (node == nullptr)
  {
    return "";
  }

  QString type = "";
  switch (node->kmlNodeType())
  {
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
  {
    return buildPathLabel(node->parentNode()) << node->name();
  }
  return QStringList{};
}

void ListKmlContents::displayPreviousLevel()
{
  KmlNode* parentNode = m_currentNode->parentNode();
  if (parentNode == nullptr)
  {
    return;
  }

  KmlNode* grandparentNode = parentNode->parentNode();

  if (grandparentNode != nullptr)
  {
    m_currentNode = grandparentNode;
  }
  // if grandparent node is nullptr, then at top of tree
  else
  {
    m_currentNode = parentNode;
  }
  emit currentNodeChanged();
  emit labelTextChanged();
  emit levelNodeNamesChanged();
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
  for (KmlNode* node : std::as_const(m_kmlNodesList))
  {
    if (extractedNodeName == node->name())
    {
      // update current node
      m_currentNode = node;
      emit currentNodeChanged();
      emit labelTextChanged();
      emit levelNodeNamesChanged();

      // if displaying end-nodes, change m_currentNode to first end-node for correct behavior of back button
      if (KmlContainer* container = dynamic_cast<KmlContainer*>(m_currentNode))
      {
        if (!hasGrandchildren(container))
        {
          m_currentNode = container->childNodesListModel()->at(0);
          emit currentNodeChanged();
        }
      }

      // If node has viewpoint, set view to that viewpoint. Otherwise, create a viewpoint to encompass node's geometry
      KmlViewpoint nodeViewpoint = node->viewpoint();

      if (nodeViewpoint.isEmpty())
      {
        // if no viewpoint, set view to encompass node's geometry
        const Envelope nodeExtent = node->extent();
        if (nodeExtent.isValid())
        {
          if (nodeExtent.width() == 0 && nodeExtent.height() == 0)
          {
            nodeViewpoint = KmlViewpoint::createLookAtViewpoint(nodeExtent.center(), 0, 45, 1000, KmlAltitudeMode::RelativeToGround);
          }
          else
          {
            nodeViewpoint = KmlViewpoint::createWithViewpoint(Viewpoint(nodeExtent));
          }
        }
      }
      // If nodeViewpoint is still empty, this method is a no-op and will not change the current view
      setSceneViewCameraFromKmlViewpoint(nodeViewpoint);
      break;
    }
  }
}

void ListKmlContents::setSceneViewCameraFromKmlViewpoint(const KmlViewpoint& viewpoint)
{
  if (viewpoint.isEmpty())
  {
    return;
  }

  const Camera camera = [&viewpoint]() -> Camera
  {
    switch (viewpoint.type())
    {
      case KmlViewpointType::LookAt:
        return Camera(viewpoint.location(), viewpoint.range(), viewpoint.heading(), viewpoint.pitch(), viewpoint.roll());
      case KmlViewpointType::Camera:
        return Camera(viewpoint.location(), viewpoint.heading(), viewpoint.pitch(), viewpoint.roll());
      default:
        qWarning("Unexpected KmlViewpointType");
        return Camera();
    }
  }();

  if (!camera.location().isValid())
  {
    qWarning() << "Invalid camera location in KML viewpoint";
    return;
  }

  if (viewpoint.altitudeMode() != KmlAltitudeMode::Absolute)
  {
    // Get elevation at camera location
    m_scene->baseSurface()
      ->applyElevationAsync(viewpoint.location())
      .then(this, [camera, this](const Geometry& locationOnSurface)
    {
      const double deltaElevation = Point(locationOnSurface).z();
      const Camera raisedCamera = camera.elevate(deltaElevation);
      m_sceneView->setViewpointCameraAsync(raisedCamera, 1);
    });
  }
  else
  {
    m_sceneView->setViewpointCameraAsync(camera, 1);
  }
}

// recursively build list of all KML nodes
void ListKmlContents::buildTree(const QList<KmlNode*>& rootNodes)
{
  for (KmlNode* node : std::as_const(rootNodes))
  {
    // some nodes have default visibility set to false
    node->setVisible(true);
    m_kmlNodesList << node;

    if (KmlContainer* container = dynamic_cast<KmlContainer*>(node))
    {
      QList<KmlNode*> childNodes;
      for (KmlNode* node : *(container->childNodesListModel()))
      {
        childNodes << node;
      }
      if (!childNodes.isEmpty())
      {
        buildTree(childNodes);
      }
    }
    else if (KmlNetworkLink* networkLink = dynamic_cast<KmlNetworkLink*>(node))
    {
      const QList<KmlNode*> childNodes = networkLink->childNodes();
      buildTree(childNodes);
    }
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
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

QStringList ListKmlContents::levelNodeNames()
{
  if (m_currentNode == nullptr)
  {
    return {};
  }

  QStringList nodeNames = {};

  // if node is not a container, m_levelNodeNames will be unchanged
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(m_currentNode))
  {
    // for current level, get names of child nodes
    for (KmlNode* node : *(container->childNodesListModel()))
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

bool ListKmlContents::hasGrandchildren(KmlContainer* container) const
{
  return std::any_of(container->childNodesListModel()->begin(), container->childNodesListModel()->end(), [](KmlNode* node)
  {
    if (KmlContainer* container = dynamic_cast<KmlContainer*>(node))
    {
      return !container->children().isEmpty();
    }
    else if (KmlNetworkLink* networkLink = dynamic_cast<KmlNetworkLink*>(node))
    {
      return !networkLink->childNodes().isEmpty();
    }
    return false;
  });
}

QString ListKmlContents::labelText() const
{
  return buildPathLabel(m_currentNode).join(" > ");
}

bool ListKmlContents::isTopLevel() const
{
  return (!m_currentNode || !m_currentNode->parentNode());
}
