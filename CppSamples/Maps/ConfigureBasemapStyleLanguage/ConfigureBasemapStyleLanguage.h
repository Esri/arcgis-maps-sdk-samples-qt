// [WriteFile Name=ConfigureBasemapStyleLanguage, Category=Maps]
// [Legal]
// Copyright 2024 Esri.
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

#ifndef CONFIGUREBASEMAPSTYLELANGUAGE_H
#define CONFIGUREBASEMAPSTYLELANGUAGE_H

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime {
  class Basemap;
  class BasemapStyleParameters;
  class Map;
  class MapQuickView;
}

Q_MOC_INCLUDE("MapQuickView.h");

class ConfigureBasemapStyleLanguage : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit ConfigureBasemapStyleLanguage(QObject* parent = nullptr);
  ~ConfigureBasemapStyleLanguage() override;

  static void init();
  Q_INVOKABLE void setNewBasemapLanguage(bool global, const QString& language);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::BasemapStyleParameters* m_basemapStyleParameters = nullptr;
  Esri::ArcGISRuntime::Basemap* m_basemap = nullptr;
};

#endif // CONFIGUREBASEMAPSTYLELANGUAGE_H
