// [WriteFile Name=SymbolizeShapefile, Category=DisplayInformation]
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

#ifndef SYMBOLIZESHAPEFILE_H
#define SYMBOLIZESHAPEFILE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class FeatureLayer;
}
}

#include <QQuickItem>

class SymbolizeShapefile : public QQuickItem
{
  Q_OBJECT

public:
  explicit SymbolizeShapefile(QQuickItem* parent = nullptr);
  ~SymbolizeShapefile() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void updateRenderer();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
};

#endif // SYMBOLIZESHAPEFILE_H
