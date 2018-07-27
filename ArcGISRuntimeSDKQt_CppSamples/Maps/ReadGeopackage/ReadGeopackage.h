// [WriteFile Name=ReadGeopackage, Category=Maps]
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
class Layer;
}
}

#include <QQuickItem>
#include <Viewpoint.h>
#include <GeoPackage.h>
#include <QListView>
#include <QList>
#include <QString>
#include <GeoPackageRaster.h>
#include <GeoPackageFeatureTable.h>
#include <FeatureTable.h>
#include <FeatureLayer.h>
#include <RasterLayer.h>
#include <QVariantMap>
#include <QVariantList>
#include <Layer.h>

class ReadGeopackage : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QVariantList layerNamesList READ layerNamesList NOTIFY layerNamesListChanged)

public:
  explicit ReadGeopackage(QQuickItem* parent = nullptr);
  ~ReadGeopackage() = default;
  void listMaps();
  Q_INVOKABLE void addOrShowLayer(int index, bool onOff);
  void componentComplete() Q_DECL_OVERRIDE;
  static void init();

signals:

  void layerNamesListChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::GeoPackage* auroraGpkg;
  QVariantList layerNamesList() const;
  QVariantMap layerMap;
  QVariantList m_layerNamesList;

  QList<Esri::ArcGISRuntime::Layer*> m_layerList;


};

#endif // READGEOPACKAGE_H
