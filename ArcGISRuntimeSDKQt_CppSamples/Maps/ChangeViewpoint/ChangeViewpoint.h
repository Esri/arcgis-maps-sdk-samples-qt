// [WriteFile Name=ChangeViewpoint, Category=Maps]
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

#ifndef CHANGE_VIEWPOINT_H
#define CHANGE_VIEWPOINT_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
  }
}

#include <QQuickItem>

class ChangeViewpoint : public QQuickItem
{
  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapQuickView WRITE setMapQuickView NOTIFY mapQuickViewChanged)
  Q_OBJECT

public:
  explicit ChangeViewpoint(QQuickItem* parent = nullptr);
  ~ChangeViewpoint() override = 0;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void changeViewpoint(QString viewpoint);

signals:
  void mapQuickViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapQuickView() const;
  void setMapQuickView(Esri::ArcGISRuntime::MapQuickView* mapView);

  double screenRatio() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  int m_rotationValue = 0;
};

#endif // CHANGE_VIEWPOINT_H

