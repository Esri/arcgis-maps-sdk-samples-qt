// [WriteFile Name=LineOfSightLocation, Category=Analysis]
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

#ifndef LINEOFSIGHTLOCATION_H
#define LINEOFSIGHTLOCATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class AnalysisOverlay;
class LocationLineOfSight;
}
}

#include <QQuickItem>

class LineOfSightLocation : public QQuickItem
{
  Q_OBJECT

public:
  explicit LineOfSightLocation(QQuickItem* parent = nullptr);
  ~LineOfSightLocation() override = default;

  void componentComplete() override;
  static void init();

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::LocationLineOfSight* m_lineOfSight = nullptr;

  bool m_calculating = false;

  void setInitialViewpoint();
  void connectSignals();
  void createLineOfSight();
};

#endif // LINEOFSIGHTLOCATION_H
