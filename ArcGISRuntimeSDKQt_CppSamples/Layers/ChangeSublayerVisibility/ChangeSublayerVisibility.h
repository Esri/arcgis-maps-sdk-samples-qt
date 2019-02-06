// [WriteFile Name=ChangeSublayerVisibility, Category=Layers]
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

#ifndef CHANGE_SUBLAYER_VISIBILITY_H
#define CHANGE_SUBLAYER_VISIBILITY_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class ArcGISMapImageLayer;
  }
}

#include <QAbstractListModel>
#include <QQuickItem>

class ChangeSublayerVisibility : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* sublayerModel READ sublayerModel NOTIFY sublayerModelChanged)

public:
  explicit ChangeSublayerVisibility(QQuickItem* parent = nullptr);
  ~ChangeSublayerVisibility() override;

  void componentComplete() override;
  static void init();

signals:
  void sublayerModelChanged();

private:
  QAbstractListModel* sublayerModel();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_mapImageLayer = nullptr;
  QAbstractListModel* m_sublayerModel = nullptr;
};

#endif // CHANGE_SUBLAYER_VISIBILITY_H

