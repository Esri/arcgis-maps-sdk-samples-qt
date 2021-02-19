
// Copyright 2021 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef DISPLAY3DLABELS_H
#define DISPLAY3DLABELS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class FeatureLayer;
}
}

#include <QObject>

class Display3dLabels : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit Display3dLabels(QObject* parent = nullptr);
  ~Display3dLabels() override;

signals:
  void sceneViewChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void display3dLabelsOnFeatureLayer(Esri::ArcGISRuntime::FeatureLayer* featureLayer);

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // DISPLAY3DLABELS_H
