// [WriteFile Name=ViewshedLocation, Category=Analysis]
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

#ifndef VIEWSHEDLOCATION_H
#define VIEWSHEDLOCATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class LocationViewshed;
class AnalysisOverlay;
}
}

#include <QQuickItem>
#include <QColor>

class ViewshedLocation : public QQuickItem
{
  Q_OBJECT

public:
  explicit ViewshedLocation(QQuickItem* parent = nullptr);
  ~ViewshedLocation() override = default;

  void componentComplete() override;
  static void init();

  Q_PROPERTY(bool viewshedVisible READ isViewshedVisible WRITE setViewshedVisible NOTIFY viewshedVisibleChanged)
  Q_PROPERTY(bool frustumVisible READ isFrustumOutlineVisible WRITE setFrustumOutlineVisible NOTIFY frustumVisibleChanged)
  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
  Q_PROPERTY(QColor visibleColor READ visibleColor WRITE setVisibleColor NOTIFY visibleColorChanged)
  Q_PROPERTY(QColor obstructedColor READ obstructedColor WRITE setObstructedColor NOTIFY obstructedColorChanged)

  bool isViewshedVisible() const;
  void setViewshedVisible(bool viewshedVisible);

  bool isFrustumOutlineVisible() const;
  void setFrustumOutlineVisible(bool frustumVisible);

  double minDistance() const;
  void setMinDistance(double minDistance);

  double maxDistance() const;
  void setMaxDistance(double maxDistance);

  double horizontalAngle() const;
  void setHorizontalAngle(double horizontalAngle);

  double verticalAngle() const;
  void setVerticalAngle(double verticalAngle);

  double heading() const;
  void setHeading(double heading);

  double pitch() const;
  void setPitch(double pitch);

  QColor visibleColor() const;
  void setVisibleColor(const QColor& visibleColor);

  QColor obstructedColor() const;
  void setObstructedColor(const QColor& obstructedColor);

signals:
  void viewshedVisibleChanged();
  void frustumVisibleChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();
  void visibleColorChanged();
  void obstructedColorChanged();

private:
  void connectSignals();
  void setInitialViewpoint();
  void createViewshed(double x, double y);

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::LocationViewshed* m_locationViewshed = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;  

  bool m_viewshedVisible = true;
  bool m_frustumVisible = false;
  double m_minDistance = 50;
  double m_maxDistance = 1000;
  double m_horizontalAngle = 45;
  double m_veriticalAngle = 90;
  double m_heading = 0;
  double m_pitch = 90;
  bool m_calculating = false;
};

#endif // VIEWSHEDLOCATION_H
