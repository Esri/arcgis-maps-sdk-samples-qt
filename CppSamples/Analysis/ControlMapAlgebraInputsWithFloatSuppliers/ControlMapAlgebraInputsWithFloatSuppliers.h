// [WriteFile Name=ControlMapAlgebraInputsWithFloatSuppliers, Category=Analysis]
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

#ifndef CONTROLMAPALGEBRAINPUTSWITHFLOATSUPPLIERS_H
#define CONTROLMAPALGEBRAINPUTSWITHFLOATSUPPLIERS_H

#include <QObject>
#include <QString>

namespace Esri::ArcGISRuntime
{
  class Analysis;
  class AnalysisOverlay;
  class AnalysisViewState;
  class ContinuousField;
  class ContinuousFieldFunction;
  class FieldAnalysis;
  class FloatSupplier;
  class Map;
  class MapQuickView;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h")

class ControlMapAlgebraInputsWithFloatSuppliers : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int minimumSlope READ minimumSlope NOTIFY slopeRangeChanged)
  Q_PROPERTY(int maximumSlope READ maximumSlope NOTIFY slopeRangeChanged)
  Q_PROPERTY(bool updating READ updating NOTIFY updatingChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit ControlMapAlgebraInputsWithFloatSuppliers(QObject* parent = nullptr);
  ~ControlMapAlgebraInputsWithFloatSuppliers() override;

  static void init();

  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  int minimumSlope() const;
  int maximumSlope() const;
  bool updating() const;
  QString errorMessage() const;

  Q_INVOKABLE void setSlopeRange(int minimum, int maximum);

signals:
  void mapViewChanged();
  void slopeRangeChanged();
  void updatingChanged();
  void errorMessageChanged();

private:
  void loadElevationField();
  void onContinuousFieldCreated(Esri::ArcGISRuntime::ContinuousField* continuousField);
  void onAnalysisViewStateChanged(Esri::ArcGISRuntime::Analysis* analysis, const Esri::ArcGISRuntime::AnalysisViewState& viewState);
  void setUpdating(bool updating);
  void setErrorMessage(const QString& errorMessage);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldFunction* m_elevationFunction = nullptr;
  Esri::ArcGISRuntime::ContinuousFieldFunction* m_slopeFunction = nullptr;
  Esri::ArcGISRuntime::FloatSupplier* m_minimumSlopeSupplier = nullptr;
  Esri::ArcGISRuntime::FloatSupplier* m_maximumSlopeSupplier = nullptr;
  Esri::ArcGISRuntime::FieldAnalysis* m_slopeAnalysis = nullptr;

  int m_minimumSlope = 0;
  int m_maximumSlope = 10;
  bool m_updating = false;
  QString m_errorMessage;
};

#endif // CONTROLMAPALGEBRAINPUTSWITHFLOATSUPPLIERS_H
