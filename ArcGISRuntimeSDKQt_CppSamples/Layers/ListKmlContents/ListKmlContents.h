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
  Q_PROPERTY(bool isTopLevel READ isTopLevel NOTIFY isTopLevelChanged)
  Q_PROPERTY(bool selectedLastLevel READ selectedLastLevel NOTIFY selectedLastLevelChanged)
  Q_PROPERTY(QString labelText READ labelText NOTIFY labelTextChanged)

public:
  explicit ListKmlContents(QObject* parent = nullptr);
  ~ListKmlContents();

  static void init();
  Q_INVOKABLE void nodeSelected(QString nodeName);
  Q_INVOKABLE void displayPreviousLevel();

signals:
  void sceneViewChanged();
  void nodeNamesChanged();
  void levelNodeNamesChanged();
  void isTopLevelChanged();
  void labelTextChanged();
  void selectedLastLevelChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void buildTree(Esri::ArcGISRuntime::KmlNode* parentNode);
  QStringList nodeNames() const { return m_nodeNames; }
  QStringList levelNodeNames() const { return m_levelNodeNames; }
  QString labelText() const { return m_labelText; }

  void displayChildren(Esri::ArcGISRuntime::KmlNode* node);
  void buildPathLabel(Esri::ArcGISRuntime::KmlNode* node);
  QString getKmlNodeType(Esri::ArcGISRuntime::KmlNode* node);

  bool isTopLevel() const { return m_isTopLevel; }
  bool selectedLastLevel() const { return m_selectedLastLevel; }

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;
  QAbstractListModel* m_nodeListModel = nullptr;
  QStringList m_nodeNames = {}; // shows nested tree
  QStringList m_levelNodeNames = {};
  bool m_isTopLevel = true;
  bool m_selectedLastLevel = false;
  QList<Esri::ArcGISRuntime::KmlNode*> m_kmlNodesList = {};
  Esri::ArcGISRuntime::KmlNode* m_currentNode = nullptr;
  QString m_labelText = "";
};

#endif // LISTKMLCONTENTS_H
