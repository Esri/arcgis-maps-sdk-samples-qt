// [WriteFile Name=GraphicsOverlayDictionaryRenderer, Category=DisplayInformation]
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

#ifndef GraphicsOverlayDictionaryRenderer_H
#define GraphicsOverlayDictionaryRenderer_H

#include <QQuickItem>
#include <QXmlStreamReader>

#include "Envelope.h"

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class GraphicsOverlay;
    class MapQuickView;
  }
}

class GraphicsOverlayDictionaryRenderer : public QQuickItem
{
  Q_OBJECT

public:
  explicit GraphicsOverlayDictionaryRenderer(QQuickItem* parent = nullptr);
  ~GraphicsOverlayDictionaryRenderer();

  void componentComplete() Q_DECL_OVERRIDE;
  static void init();

signals:
  void graphicsLoaded();

public slots:
  void zoomToGraphics();

private:
  static const QString FIELD_CONTROL_POINTS;
  static const QString FIELD_WKID;

  void parseXmlFile();
  void createGraphic(QVariantMap rawAttributes);

  QString m_dataPath;
  QXmlStreamReader m_xmlParser;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Envelope m_bbox;
};

#endif // GraphicsOverlayDictionaryRenderer_H
