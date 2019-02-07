// [WriteFile Name=MapRotation, Category=Maps]
// [Legal]
// Copyright 2015 Esri.

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

#ifndef MAP_ROTATION_H
#define MAP_ROTATION_H


namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapGraphicsView;
  }
}

class QLabel;
class QSlider;

#include <QWidget>

class MapRotation : public QWidget
{
  Q_OBJECT

public:
  explicit MapRotation(QWidget* parent = nullptr);
  ~MapRotation() override;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
  QSlider* m_slider = nullptr;
  QLabel* m_degrees = nullptr;
};

#endif // MAP_ROTATION_H
