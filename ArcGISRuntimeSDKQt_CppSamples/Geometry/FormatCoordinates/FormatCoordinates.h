// [WriteFile Name=FormatCoordinates, Category=Geometry]
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

#ifndef DISPLAY_MAP_H
#define DISPLAY_MAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
}
}

#include <QQuickItem>

#include "Point.h"

class FormatCoordinates : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString coordinatesInDD READ coordinatesInDD NOTIFY coordinatesInDDChanged)
  Q_PROPERTY(QString coordinatesInDMS READ coordinatesInDMS NOTIFY coordinatesInDMSChanged)
  Q_PROPERTY(QString coordinatesInUsng READ coordinatesInUsng NOTIFY coordinatesInUsngChanged)
  Q_PROPERTY(QString coordinatesInUtm READ coordinatesInUtm NOTIFY coordinatesInUtmChanged)

public:
  explicit FormatCoordinates(QQuickItem* parent = nullptr);
  ~FormatCoordinates();

  void componentComplete() Q_DECL_OVERRIDE;
  static void init();
  Q_INVOKABLE void handleTextUpdate(QString textType, QString text);
  Q_INVOKABLE void handleLocationUpdate(Esri::ArcGISRuntime::Point point);

signals:
  void coordinatesInDDChanged();
  void coordinatesInDMSChanged();
  void coordinatesInUsngChanged();
  void coordinatesInUtmChanged();

private:
  QString coordinatesInDD();
  QString coordinatesInDMS();
  QString coordinatesInUsng();
  QString coordinatesInUtm();

private:
  void connectSignals();
  Esri::ArcGISRuntime::Point createPointFromText(QString textType, QString text);
  void setTextFromPoint(Esri::ArcGISRuntime::Point point);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

  const double m_startLongitude = -117.195723;
  const double m_startLatitude = 34.056195;

  QString m_coordinatesInDD;
  QString m_coordinatesInDMS;
  QString m_coordinatesInUsng;
  QString m_coordinatesInUtm;
};

#endif // DISPLAY_MAP_H

