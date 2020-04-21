// [WriteFile Name=AnimateImagesWithImageOverlay, Category=Scenes]
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

#ifndef ANIMATEIMAGESWITHIMAGEOVERLAY_H
#define ANIMATEIMAGESWITHIMAGEOVERLAY_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class ImageFrame;
class ImageOverlay;
}
}

#include <QDir>
#include <QObject>

class QTimer;

#include "Envelope.h"

class AnimateImagesWithImageOverlay : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(bool isStopped MEMBER m_isStopped NOTIFY isStoppedChanged)

public:
  explicit AnimateImagesWithImageOverlay(QObject* parent = nullptr);
  ~AnimateImagesWithImageOverlay();

  static void init();

  Q_INVOKABLE void setTimerInterval(int value);
  Q_INVOKABLE void setOpacity(float value);
  Q_INVOKABLE void startTimer();
  Q_INVOKABLE void stopTimer();

signals:
  void sceneViewChanged();
  void isStoppedChanged();

private slots:
  void animateImageFrames();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void setupImageOverlay();

  Esri::ArcGISRuntime::Envelope m_pacificSouthwestEnvelope;
  Esri::ArcGISRuntime::ImageOverlay* m_imageOverlay = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  int m_index = 0;
  bool m_isStopped = true;
  QTimer* m_timer = nullptr;
  const QString m_dataPath = "";
  const QDir m_dir;
  const QStringList m_images;
  const int m_imagesSize = 0;
};

#endif // ANIMATEIMAGESWITHIMAGEOVERLAY_H
