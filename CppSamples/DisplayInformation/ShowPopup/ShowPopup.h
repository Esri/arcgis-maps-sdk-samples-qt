// [WriteFile Name=ShowPopup, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.
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

#ifndef SHOWPOPUP_H
#define SHOWPOPUP_H

// Qt headers
#include <QFuture>
#include <QMouseEvent>
#include <QObject>
#include <QQmlListProperty>
#include <QUuid>

namespace Esri::ArcGISRuntime
{
class FeatureLayer;
class GeoElement;
class IdentifyLayerResult;
class Map;
class MapQuickView;
class Popup;
}

Q_MOC_INCLUDE("MapQuickView.h")
Q_MOC_INCLUDE("Popup.h")

class ShowPopup : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::Popup* popup MEMBER m_popup NOTIFY popupChanged)
  Q_PROPERTY(bool taskRunning READ taskRunning NOTIFY taskRunningChanged)

public:
  explicit ShowPopup(QObject* parent = nullptr);
  ~ShowPopup();

  static void init();

  Q_INVOKABLE void clearSelection() const;

signals:
  void mapViewChanged();
  void popupChanged();
  void taskRunningChanged();

private slots:
  void onMouseClicked(QMouseEvent& mouseEvent);
  void onIdentifyLayerCompleted(const QUuid&, Esri::ArcGISRuntime::IdentifyLayerResult* rawIdentifyResult);

private:
  bool taskRunning() const;
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Popup* m_popup = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;

  QFuture<Esri::ArcGISRuntime::IdentifyLayerResult*> m_future;
};

#endif // SHOWPOPUP_H
