// [WriteFile Name=SetMaxExtent, Category=Maps]
// [Legal]
// Copyright 2022 Esri.
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

#ifndef SETMAXEXTENT_H
#define SETMAXEXTENT_H

// ArcGIS Maps SDK headers
#include "Envelope.h"

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime
{
class Envelope;
class Graphic;
class GraphicsOverlay;
class Map;
class MapQuickView;
class SimpleLineSymbol;
}

Q_MOC_INCLUDE("MapQuickView.h")

class SetMaxExtent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit SetMaxExtent(QObject* parent = nullptr);
  ~SetMaxExtent();

  static void init();
  Q_INVOKABLE void toggleMaxExtent();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createExtentBorder();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::Graphic* m_coloradoGraphic = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_coloradoOverlay = nullptr;
  Esri::ArcGISRuntime::Envelope m_coloradoEnvelope;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_coloradoOutline = nullptr;
  bool m_maxExtentEnabled = true;
};

#endif // SETMAXEXTENT_H
