// [WriteFile Name=AddEncExchangeSet, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
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

#ifndef ADDENCEXCHANGESET_H
#define ADDENCEXCHANGESET_H

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
}

Q_MOC_INCLUDE("MapQuickView.h")

class AddEncExchangeSet : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString colorScheme READ colorScheme WRITE setColorScheme NOTIFY colorSchemeChanged)
  Q_PROPERTY(QString areaSymbolizationType READ areaSymbolizationType WRITE setAreaSymbolizationType NOTIFY areaSymbolizationTypeChanged)
  Q_PROPERTY(QString pointSymbolizationType READ pointSymbolizationType WRITE setPointSymbolizationType NOTIFY pointSymbolizationTypeChanged)

public:
  explicit AddEncExchangeSet(QObject* parent = nullptr);
  ~AddEncExchangeSet();

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
  
  QString colorScheme() const;
  void setColorScheme(const QString& colorScheme);
  
  QString areaSymbolizationType() const;
  void setAreaSymbolizationType(const QString& areaSymbolizationType);
  
  QString pointSymbolizationType() const;
  void setPointSymbolizationType(const QString& pointSymbolizationType);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::EncExchangeSet* m_encExchangeSet = nullptr;
  QList<Esri::ArcGISRuntime::Geometry> m_extents;
  
  QString m_colorScheme = "Day";
  QString m_areaSymbolizationType = "Plain";
  QString m_pointSymbolizationType = "PaperChart";
};

#endif // ADDENCEXCHANGESET_H
