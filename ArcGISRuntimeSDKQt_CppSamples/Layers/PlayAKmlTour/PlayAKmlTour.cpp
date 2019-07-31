// [WriteFile Name=PlayAKmlTour, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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

#include "PlayAKmlTour.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "KmlTour.h"
#include "KmlDataset.h"
#include "KmlLayer.h"
#include "KmlNode.h"
#include "KmlContainer.h"
#include "KmlNodeListModel.h"

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

PlayAKmlTour::PlayAKmlTour(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imagery(this), this)),
  m_dataPath(defaultDataPath() + "/ArcGIS/Runtime/Data")
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  m_kmlDataset = new KmlDataset(QUrl::fromLocalFile(m_dataPath + "/kml/Esri_tour.kmz"), this);
  m_kmlLayer = new KmlLayer(m_kmlDataset, this);

  connect(m_kmlLayer, &KmlLayer::doneLoading, this, [this](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << e.message();
      return;
    }

    m_scene->operationalLayers()->append(m_kmlLayer);

    m_kmlTour = findFirstKMLTour(m_kmlDataset->rootNodes());

    if (m_kmlTour)
    {

      connect(m_kmlTour, &KmlTour::tourStatusChanged, this, [this](KmlTourStatus tourStatus)
      {
        switch (tourStatus) {
          case KmlTourStatus::Completed:
          case KmlTourStatus::Initialized:
            m_playButtonEnabled = true;
            m_pauseButtonEnabled = false;
            m_resetButtonEnabled = false;
            break;
          case KmlTourStatus::Playing:
            m_playButtonEnabled = false;
            m_pauseButtonEnabled = true;
            m_resetButtonEnabled = true;
            break;
          case KmlTourStatus::Paused:
            m_playButtonEnabled = true;
            m_pauseButtonEnabled = false;
            m_resetButtonEnabled = true;
            break;
        }

        emit playButtonEnabledChanged();
        emit pauseButtonEnabledChanged();
        emit resetButtonEnabledChanged();

      });
      m_kmlTourController.setTour(m_kmlTour);
      m_playButtonEnabled = true;
      m_pauseButtonEnabled = true;
      m_resetButtonEnabled = true;
      emit playButtonEnabledChanged();
      emit pauseButtonEnabledChanged();
      emit resetButtonEnabledChanged();
    }

  });
  m_kmlLayer->load();


}

PlayAKmlTour::~PlayAKmlTour() = default;

void PlayAKmlTour::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<PlayAKmlTour>("Esri.Samples", 1, 0, "PlayAKmlTourSample");
}

SceneQuickView* PlayAKmlTour::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void PlayAKmlTour::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

void PlayAKmlTour::playKmlTour()
{
  m_kmlTourController.play();
}
void PlayAKmlTour::pauseKmlTour()
{
  m_kmlTourController.pause();
}
void PlayAKmlTour::resetKmlTour()
{
  m_kmlTourController.reset();
}

//void PlayAKmlTour::updateButtonStatus(const KmlTourStatus& tourStatus)
//{
//  if(tourStatus == 2)
//    qDebug() << "it works";
//}

KmlTour* PlayAKmlTour::findFirstKMLTour(const QList<KmlNode*>& nodes)
{
  for (KmlNode* node : nodes)
  {
    qDebug() << node->name();
    qDebug() << nodes.length();
    if (node->kmlNodeType() == KmlNodeType::KmlTour)
      return dynamic_cast<KmlTour*>(node);
    else if ((node->kmlNodeType() == KmlNodeType::KmlDocument) || (node->kmlNodeType() == KmlNodeType::KmlFolder))
      return findFirstKMLTour(dynamic_cast<KmlContainer*>(node)->childNodes());
//      return findFirstKMLTourFromListModel(dynamic_cast<KmlContainer*>(node)->childNodesListModel());
  }
  return nullptr;
}


// not working yet
KmlTour* PlayAKmlTour::findFirstKMLTourFromListModel(const KmlNodeListModel& nodes)
{
  for (KmlNode* node : nodes)
  {
    qDebug() << "list model func: " << node->name();
    qDebug() << "list model func: " << nodes.size();
    if (node->kmlNodeType() == KmlNodeType::KmlTour)
      return dynamic_cast<KmlTour*>(node);
    else if ((node->kmlNodeType() == KmlNodeType::KmlDocument) || (node->kmlNodeType() == KmlNodeType::KmlFolder))
//      return findFirstKMLTourFromListModel(dynamic_cast<KmlContainer*>(node)->childNodesListModel());
      return nullptr;
  }
  return nullptr;
}
