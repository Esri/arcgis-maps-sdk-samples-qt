// [WriteFile Name=DisplayDeviceLocation, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef DISPLAY_DEVICE_LOCATION_H
#define DISPLAY_DEVICE_LOCATION_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
  }
}

#include <QQuickItem>

class DisplayDeviceLocation : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString compassMode READ compassMode NOTIFY compassModeChanged)
  Q_PROPERTY(QString navigationMode READ navigationMode NOTIFY navigationModeChanged)
  Q_PROPERTY(QString recenterMode READ recenterMode NOTIFY recenterModeChanged)
  Q_PROPERTY(QString onMode READ onMode NOTIFY onModeChanged)
  Q_PROPERTY(QString stopMode READ stopMode NOTIFY stopModeChanged)
  Q_PROPERTY(QString closeMode READ closeMode NOTIFY closeModeChanged)

public:
  explicit DisplayDeviceLocation(QQuickItem* parent = nullptr);
  ~DisplayDeviceLocation() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void startLocationDisplay();
  Q_INVOKABLE void stopLocationDisplay();
  Q_INVOKABLE void setAutoPanMode(QString autoPanMode);

signals:
  void compassModeChanged();
  void navigationModeChanged();
  void recenterModeChanged();
  void onModeChanged();
  void stopModeChanged();
  void closeModeChanged();

private:
  static const QString compassMode();
  static const QString navigationMode();
  static const QString recenterMode();
  static const QString onMode();
  static const QString stopMode();
  static const QString closeMode();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  static const QString s_compassMode;
  static const QString s_navigationMode;
  static const QString s_recenterMode;
  static const QString s_onMode;
  static const QString s_stopMode;
  static const QString s_closeMode;
};

#endif // DISPLAY_DEVICE_LOCATION_H

