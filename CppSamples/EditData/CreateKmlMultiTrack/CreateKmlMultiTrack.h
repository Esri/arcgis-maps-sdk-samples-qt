// [WriteFile Name=CreateKmlMultiTrack, Category=EditData]]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef CREATEKMLMULTITRACK_H
#define CREATEKMLMULTITRACK_H

// Qt headers
#include <QObject>
#include <QString>

namespace Esri::ArcGISRuntime
{
class Geometry;
class Graphic;
class GraphicsOverlay;
class KmlTrack;
class KmlTrackElement;
class Map;
class MapQuickView;
class Point;
class PolylineBuilder;
class SimpleLineSymbol;
class SimpleMarkerSymbol;
class SimulatedLocationDataSource;
}

Q_MOC_INCLUDE("MapQuickView.h")

class CreateKmlMultiTrack : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(bool isRecordingTrack MEMBER m_isRecordingTrack NOTIFY isRecordingTrackChanged)
  Q_PROPERTY(bool isRecenterButtonEnabled MEMBER m_isRecenterButtonEnabled NOTIFY isRecenterButtonEnabledChanged)
  Q_PROPERTY(bool isShowTracksFromFileEnabled MEMBER m_isShowTracksFromFileEnabled NOTIFY isShowTracksFromFileEnabledChanged)
  Q_PROPERTY(int elementsCount READ elementsCount NOTIFY elementsCountChanged)
  Q_PROPERTY(int multiTrackCount READ multiTrackCount NOTIFY multiTrackCountChanged)
  Q_PROPERTY(int trackGeometriesCount READ trackGeometriesCount NOTIFY trackGeometriesCountChanged)

public:
  explicit CreateKmlMultiTrack(QObject* parent = nullptr);
  ~CreateKmlMultiTrack() override;

  Q_INVOKABLE void startNavigation();
  Q_INVOKABLE void stopNavigation();
  Q_INVOKABLE void startRecordingKmlTrack();
  Q_INVOKABLE void stopRecordingKmlTrack();
  Q_INVOKABLE void exportKmlMultiTrack();
  Q_INVOKABLE void loadLocalKmlFile();
  Q_INVOKABLE void previewKmlTrack(int index);
  Q_INVOKABLE void recenter();
  Q_INVOKABLE void reset();

  static void init();

signals:
  void mapViewChanged();
  void isRecordingTrackChanged();
  void isShowTracksFromFileEnabledChanged();
  void isRecenterButtonEnabledChanged();
  void elementsCountChanged();
  void multiTrackCountChanged();
  void trackGeometriesCountChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;

  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void addTrackElement(const Esri::ArcGISRuntime::Point& locationPoint);
  void displayKmlTracks();
  int elementsCount() const;
  int multiTrackCount() const;
  int trackGeometriesCount() const;

private:
  bool m_isRecenterButtonEnabled = false;
  bool m_isRecordingTrack = false;
  bool m_isShowTracksFromFileEnabled = false;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::SimulatedLocationDataSource* m_simulatedLocationDataSource = nullptr;
  QList<Esri::ArcGISRuntime::Geometry> m_trackGeometries;

  QList<Esri::ArcGISRuntime::KmlTrackElement*> m_kmlTrackElements;
  QList<Esri::ArcGISRuntime::KmlTrack> m_kmlTracks;

  Esri::ArcGISRuntime::PolylineBuilder* m_polylineBuilder = nullptr;
  QList<Esri::ArcGISRuntime::PolylineBuilder*> m_polylineBuilderList;

  Esri::ArcGISRuntime::SimpleMarkerSymbol* m_locationSymbol = nullptr;
  Esri::ArcGISRuntime::SimpleLineSymbol* m_lineSymbol = nullptr;
};

#endif // CREATEKMLMULTITRACK_H
