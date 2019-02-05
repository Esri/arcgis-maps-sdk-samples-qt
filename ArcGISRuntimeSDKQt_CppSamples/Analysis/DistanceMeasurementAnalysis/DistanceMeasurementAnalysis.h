// [WriteFile Name=DistanceMeasurementAnalysis, Category=Analysis]
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

#ifndef DISTANCEMEASUREMENTANALYSIS_H
#define DISTANCEMEASUREMENTANALYSIS_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
class LocationDistanceMeasurement;
}
}

#include <QQuickItem>

class DistanceMeasurementAnalysis : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString directDistance READ directDistance NOTIFY directDistanceChanged)
  Q_PROPERTY(QString horizontalDistance READ horizontalDistance NOTIFY horizontalDistanceChanged)
  Q_PROPERTY(QString verticalDistance READ verticalDistance NOTIFY verticalDistanceChanged)

public:
  explicit DistanceMeasurementAnalysis(QQuickItem* parent = nullptr);
  ~DistanceMeasurementAnalysis() override = default;

  void componentComplete() override;
  static void init();  
  Q_INVOKABLE void setUnits(const QString& unitName);

signals:
  void directDistanceChanged();
  void horizontalDistanceChanged();
  void verticalDistanceChanged();

private:
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::LocationDistanceMeasurement* m_distanceAnalysis = nullptr;
  QString m_directDistance;
  QString m_horizontalDistance;
  QString m_verticalDistance;
  bool m_isPressAndHold = false;
  bool m_isNavigating = false;
  QString directDistance() const;
  QString horizontalDistance() const;
  QString verticalDistance() const;
  void connectSignals();
};

#endif // DISTANCEMEASUREMENTANALYSIS_H
