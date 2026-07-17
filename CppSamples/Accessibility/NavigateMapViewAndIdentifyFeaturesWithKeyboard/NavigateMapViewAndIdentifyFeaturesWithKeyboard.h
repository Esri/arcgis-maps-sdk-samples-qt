// [WriteFile Name=NavigateMapViewAndIdentifyFeaturesWithKeyboard, Category=Accessibility]
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

#ifndef NAVIGATEMAPVIEWANDIDENTIFYFEATURESWITHKEYBOARD_H
#define NAVIGATEMAPVIEWANDIDENTIFYFEATURESWITHKEYBOARD_H

// Qt headers
#include <QObject>
#include <QPointer>
#include <QVector>

namespace Esri::ArcGISRuntime
{
  class Feature;
  class FeatureLayer;
  class FeatureQueryResult;
  class GraphicsOverlay;
  class Map;
  class MapQuickView;
  class ServiceFeatureTable;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class NavigateMapViewAndIdentifyFeaturesWithKeyboard : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(double areaOfInterestSize READ areaOfInterestSize WRITE setAreaOfInterestSize NOTIFY areaOfInterestSizeChanged)
  Q_PROPERTY(bool overflowVisible READ overflowVisible NOTIFY overflowVisibleChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit NavigateMapViewAndIdentifyFeaturesWithKeyboard(QObject* parent = nullptr);
  ~NavigateMapViewAndIdentifyFeaturesWithKeyboard() override;

  static void init();

  Q_INVOKABLE bool showCallout(int featureNumber);
  Q_INVOKABLE void dismissCallout();

signals:
  void mapViewChanged();
  void areaOfInterestSizeChanged();
  void overflowVisibleChanged();
  void errorMessageChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  double areaOfInterestSize() const;
  void setAreaOfInterestSize(double size);
  bool overflowVisible() const;
  QString errorMessage() const;

  void identifyFeatures();
  void processQueryResult(Esri::ArcGISRuntime::FeatureQueryResult* queryResult);
  void clearSelection();
  bool calloutIsVisible() const;
  bool calloutIsInView() const;
  QString featureName(Esri::ArcGISRuntime::Feature* feature) const;
  void setErrorMessage(const QString& message);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_restaurantsTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_restaurantsLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_labelsOverlay = nullptr;
  QVector<QPointer<Esri::ArcGISRuntime::Feature>> m_identifiedFeatures;
  double m_areaOfInterestSize = 200.0;
  bool m_queryInProgress = false;
  bool m_refreshPending = false;
  bool m_initialDrawCompleted = false;
  bool m_overflowVisible = false;
  QString m_errorMessage;
};

#endif // NAVIGATEMAPVIEWANDIDENTIFYFEATURESWITHKEYBOARD_H
