// [WriteFile Name=UpdateLabelsAndSymbolsToScaleForVisualAccessibility, Category=Accessibility]
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

#ifndef UPDATELABELSANDSYMBOLSTOSCALEFORVISUALACCESSIBILITY_H
#define UPDATELABELSANDSYMBOLSTOSCALEFORVISUALACCESSIBILITY_H

// Qt headers
#include <QObject>
#include <QUrl>

// STL headers
#include <memory>

namespace AccessibilitySample
{
  class SystemTextScaleMonitor;
}

namespace Esri::ArcGISRuntime
{
  class FeatureLayer;
  class Map;
  class MapQuickView;
  class MultilayerPointSymbol;
} // namespace Esri::ArcGISRuntime

class QMouseEvent;

Q_MOC_INCLUDE("MapQuickView.h")

class UpdateLabelsAndSymbolsToScaleForVisualAccessibility : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QUrl symbolSwatchUrl READ symbolSwatchUrl NOTIFY symbolSwatchUrlChanged)

public:
  explicit UpdateLabelsAndSymbolsToScaleForVisualAccessibility(QObject* parent = nullptr);
  ~UpdateLabelsAndSymbolsToScaleForVisualAccessibility() override;

  static void init();

signals:
  void mapViewChanged();
  void symbolSwatchUrlChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QUrl symbolSwatchUrl() const;
  void setSymbolSize(double symbolSize);
  void updateSymbolSize();

  void loadRestaurantSymbol();
  void addLabels();
  void identifyRestaurant(const QMouseEvent& mouseEvent);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_restaurantsLayer = nullptr;
  Esri::ArcGISRuntime::MultilayerPointSymbol* m_restaurantSymbol = nullptr;
  std::shared_ptr<AccessibilitySample::SystemTextScaleMonitor> m_systemTextScaleMonitor;
  double m_symbolSize = 24;
  QUrl m_symbolSwatchUrl;
};

#endif // UPDATELABELSANDSYMBOLSTOSCALEFORVISUALACCESSIBILITY_H
