// [WriteFile Name=ViewshedGeoElement, Category=Analysis]
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

#ifndef VIEWSHEDGEOELEMENT_H
#define VIEWSHEDGEOELEMENT_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class AnalysisOverlay;
class GeoElementViewshed;
class GraphicsOverlay;
class Graphic;
}
}

#include "Point.h"
#include "GeometryTypes.h"
#include <QQuickItem>
#include <QTimer>

class ViewshedGeoElement : public QQuickItem
{
  Q_OBJECT

public:
  explicit ViewshedGeoElement(QQuickItem* parent = nullptr);
  ~ViewshedGeoElement() override = default;

  void componentComplete() override;
  static void init();

private slots:
  void animate();

private:
  void createGraphicsOverlay();
  void createGraphic();

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::GeoElementViewshed* m_viewshed = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_tank = nullptr;
  Esri::ArcGISRuntime::Point m_waypoint;
  Esri::ArcGISRuntime::LinearUnitId m_linearUnit = Esri::ArcGISRuntime::LinearUnitId::Meters;
  Esri::ArcGISRuntime::AngularUnitId m_angularUnit = Esri::ArcGISRuntime::AngularUnitId::Degrees;
  Esri::ArcGISRuntime::GeodeticCurveType m_curveType = Esri::ArcGISRuntime::GeodeticCurveType::Geodesic;

  const QString m_headingAttr = QStringLiteral("HEADING");
  QTimer* m_timer = nullptr;
};

#endif // VIEWSHEDGEOELEMENT_H
