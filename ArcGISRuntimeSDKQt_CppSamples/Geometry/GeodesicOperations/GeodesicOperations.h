// [WriteFile Name=GeodesicOperations, Category=Geometry]
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

#ifndef GEODESICOPERATIONS_H
#define GEODESICOPERATIONS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
}
}

#include "Point.h"
#include "Polyline.h"
#include <QQuickItem>

class GeodesicOperations : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString distanceText READ distanceText NOTIFY distanceTextChanged)

public:
  explicit GeodesicOperations(QQuickItem* parent = nullptr);
  ~GeodesicOperations() override = default;

  void componentComplete() override;
  static void init();

signals:
  void distanceTextChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Graphic* m_nycGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_pathGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_destinationGraphic = nullptr;
  QString m_distanceText;

private:
  Esri::ArcGISRuntime::Polyline pointsToPolyline(const QList<Esri::ArcGISRuntime::Point>& points);
  QString distanceText() const;
};

#endif // GEODESICOPERATIONS_H
