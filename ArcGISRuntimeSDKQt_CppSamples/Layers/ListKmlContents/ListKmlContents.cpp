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
    for (int i = 0; i < m_kmlDataset->rootNodes().size(); ++i)
    {
      m_levelNodeNames << m_kmlDataset->rootNodes()[i]->name();
      m_kmlNodesList << m_kmlDataset->rootNodes()[i];

      buildTree(m_kmlDataset->rootNodes()[i]);
    }

    // if at top node, then display children
    if (!m_kmlNodesList.isEmpty() && m_kmlNodesList[0]->parentNode() == nullptr)
    {
      displayChildren(m_kmlNodesList[0]);
    }
  });
}

QString ListKmlContents::getKmlNodeType(KmlNode *node)
{
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
  }
  return " - " + type;
}

void ListKmlContents::displayChildren(KmlNode *parentNode)
{
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(parentNode))
  {
    m_levelNodeNames.clear();
    bool lastLevel = true;

    // for current level, get names of child nodes
    for (KmlNode* node: *(container->childNodesListModel()))
    {
      QString str = node->name() + getKmlNodeType(node);

      // if node has children, add ">" to indicate further levels
      if (!node->children().isEmpty())
      {
        str.append(" >");
        lastLevel = false;
      }
      m_levelNodeNames << str;
    }

    if (lastLevel)
    {
      m_currentNode = container->childNodesListModel()->at(0);
    }
    emit levelNodeNamesChanged();
  }
}

// recursively build string to indicate node's ancestors
void ListKmlContents::buildPathLabel(KmlNode* node)
{
  if (node->parentNode() != nullptr)
  {
    buildPathLabel(node->parentNode());
    m_labelText.append(">");
  }
  m_labelText.append(node->name());
}

void ListKmlContents::displayPreviousLevel()
{
  KmlNode* parentNode = m_currentNode->parentNode();
  KmlNode* grandparentNode = parentNode->parentNode();

  if (grandparentNode != nullptr)
  {
    m_labelText.clear();
    buildPathLabel(grandparentNode);
    emit labelTextChanged();

    displayChildren(grandparentNode);
    m_currentNode = grandparentNode;
  }
  // if parent node is nullptr, then at top of tree
  else
  {
    m_labelText.clear();
    buildPathLabel(parentNode);
    emit labelTextChanged();

    displayChildren(parentNode);
    m_isTopLevel = true;
    emit isTopLevelChanged();
  }

  if (m_currentNode->name() == "")
  {
    m_isTopLevel = true;
    emit isTopLevelChanged();
  }
}

// display selected node on sceneview and show its children
void ListKmlContents::nodeSelected(QString nodeName)
{
  // find node in the list
  for (KmlNode* node : m_kmlNodesList)
  {
    if (nodeName.contains(" - "))
    {
      int ind = nodeName.indexOf(" - ");
      nodeName = nodeName.left(ind);
    }

    if (nodeName == node->name())
    {
      // update current node
      m_currentNode = node;
      m_isTopLevel = false;
      emit isTopLevelChanged();

      // set the scene view viewpoint to the extent of the selected node
      Envelope nodeExtent = node->extent();
      if (nodeExtent.isValid() && !nodeExtent.isEmpty())
      {
        m_sceneView->setViewpoint(Viewpoint(nodeExtent));
      }

      m_selectedLastLevel = node->children().isEmpty();

      // show the children of the node
      displayChildren(node);

      m_labelText.clear();
      buildPathLabel(node);
      emit labelTextChanged();

      break;
    }
  }
}

// recursively build list of all KML nodes
void ListKmlContents::buildTree(KmlNode* parentNode)
{
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(parentNode))
  {
    KmlNodeListModel* childNodes = container->childNodesListModel();

    for (KmlNode* node : *childNodes)
    {
      // some nodes have default visibility set to false
      node->setVisible(true);

      m_kmlNodesList << node;
      buildTree(node);
    }
  }

  if (KmlNetworkLink* networkLink = dynamic_cast<KmlNetworkLink*>(parentNode))
  {
    QList<KmlNode*> childNodes = networkLink->childNodes();

    for (KmlNode* node : childNodes)
    {
      // some nodes have default visibility set to false
      node->setVisible(true);

      m_kmlNodesList << node;
      buildTree(node);
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
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

QStringList ListKmlContents::levelNodeNames() const
{
  return m_levelNodeNames;
}

QString ListKmlContents::labelText() const
{
  return m_labelText;
}

bool ListKmlContents::isTopLevel() const
{
  return m_isTopLevel;
}

bool ListKmlContents::selectedLastLevel() const
{
  return m_selectedLastLevel;
}
