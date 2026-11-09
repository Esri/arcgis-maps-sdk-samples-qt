// [WriteFile Name=GetFieldFunctionValueFromAPoint, Category=Analysis]
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

#ifndef GETFIELDFUNCTIONVALUEFROMAPOINT_H
#define GETFIELDFUNCTIONVALUEFROMAPOINT_H

// Qt headers
#include <QObject>
#include <QSet>
#include <QString>

// STL headers
#include <Point.h>
#include <optional>

namespace Esri::ArcGISRuntime
{
  class Analysis;
  class AnalysisOverlay;
  class BooleanFieldFunction;
  class BooleanFieldSampleFunction;
  class ContinuousField;
  class ContinuousFieldFunction;
  class ContinuousFieldSampleFunction;
  class FloatSupplier;
  class Graphic;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h")

class GetFieldFunctionValueFromAPoint : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(double aircraftAltitude READ aircraftAltitude WRITE setAircraftAltitude NOTIFY aircraftAltitudeChanged)
  Q_PROPERTY(bool analysisUpdating READ analysisUpdating NOTIFY analysisUpdatingChanged)

public:
  explicit GetFieldFunctionValueFromAPoint(QObject* parent = nullptr);
  ~GetFieldFunctionValueFromAPoint() override;

  static void init();
  double aircraftAltitude() const;
  bool analysisUpdating() const;
  bool initialized() const;
  bool loading() const;

signals:
  void aircraftAltitudeChanged();
  void analysisUpdatingChanged();
  void initializedChanged();
  void loadingChanged();
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setAircraftAltitude(double altitude);
  void addOverlays();
  void connectSignals();
  bool createFieldAnalysis(Esri::ArcGISRuntime::ContinuousFieldFunction* elevationFunction,
                           Esri::ArcGISRuntime::BooleanFieldFunction* clearanceFunction);
  void createContinuousField();
  void initializeFieldAnalysis();
  void onContinuousFieldCreated(Esri::ArcGISRuntime::ContinuousField* continuousField);
  void finishSampling(quint64 requestId, bool failed = false);
  void requestSample(const Esri::ArcGISRuntime::Point& point);
  void sampleCurrentPoint();
  void setLoading(bool loading);
  QString sampleDetail(std::optional<float> elevation, std::optional<bool> hasClearance) const;
  void updateCallout(const QString& detail);

  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::ContinuousField* m_continuousField = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldSampleFunction* m_elevationSampleFunction = nullptr;
  Esri::ArcGISRuntime::BooleanFieldSampleFunction* m_clearanceSampleFunction = nullptr;
  Esri::ArcGISRuntime::FloatSupplier* m_aircraftAltitudeSupplier = nullptr;
  Esri::ArcGISRuntime::Graphic* m_samplePointGraphic = nullptr;
  Esri::ArcGISRuntime::Point m_samplePoint;
  const QString m_elevationFilePath;
  quint64 m_sampleRequestId = 0;
  double m_aircraftAltitude = 2000.0;
  bool m_mapLoaded = false;
  bool m_initialized = false;
  bool m_loading = true;
  bool m_isSampling = false;
  bool m_isDraggingPoint = false;
  bool m_analysisUpdating = false;
  QSet<Esri::ArcGISRuntime::Analysis*> m_updatingAnalyses;
};

#endif // GETFIELDFUNCTIONVALUEFROMAPOINT_H
