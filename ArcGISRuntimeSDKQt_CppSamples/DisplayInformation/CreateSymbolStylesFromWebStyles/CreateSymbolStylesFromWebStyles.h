// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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

namespace Esri::ArcGISRuntime
{
  class FeatureLayer;
  class Map;
  class MapQuickView;
  class UniqueValue;
  class UniqueValueRenderer;
  class LegendInfo;
  class SymbolStyle;
}

class QAbstractListModel;

#include <QObject>
#include <QMap>
#include <QAbstractListModel>

Q_MOC_INCLUDE("MapQuickView.h")
Q_MOC_INCLUDE("QAbstractListModel")

class CreateSymbolStylesFromWebStyles : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QAbstractListModel* legendInfoListModel MEMBER m_legendInfoListModel NOTIFY legendInfoListModelChanged)

public:
  explicit CreateSymbolStylesFromWebStyles(QObject* parent = nullptr);
  ~CreateSymbolStylesFromWebStyles();

  static void init();

signals:
  void mapViewChanged();
  void legendInfoListModelChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QMap<QString,QStringList> createCategoriesMap();
  void createSymbolStyles();

  Esri::ArcGISRuntime::FeatureLayer* m_webStyleLayer = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::UniqueValueRenderer* m_uniqueValueRenderer = nullptr;
  QAbstractListModel* m_legendInfoListModel = nullptr;
};

#endif // CREATESYMBOLSTYLESFROMWEBSTYLES_H
