// [WriteFile Name=SetMapSpatialReference, Category=Maps]
// [Legal]
// Copyright 2016 Esri.
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

#ifndef SET_MAP_SPATIAL_REFERENCE_H
#define SET_MAP_SPATIAL_REFERENCE_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class Basemap;
  class ArcGISMapImageLayer;
} // namespace Esri::ArcGISRuntime

class SetMapSpatialReference : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QStringList spatialReferenceNames READ spatialReferenceNames CONSTANT)

public:
  explicit SetMapSpatialReference(QQuickItem* parent = nullptr);
  ~SetMapSpatialReference() override;

  void componentComplete() override;
  static void init();

  QStringList spatialReferenceNames() const;

public slots:
  void setSpatialReference(int index);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Basemap* m_basemap = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_imageLayer = nullptr;

  const QList<QPair<QString, int>> m_srOptions = {{"Berghaus Star AAG", 102299},
                                                  {"Fuller", 54050},
                                                  {"New Zealand Map Grid", 27200},
                                                  {"North Pole Stereographic", 102018},
                                                  {"Peirce Quincuncial", 54090},
                                                  {"UTM Zone 10 N", 32610},
                                                  {"World Bonne", 54024},
                                                  {"World Goode Homolosine", 54052},
                                                  {"World Orthographic", 102038},
                                                  {"Web Mercator", 3857},
                                                  {"WGS 84", 4326}};
};

#endif // SET_MAP_SPATIAL_REFERENCE_H
