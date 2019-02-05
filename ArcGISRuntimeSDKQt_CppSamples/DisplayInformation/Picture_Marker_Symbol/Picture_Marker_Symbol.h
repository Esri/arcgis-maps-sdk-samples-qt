// [WriteFile Name=Picture_Marker_Symbol, Category=DisplayInformation]
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

#ifndef PICTURE_MARKER_SYMBOL_H
#define PICTURE_MARKER_SYMBOL_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class PictureMarkerSymbol;
    class Point;
  }
}

#include <QString>
#include <QQuickItem>

class Picture_Marker_Symbol : public QQuickItem
{
  Q_OBJECT

public:
  explicit Picture_Marker_Symbol(QQuickItem* parent = nullptr);
  ~Picture_Marker_Symbol() override;

  void componentComplete() override;
  static void init();

private:
  void setWidthAndHeight(Esri::ArcGISRuntime::PictureMarkerSymbol* symbol, float size);
  void addGraphic(Esri::ArcGISRuntime::Point &point, Esri::ArcGISRuntime::PictureMarkerSymbol* symbol);

  QString m_dataPath;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
};

#endif // PICTURE_MARKER_SYMBOL_H
