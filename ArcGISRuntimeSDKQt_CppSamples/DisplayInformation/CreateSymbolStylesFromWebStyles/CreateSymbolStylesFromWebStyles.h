// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef CREATESYMBOLSTYLESFROMWEBSTYLES_H
#define CREATESYMBOLSTYLESFROMWEBSTYLES_H

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureLayer;
class Map;
class MapQuickView;
class UniqueValueRenderer;
}
}

#include <QObject>
#include <QMap>

class CreateSymbolStylesFromWebStyles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit CreateSymbolStylesFromWebStyles(QObject* parent = nullptr);
  ~CreateSymbolStylesFromWebStyles();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QMap<QString,QList<QString>> createCategoriesMap();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_webStyleLayer = nullptr;
  QMap<QString,QList<QString>> m_categoriesMap;
  Esri::ArcGISRuntime::UniqueValueRenderer* m_uniqueValueRenderer = nullptr;
};

#endif // CREATESYMBOLSTYLESFROMWEBSTYLES_H
