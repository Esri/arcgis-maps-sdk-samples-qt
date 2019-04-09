// [WriteFile Name=Symbols, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef SYMBOLS_H
#define SYMBOLS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Scene;
    class SceneQuickView;
  }
}

#include <QQuickItem>

class Symbols : public QQuickItem
{
  Q_OBJECT

public:
  explicit Symbols(QQuickItem* parent = nullptr);
  ~Symbols() override;

  void componentComplete() override;
  static void init();

private:
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
};

#endif // SYMBOLS_H

