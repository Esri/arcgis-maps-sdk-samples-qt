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

#ifndef LISTKMLCONTENTS_H
#define LISTKMLCONTENTS_H

namespace Esri
{
namespace ArcGISRuntime
{
class KmlDataset;
class KmlNode;
class KmlNodeListModel;
class Scene;
class SceneQuickView;
}
}

//class QAbstractListModel;

#include <QObject>
#include <QList>
#include <QStringList>
#include <QAbstractListModel>

class ListKmlContents : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(QStringList nodeNames READ nodeNames NOTIFY nodeNamesChanged)
  Q_PROPERTY(QStringList levelNodeNames READ levelNodeNames NOTIFY levelNodeNamesChanged)
  Q_PROPERTY(bool levelAdded READ levelAdded NOTIFY levelAddedChanged)
  Q_PROPERTY(QAbstractListModel* nodesList READ nodesList NOTIFY nodesListChanged)

public:
  explicit ListKmlContents(QObject* parent = nullptr);
  ~ListKmlContents();

  static void init();
  Q_INVOKABLE void nodeSelected(const QString nodeName);
  Q_INVOKABLE void getParents();

signals:
  void sceneViewChanged();
  void nodeNamesChanged();
  void levelAddedChanged();
  void nodesListChanged();
  void levelNodeNamesChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void buildTree(Esri::ArcGISRuntime::KmlNode* parentNode);
  QStringList nodeNames() const { return m_nodeNames; }

  void displayChildren(Esri::ArcGISRuntime::KmlNode* node);

  QStringList levelNodeNames() const { return m_levelNodeNames; }

  bool levelAdded() const { return m_levelAdded; }
  QAbstractListModel* nodesList() const { return m_nodesList; }

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;
  QAbstractListModel* m_nodeListModel = nullptr;
  QStringList m_nodeNames = {};
  QStringList m_levelNodeNames = {};
  bool m_levelAdded = false;
  QAbstractListModel* m_nodesList = nullptr;
  QList<Esri::ArcGISRuntime::KmlNode*> m_kmlNodesList = {};
  QList<Esri::ArcGISRuntime::KmlNode*> m_previousLevel = {};
  QList<int> m_indices = {};
};

#endif // LISTKMLCONTENTS_H
