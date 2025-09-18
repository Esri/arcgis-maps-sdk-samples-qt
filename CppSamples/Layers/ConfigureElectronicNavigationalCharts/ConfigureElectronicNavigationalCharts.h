// [WriteFile Name=ConfigureElectronicNavigationalCharts, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef CONFIGUREELECTRONICNAVIGATIONALCHARTS_H
#define CONFIGUREELECTRONICNAVIGATIONALCHARTS_H

// ArcGIS Maps SDK headers
#include "HydrographyTypes.h"

// Qt headers
#include <QObject>

class QMouseEvent;

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class EncExchangeSet;
  class Geometry;
  class EncDisplaySettings;
  class EncMarinerSettings;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h")

class ConfigureElectronicNavigationalCharts : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::EncColorScheme colorScheme READ colorScheme WRITE setColorScheme NOTIFY colorSchemeChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::EncAreaSymbolizationType areaSymbolizationType READ areaSymbolizationType WRITE setAreaSymbolizationType NOTIFY
               areaSymbolizationTypeChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::EncPointSymbolizationType pointSymbolizationType READ pointSymbolizationType WRITE setPointSymbolizationType NOTIFY
               pointSymbolizationTypeChanged)

public:
  enum class ColorScheme
  {
    ColorSchemeDay = static_cast<int>(Esri::ArcGISRuntime::EncColorScheme::Day),
    ColorSchemeDusk = static_cast<int>(Esri::ArcGISRuntime::EncColorScheme::Dusk),
    ColorSchemeNight = static_cast<int>(Esri::ArcGISRuntime::EncColorScheme::Night)
  };
  Q_ENUM(ColorScheme)

  enum class AreaSymbolizationType
  {
    AreaSymbolizationTypePlain = static_cast<int>(Esri::ArcGISRuntime::EncAreaSymbolizationType::Plain),
    AreaSymbolizationTypeSymbolized = static_cast<int>(Esri::ArcGISRuntime::EncAreaSymbolizationType::Symbolized)
  };
  Q_ENUM(AreaSymbolizationType)

  enum class PointSymbolizationType
  {
    PointSymbolizationTypeSimplified = static_cast<int>(Esri::ArcGISRuntime::EncPointSymbolizationType::Simplified),
    PointSymbolizationTypePaperChart = static_cast<int>(Esri::ArcGISRuntime::EncPointSymbolizationType::PaperChart)
  };
  Q_ENUM(PointSymbolizationType)

  explicit ConfigureElectronicNavigationalCharts(QObject* parent = nullptr);
  ~ConfigureElectronicNavigationalCharts();

  static void init();

  Q_INVOKABLE void updateDisplaySettings();
  Q_INVOKABLE void clearAllSelections();

signals:
  void mapViewChanged();
  void colorSchemeChanged();
  void areaSymbolizationTypeChanged();
  void pointSymbolizationTypeChanged();

private slots:
  void onGeoViewTapped(QMouseEvent& mouseEvent);

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::EncColorScheme colorScheme() const;
  void setColorScheme(Esri::ArcGISRuntime::EncColorScheme colorScheme);

  Esri::ArcGISRuntime::EncAreaSymbolizationType areaSymbolizationType() const;
  void setAreaSymbolizationType(Esri::ArcGISRuntime::EncAreaSymbolizationType areaSymbolizationType);

  Esri::ArcGISRuntime::EncPointSymbolizationType pointSymbolizationType() const;
  void setPointSymbolizationType(Esri::ArcGISRuntime::EncPointSymbolizationType pointSymbolizationType);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::EncExchangeSet* m_encExchangeSet = nullptr;
  QList<Esri::ArcGISRuntime::Geometry> m_extents;

  Esri::ArcGISRuntime::EncColorScheme m_colorScheme;
  Esri::ArcGISRuntime::EncAreaSymbolizationType m_areaSymbolizationType;
  Esri::ArcGISRuntime::EncPointSymbolizationType m_pointSymbolizationType;
};

#endif // CONFIGUREELECTRONICNAVIGATIONALCHARTS_H
