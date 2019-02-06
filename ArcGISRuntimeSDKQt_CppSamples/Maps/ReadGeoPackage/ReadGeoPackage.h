// [WriteFile Name=ReadGeoPackage, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef READGEOPACKAGE_H
#define READGEOPACKAGE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class listModel;
    class listView;
    class GeoPackage;
    class Layer;
    class RasterLayer;
    class FeatureTable;
    class FeatureLayer;
    class GeoPackageRaster;
    class GeoPackageFeatureTable;
  }
}

#include <QQuickItem>
#include <Viewpoint.h>
#include <QVariantMap>
#include <QVariantList>

class ReadGeoPackage : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QVariantList layerList READ layerList NOTIFY layerListChanged)

public:
  explicit ReadGeoPackage(QQuickItem* parent = nullptr);
  ~ReadGeoPackage() override = default;

  void readGeoPackage();
  void componentComplete() override;

  static void init();

  Q_INVOKABLE void addOrShowLayer(int index, bool onOff);

signals:
  void layerListChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GeoPackage* auroraGpkg = nullptr;

  QVariantList m_layerList;
  QVariantList layerList() const;
  QString m_dataPath;
};

#endif // READGEOPACKAGE_H
