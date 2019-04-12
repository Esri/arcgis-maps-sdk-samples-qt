// [WriteFile Name=GroupLayers, Category=Layers]
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

#ifndef GROUPLAYERS_H
#define GROUPLAYERS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class LayerListModel;
}
}

class QAbstractListModel;

#include <QObject>

class GroupLayers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::LayerListModel* layerListModel MEMBER m_layerListModel NOTIFY layerListModelChanged)

public:
  explicit GroupLayers(QObject* parent = nullptr);
  ~GroupLayers();

  static void init();

signals:
  void sceneViewChanged();
  void layerListModelChanged();

public:
  Q_INVOKABLE Esri::ArcGISRuntime::LayerListModel* getGroupLayerListModel(int layerId);

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::LayerListModel* m_layerListModel = nullptr;
};

#endif // GROUPLAYERS_H
