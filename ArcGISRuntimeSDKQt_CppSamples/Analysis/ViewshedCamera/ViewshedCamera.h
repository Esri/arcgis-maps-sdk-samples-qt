// [WriteFile Name=ViewshedCamera, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef VIEWSHEDCAMERA_H
#define VIEWSHEDCAMERA_H

namespace Esri
{
namespace ArcGISRuntime
{
class Scene;
class SceneQuickView;
class AnalysisOverlay;
class LocationViewshed;
}
}

#include <QQuickItem>

class ViewshedCamera : public QQuickItem
{
  Q_OBJECT

public:
  explicit ViewshedCamera(QQuickItem* parent = nullptr);
  ~ViewshedCamera() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void calculateViewshed();

private:
  void setInitialViewpoint();

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::LocationViewshed* m_viewshed = nullptr;
};

#endif // VIEWSHEDCAMERA_H
