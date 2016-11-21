// [WriteFile Name=ChangeViewpoint, Category=Maps]
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

#ifndef CHANGE_VIEWPOINT_H
#define CHANGE_VIEWPOINT_H

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapGraphicsView;
  }
}

class QComboBox;

#include <QWidget>

class ChangeViewpoint : public QWidget
{
  Q_OBJECT

public:
  explicit ChangeViewpoint(QWidget* parent = 0);
  ~ChangeViewpoint();

private slots:
  void changeToNewViewpoint(int);

private:
  double screenRatio() const;

  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView;
  QComboBox* m_viewpointCombo;
  int m_rotationValue;
};

#endif // CHANGE_VIEWPOINT_H
