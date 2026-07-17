// [WriteFile Name=AnalyzeTerrainSuitabilityWithSlopeAndAspect, Category=Analysis]
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

#ifndef ANALYZETERRAINSUITABILITYWITHSLOPEANDASPECT_H
#define ANALYZETERRAINSUITABILITYWITHSLOPEANDASPECT_H

// Qt headers
#include <QColor>
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class AnalysisOverlay;
  class BooleanFieldFunction;
  class ContinuousField;
  class ContinuousFieldFunction;
  class FieldAnalysis;
  class Map;
  class MapQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class AnalyzeTerrainSuitabilityWithSlopeAndAspect : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(SiteScenario selectedScenario MEMBER m_selectedScenario WRITE setSelectedScenario NOTIFY selectedScenarioChanged)

public:
  enum SiteScenario
  {
    GentleSouthFacingSlopes,
    SteepWestAndNorthFacingSlopes
  };
  Q_ENUM(SiteScenario)

  explicit AnalyzeTerrainSuitabilityWithSlopeAndAspect(QObject* parent = nullptr);
  ~AnalyzeTerrainSuitabilityWithSlopeAndAspect() override;

  static void init();

signals:
  void mapViewChanged();
  void selectedScenarioChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void loadElevationField();
  void onContinuousFieldCreated(Esri::ArcGISRuntime::ContinuousField* continuousField);
  void buildAnalysisForScenario(SiteScenario scenario);
  void applyFieldAnalysisVisibility();
  Esri::ArcGISRuntime::FieldAnalysis* createScenarioAnalysis(float slopeMin,
                                                             float slopeMax,
                                                             float aspectStart,
                                                             float aspectEnd,
                                                             float elevationMin,
                                                             float elevationMax,
                                                             const QColor& color);
  Esri::ArcGISRuntime::BooleanFieldFunction* createScenarioFieldFunction(float slopeMin,
                                                                         float slopeMax,
                                                                         float aspectStart,
                                                                         float aspectEnd,
                                                                         float elevationMin,
                                                                         float elevationMax);

  void setSelectedScenario(SiteScenario scenario);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldFunction* m_elevationFieldFunction = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldFunction* m_slopeFunction = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldFunction* m_aspectFunction = nullptr;
  Esri::ArcGISRuntime::BooleanFieldFunction* m_aboveSeaLevelSelection = nullptr;
  Esri::ArcGISRuntime::FieldAnalysis* m_gentleSouthFacingSlopesAnalysis = nullptr;
  Esri::ArcGISRuntime::FieldAnalysis* m_steepWestAndNorthFacingSlopesAnalysis = nullptr;

  SiteScenario m_selectedScenario = GentleSouthFacingSlopes;
};

#endif // ANALYZETERRAINSUITABILITYWITHSLOPEANDASPECT_H
