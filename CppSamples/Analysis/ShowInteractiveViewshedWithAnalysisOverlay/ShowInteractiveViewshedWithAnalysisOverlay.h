// [WriteFile Name=ShowInteractiveViewshedWithAnalysisOverlay, Category=Analysis]
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

#ifndef SHOWINTERACTIVEVIEWSHEDWITHANALYSISOVERLAY_H
#define SHOWINTERACTIVEVIEWSHEDWITHANALYSISOVERLAY_H

#include <QObject>
#include <optional>

// ArcGISRuntime headers
#include <Point.h>

namespace Esri::ArcGISRuntime
{
  class AnalysisOverlay;
  class ContinuousField;
  class ContinuousFieldFunction;
  class DiscreteFieldFunction;
  class Graphic;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
  class SimpleMarkerSymbol;
  class ViewshedFunction;
  class ViewshedParameters;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class ShowInteractiveViewshedWithAnalysisOverlay : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)
  Q_PROPERTY(double observerElevation MEMBER m_observerPointZ WRITE setObserverElevation NOTIFY observerElevationChanged)
  Q_PROPERTY(double targetHeight MEMBER m_targetHeight WRITE setTargetHeight NOTIFY targetHeightChanged)
  Q_PROPERTY(double maxRadius MEMBER m_maxRadius WRITE setMaxRadius NOTIFY maxRadiusChanged)
  Q_PROPERTY(double fieldOfView MEMBER m_fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
  Q_PROPERTY(double heading MEMBER m_headingValue WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double elevationSamplingInterval READ elevationSamplingInterval WRITE setElevationSamplingInterval NOTIFY elevationSamplingIntervalChanged)

public:
  explicit ShowInteractiveViewshedWithAnalysisOverlay(QObject* parent = nullptr);
  ~ShowInteractiveViewshedWithAnalysisOverlay() override;

  static void init();
  double elevationSamplingInterval() const;
  bool initialized() const;
  Q_INVOKABLE void dragObserver(double x, double y);
  Q_INVOKABLE void endObserverDrag(double x, double y);
  Q_INVOKABLE void startObserverDrag(double x, double y);

signals:
  void elevationSamplingIntervalChanged();
  void fieldOfViewChanged();
  void headingChanged();
  void initializedChanged();
  void mapViewChanged();
  void maxRadiusChanged();
  void observerElevationChanged();
  void targetHeightChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setInitialViewpoint();
  void addOverlays();
  void createContinuousField();
  void connectSignals();
  void initializeViewshed();
  void syncObserverGraphic();
  void updateObserverPositionFromScreen(double x, double y);
  void setObserverElevation(double value);
  void setTargetHeight(double value);
  void setMaxRadius(double value);
  void setFieldOfView(double value);
  void setHeading(double value);
  void setElevationSamplingInterval(double value);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::Point m_observerPositionPoint;
  Esri::ArcGISRuntime::Graphic* m_observerGraphic = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_observerSymbol = nullptr;
  Esri::ArcGISRuntime::ViewshedParameters* m_viewshedParameters = nullptr;
  Esri::ArcGISRuntime::ContinuousField* m_continuousField = nullptr;
  Esri::ArcGISRuntime::ViewshedFunction* m_viewshedFunction = nullptr;
  Esri::ArcGISRuntime::DiscreteFieldFunction* m_discreteViewshed = nullptr;
  const QString m_elevationFilePath;
  bool m_initialized = false;
  bool m_isDraggingObserver = false;
  double m_targetHeight = 20.0;
  double m_maxRadius = 8000.0;
  double m_fieldOfView = 150.0;
  double m_headingValue = 10.0;
  std::optional<double> m_elevationSamplingInterval;
  double m_observerPointZ = 20.0;
};

#endif // SHOWINTERACTIVEVIEWSHEDWITHANALYSISOVERLAY_H
