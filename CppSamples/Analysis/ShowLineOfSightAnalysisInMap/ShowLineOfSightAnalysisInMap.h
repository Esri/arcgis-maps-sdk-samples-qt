// [WriteFile Name=ShowLineOfSightAnalysisInMap, Category=Analysis]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef SHOWLINEOFSIGHTANALYSISINMAP_H
#define SHOWLINEOFSIGHTANALYSISINMAP_H

#include <QObject>
#include <QColor>
#include <QList>

#include "Point.h"

class QString;
class QMouseEvent;

namespace Esri::ArcGISRuntime
{
  class ContinuousField;
  class GraphicsOverlay;
  class IdentifyGraphicsOverlayResult;
  class LineOfSight;
  class Map;
  class MapQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

struct ObserverDefinition
{
  QColor color;
  Esri::ArcGISRuntime::Point position;
};

class ShowLineOfSightAnalysisInMap : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool visibilityFilter MEMBER m_visibilityFilter WRITE setVisibilityFilter NOTIFY visibilityFilterChanged)

public:
  explicit ShowLineOfSightAnalysisInMap(QObject* parent = nullptr);
  ~ShowLineOfSightAnalysisInMap() override;

  static void init();

  Q_INVOKABLE void setVisibilityFilter(bool visibilityFilter);

signals:
  void mapViewChanged();
  void visibilityFilterChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  void initialize();
  void createLineOfSightAnalysis();
  void onElevationFieldCreated(Esri::ArcGISRuntime::ContinuousField* elevation);
  void onLineOfSightEvaluated(const QList<Esri::ArcGISRuntime::LineOfSight*>& results);
  void onIdentifyObserverCompleted(Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult);
  void applyVisibilityFilter();
  void identifyObserverAt(QMouseEvent& mouseEvent);
  QString lineOfSightDetail(Esri::ArcGISRuntime::LineOfSight* result) const;

  const QString m_elevationFilePath;
  QList<ObserverDefinition> m_observers;
  bool m_isObserverIdentifyEnabled = false;
  bool m_visibilityFilter = false;
  bool m_initialized = false;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_targetGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_observersGraphicsOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_resultsGraphicsOverlay = nullptr;

  Esri::ArcGISRuntime::Point m_targetPoint;

  QList<Esri::ArcGISRuntime::LineOfSight*> m_lineOfSightResults;
};

#endif // SHOWLINEOFSIGHTANALYSISINMAP_H
