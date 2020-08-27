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

#include "Viewpoint.h"

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

#include <QObject>
#include <QList>
#include <QStringList>

class ListKmlContents : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)
  Q_PROPERTY(QStringList levelNodeNames READ levelNodeNames NOTIFY levelNodeNamesChanged)
  Q_PROPERTY(bool isTopLevel READ isTopLevel NOTIFY currentNodeChanged)
  Q_PROPERTY(QString labelText READ labelText NOTIFY labelTextChanged)

public:
  explicit ListKmlContents(QObject* parent = nullptr);
  ~ListKmlContents();

  static void init();
  Q_INVOKABLE void processSelectedNode(const QString& nodeName);
  Q_INVOKABLE void displayPreviousLevel();

signals:
  void sceneViewChanged();
  void levelNodeNamesChanged();
  void labelTextChanged();
  void currentNodeChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  QStringList levelNodeNames();
  QString labelText() const;
  bool isTopLevel() const;

  void buildTree(Esri::ArcGISRuntime::KmlNode* parentNode);
  QStringList buildPathLabel(Esri::ArcGISRuntime::KmlNode* node) const;
  QString getKmlNodeType(Esri::ArcGISRuntime::KmlNode* node);
  bool noGrandchildren(Esri::ArcGISRuntime::KmlNode* node) const;
  void getViewpointFromKmlViewpoint(Esri::ArcGISRuntime::KmlNode* node);
  void getAltitudeAdjustedViewpoint(Esri::ArcGISRuntime::KmlNode* node);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;
  QStringList m_levelNodeNames = {};
  QList<Esri::ArcGISRuntime::KmlNode*> m_kmlNodesList = {};
  Esri::ArcGISRuntime::KmlNode* m_currentNode = nullptr;
  bool m_needsAltitudeFixed;
  Esri::ArcGISRuntime::Viewpoint m_viewpoint;
};

#endif // LISTKMLCONTENTS_H
