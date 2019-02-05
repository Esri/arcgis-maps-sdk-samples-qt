// [WriteFile Name=ClipGeometry, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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

#ifndef CLIPGEOMETRY_H
#define CLIPGEOMETRY_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class GraphicsOverlay;
class SimpleLineSymbol;
class SimpleFillSymbol;
}
}

#include <QQuickItem>
#include "Envelope.h"

class ClipGeometry : public QQuickItem
{
  Q_OBJECT

public:
  explicit ClipGeometry(QQuickItem* parent = nullptr);
  ~ClipGeometry() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void clipAreas();

private:
  void createGraphics();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Graphic* m_coloradoGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_outsideGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_intersectingGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_insideGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* clippedGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_coloradoOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_envelopesOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_clippedAreasOverlay = nullptr;
  Esri::ArcGISRuntime::Geometry m_clippedGeometry;
  Esri::ArcGISRuntime::Envelope m_coloradoEnvelope;
  Esri::ArcGISRuntime::Envelope m_outsideEnvelope;
  Esri::ArcGISRuntime::Envelope m_insideEnvelope;
  Esri::ArcGISRuntime::Envelope m_intersectingEnvelope;
  Esri::ArcGISRuntime::SimpleFillSymbol* m_coloradoFill = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_coloradoOutline = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* envelopeOutline = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_envelopeGraphic = nullptr;
};

#endif // CLIPGEOMETRY_H
