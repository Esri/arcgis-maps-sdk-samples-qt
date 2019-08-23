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

#ifndef PLAYAKMLTOUR_H
#define PLAYAKMLTOUR_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class KmlTour;
class KmlTourController;
class KmlLayer;
class KmlDataset;
class KmlNode;
class KmlNodeListModel;
}
}

#include <QObject>

class PlayAKmlTour : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(bool playButtonEnabled MEMBER m_playButtonEnabled NOTIFY playButtonEnabledChanged())
  Q_PROPERTY(bool pauseButtonEnabled MEMBER m_pauseButtonEnabled NOTIFY pauseButtonEnabledChanged())
  Q_PROPERTY(bool resetButtonEnabled MEMBER m_resetButtonEnabled NOTIFY resetButtonEnabledChanged())

public:
  explicit PlayAKmlTour(QObject* parent = nullptr);
  ~PlayAKmlTour();

  static void init();

  Q_INVOKABLE void playKmlTour();
  Q_INVOKABLE void pauseKmlTour();
  Q_INVOKABLE void resetKmlTour();

signals:
  void sceneViewChanged();
  void playButtonEnabledChanged();
  void pauseButtonEnabledChanged();
  void resetButtonEnabledChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  Esri::ArcGISRuntime::KmlTour* findFirstKMLTour(const QList<Esri::ArcGISRuntime::KmlNode*>& nodes);
  Esri::ArcGISRuntime::KmlTour* findFirstKMLTourFromListModel(const Esri::ArcGISRuntime::KmlNodeListModel& nodes);


  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::KmlTour* m_kmlTour = nullptr;
  Esri::ArcGISRuntime::KmlTourController* m_kmlTourController = nullptr;
  Esri::ArcGISRuntime::KmlLayer* m_kmlLayer = nullptr;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;

  QString m_dataPath;
  bool m_playButtonEnabled = false;
  bool m_pauseButtonEnabled = false;
  bool m_resetButtonEnabled = false;

};

#endif // PLAYAKMLTOUR_H
