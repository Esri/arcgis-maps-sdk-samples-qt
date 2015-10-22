// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SET_MAP_SPATIAL_REFERENCE
#define SET_MAP_SPATIAL_REFERENCE

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapView;
  class Basemap;
  class ArcGISMapImageLayer;
  }
}

#include <QWidget>

class SetMapSpatialReference : public QWidget
{
  Q_OBJECT

public:
  explicit SetMapSpatialReference(QWidget* parent = 0);
  ~SetMapSpatialReference();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;
  Esri::ArcGISRuntime::Basemap* m_basemap;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_imageLayer;
};

#endif // SET_MAP_SPATIAL_REFERENCE
