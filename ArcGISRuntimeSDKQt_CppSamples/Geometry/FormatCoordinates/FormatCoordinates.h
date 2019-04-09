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

#ifndef FORMATCOORDINATES_H
#define FORMATCOORDINATES_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
}
}

#include "Point.h"

class FormatCoordinates : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView MEMBER m_mapView WRITE setMapView)
  Q_PROPERTY(QString coordinatesInDD READ coordinatesInDD NOTIFY coordinatesChanged)
  Q_PROPERTY(QString coordinatesInDMS READ coordinatesInDMS NOTIFY coordinatesChanged)
  Q_PROPERTY(QString coordinatesInUsng READ coordinatesInUsng NOTIFY coordinatesChanged)
  Q_PROPERTY(QString coordinatesInUtm READ coordinatesInUtm NOTIFY coordinatesChanged)
  Q_PROPERTY(QString strDecimalDegrees READ strDecimalDegrees CONSTANT)
  Q_PROPERTY(QString strDegreesMinutesSeconds READ strDegreesMinutesSeconds CONSTANT)
  Q_PROPERTY(QString strUsng READ strUsng CONSTANT)
  Q_PROPERTY(QString strUtm READ strUtm CONSTANT)

public:
  explicit FormatCoordinates(QObject* parent = nullptr);
  ~FormatCoordinates() override;

  static void init();
  Q_INVOKABLE void handleTextUpdate(QString textType, QString text);
  Q_INVOKABLE void handleLocationUpdate(Esri::ArcGISRuntime::Point point);

signals:
  void coordinatesChanged();

private:
  QString coordinatesInDD() const;
  QString coordinatesInDMS() const;
  QString coordinatesInUsng() const;
  QString coordinatesInUtm() const;
  QString strDecimalDegrees() const;
  QString strDegreesMinutesSeconds() const;
  QString strUsng() const;
  QString strUtm() const;

  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Point createPointFromText(const QString& textType, const QString& text) const;
  void setTextFromPoint(Esri::ArcGISRuntime::Point point);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

  QString m_coordinatesInDD;
  QString m_coordinatesInDMS;
  QString m_coordinatesInUsng;
  QString m_coordinatesInUtm;
};

#endif // FORMATCOORDINATES_H

