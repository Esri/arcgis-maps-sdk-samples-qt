// [WriteFile Name=BuildLegend, Category=DisplayInformation]
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

#ifndef BUILD_LEGEND_H
#define BUILD_LEGEND_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
  }
}

class QString;

#include <QAbstractListModel>
#include <QQuickItem>

class BuildLegend : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* legendInfoListModel MEMBER m_legendInfoListModel NOTIFY legendInfoListModelChanged)

public:
  explicit BuildLegend(QQuickItem* parent = nullptr);
  ~BuildLegend() override;

  void componentComplete() override;
  static void init();

signals:
  void legendInfoListModelChanged();

private:
  void addLayers();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QAbstractListModel* m_legendInfoListModel = nullptr;
};

#endif // BUILD_LEGEND_H

