// [WriteFile Name=GODictionaryRenderer, Category=DisplayInformation]
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

// C++ API headers
#include "Envelope.h"

// Qt headers
#include <QQuickItem>
#include <QXmlStreamReader>

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class GraphicsOverlay;
    class MapQuickView;
  }
}

class GODictionaryRenderer : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool graphicsLoaded READ graphicsLoaded NOTIFY graphicsLoadedChanged)

public:
  explicit GODictionaryRenderer(QQuickItem* parent = nullptr);
  ~GODictionaryRenderer() override;

  void componentComplete() override;
  static void init();

signals:
  void graphicsLoadedChanged();

private:
  static const QString FIELD_CONTROL_POINTS;
  static const QString FIELD_WKID;

  bool graphicsLoaded() const;
  void parseXmlFile();
  void createGraphic(QVariantMap rawAttributes);
  void zoomToGraphics();

  bool m_graphicsLoaded = false;
  QString m_dataPath;
  QXmlStreamReader m_xmlParser;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
};

#endif // GraphicsOverlayDictionaryRenderer_H
