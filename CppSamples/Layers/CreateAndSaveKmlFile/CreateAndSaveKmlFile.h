// [WriteFile Name=CreateAndSaveKmlFile, Category=Layers]
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

#ifndef CREATEANDSAVEKMLFILE_H
#define CREATEANDSAVEKMLFILE_H

// C++ API headers
#include "Geometry.h"
#include "Point.h"
#include "Polygon.h"
#include "Polyline.h"

// Qt headers
#include <QObject>
#include <QTemporaryDir>
#include <QUrl>

namespace Esri::ArcGISRuntime
{
class Map;
class MapQuickView;
class KmlDocument;
class KmlDataset;
class KmlLayer;
class KmlStyle;
}

Q_MOC_INCLUDE("MapQuickView.h")

class CreateAndSaveKmlFile : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool busy MEMBER m_busy NOTIFY busyChanged)
  Q_PROPERTY(QString kmlFilePath READ kmlFilePath NOTIFY kmlFilePathChanged)

public:
  explicit CreateAndSaveKmlFile(QObject* parent = nullptr);
  ~CreateAndSaveKmlFile();

  static void init();

  Q_INVOKABLE void saveKml();

signals:
  void mapViewChanged();
  void busyChanged();
  void kmlSaveCompleted();
  void kmlFilePathChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QString kmlFilePath() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::KmlDocument* m_kmlDocument = nullptr;
  Esri::ArcGISRuntime::KmlDataset* m_kmlDataset = nullptr;
  Esri::ArcGISRuntime::KmlLayer* m_kmlLayer = nullptr;
  Esri::ArcGISRuntime::Point m_point;
  Esri::ArcGISRuntime::Polyline m_polyline;
  Esri::ArcGISRuntime::Polygon m_polygon;
  Esri::ArcGISRuntime::KmlStyle* m_kmlStyleWithPointStyle = nullptr;
  Esri::ArcGISRuntime::KmlStyle* m_kmlStyleWithLineStyle = nullptr;
  Esri::ArcGISRuntime::KmlStyle* m_kmlStyleWithPolygonStyle = nullptr;

  bool m_busy = false;

  Esri::ArcGISRuntime::Geometry createPoint() const;
  Esri::ArcGISRuntime::Geometry createPolyline() const;
  Esri::ArcGISRuntime::Geometry createPolygon() const;
  Esri::ArcGISRuntime::Geometry createEnvelope() const;
  Esri::ArcGISRuntime::KmlStyle* createKmlStyleWithPointStyle();
  Esri::ArcGISRuntime::KmlStyle* createKmlStyleWithLineStyle();
  Esri::ArcGISRuntime::KmlStyle* createKmlStyleWithPolygonStyle();

  QTemporaryDir m_tempDir;
  QString m_kmlFilePath;

  void addGraphics();
  void addToKmlDocument(const Esri::ArcGISRuntime::Geometry& geometry, Esri::ArcGISRuntime::KmlStyle* kmlStyle);
};

#endif // CREATEANDSAVEKMLFILE_H
