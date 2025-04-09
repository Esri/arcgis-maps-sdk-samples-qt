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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "CreateKmlMultiTrack.h"

// ArcGIS Maps SDK headers
#include "Basemap.h"
#include "Error.h"
#include "ErrorException.h"
#include "Geometry.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "KmlDataset.h"
#include "KmlDocument.h"
#include "KmlMultiTrack.h"
#include "KmlNodeListModel.h"
#include "KmlPlacemark.h"
#include "KmlTrack.h"
#include "KmlTrackElement.h"
#include "Location.h"
#include "LocationDisplay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Point.h"
#include "Polyline.h"
#include "PolylineBuilder.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimulatedLocationDataSource.h"
#include "SimulationParameters.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"

// Qt headers
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QStandardPaths>
#include <QList>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }

  QString coastalTrailJson()
  {
    QFile jsonFile(":/Samples/EditData/CreateKmlMultiTrack/coastalTrail.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug() << "Failed to open JSON file";
      return {};
    }
    return jsonFile.readAll();
  }
} // namespace


CreateKmlMultiTrack::CreateKmlMultiTrack(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreets, this)),
  m_graphicsOverlay(new GraphicsOverlay(this)),
  m_locationSymbol(new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::red, 10, this)),
  m_lineSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 3, this))
{
}

CreateKmlMultiTrack::~CreateKmlMultiTrack() = default;

void CreateKmlMultiTrack::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CreateKmlMultiTrack>("Esri.Samples", 1, 0, "CreateKmlMultiTrackSample");
}

MapQuickView* CreateKmlMultiTrack::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void CreateKmlMultiTrack::setMapView(MapQuickView* mapView)
{
  if (m_mapView)
  {
    m_mapView->setMap(nullptr); // Remove map from old mapView.
    m_mapView->graphicsOverlays()->clear(); // Remove Graphics overlays from old mapView.
  }

  m_mapView = mapView;

  if (!m_mapView)
  {
    return;
  }

  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  m_mapView->setMap(m_map);
  emit mapViewChanged();

  // Automatically enable recenter button when navigation pan is disabled
  connect(m_mapView->locationDisplay(), &LocationDisplay::autoPanModeChanged, this, [this](LocationDisplayAutoPanMode autoPanMode)
  {
    m_isRecenterButtonEnabled = (autoPanMode == LocationDisplayAutoPanMode::Off);
    emit isRecenterButtonEnabledChanged();
  });

  // Listen for changes in location
  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](const Location& location)
  {
    if (m_isRecordingTrack)
    {
      addTrackElement(location.position());
      m_polylineBuilder->addPoint(location.position());
    }
  });

  // turn on map view's navigation mode
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
}

void CreateKmlMultiTrack::startNavigation()
{
  // Get the hiking path geometry
  auto routeGeometry = geometry_cast<Polyline>(Geometry::fromJson(coastalTrailJson()));
  constexpr double velocity = 25.0;
  constexpr double horizontalAccuracy = 0.0;
  constexpr double verticalAccuracy = 0.0;
  constexpr double paddingInDips = 25.0;

  // Create a simulated location data source from json data with simulation parameters to set a consistent velocity
  auto* simulationParameters = new SimulationParameters(QDateTime::currentDateTime(), velocity, horizontalAccuracy, verticalAccuracy, this);
  m_simulatedLocationDataSource = new SimulatedLocationDataSource(this);
  m_simulatedLocationDataSource->setLocationsWithPolyline(routeGeometry, simulationParameters);

  // Set the map's initial viewpoint
  m_mapView->setViewpointGeometryAsync(routeGeometry, paddingInDips);

  // Set the simulated location data source as the location data source for this app
  m_mapView->locationDisplay()->setDataSource(m_simulatedLocationDataSource);

  // Start the location data source
  m_simulatedLocationDataSource->start();
}

void CreateKmlMultiTrack::addTrackElement(const Point& locationPoint)
{
  // Add a new element to the list of KML track elements
  auto* trackElement = new KmlTrackElement(QDateTime::currentDateTime(), locationPoint, nullptr, this);
  m_kmlTrackElements.append(trackElement);
  emit elementsCountChanged();

  // Add a graphic at the location's position
  auto* graphic = new Graphic(locationPoint, m_locationSymbol, this);
  m_graphicsOverlay->graphics()->append(graphic);
}

void CreateKmlMultiTrack::startRecordingKmlTrack()
{
  m_isRecordingTrack = true;
  emit isRecordingTrackChanged();

  m_kmlTrackElements.clear();
  emit elementsCountChanged();

  m_polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), this);
}

void CreateKmlMultiTrack::stopRecordingKmlTrack()
{
  if (m_kmlTrackElements.isEmpty())
  {
    return;
  }

  m_polylineBuilderList.append(m_polylineBuilder);

  const KmlTrack kmlTrack(m_kmlTrackElements, KmlAltitudeMode::RelativeToGround);
  m_kmlTracks.append(kmlTrack);
  emit multiTrackCountChanged();

  displayKmlTracks();
  m_isRecordingTrack = false;
  emit isRecordingTrackChanged();
}

void CreateKmlMultiTrack::displayKmlTracks()
{
  m_graphicsOverlay->graphics()->clear();
  for (const auto* polyLineBuilder : m_polylineBuilderList)
  {
    // Add the polyline graphic to the map
    auto* graphic = new Graphic(polyLineBuilder->toPolyline(), m_lineSymbol, this);
    m_graphicsOverlay->graphics()->append(graphic);
  }
}

// Exports the [KmlMultiTrack] as a kmz file to local storage.
void CreateKmlMultiTrack::exportKmlMultiTrack()
{
  // Create a KML document
  auto* kmlDocument = new KmlDocument(this);

  // Create a KML multi-track
  const KmlMultiTrack multiTrack(m_kmlTracks, false);

  // Add the multi-track as a placemark
  auto* placemark = new KmlPlacemark(multiTrack, this);
  kmlDocument->childNodesListModel()->insert(0, placemark);

  // Define the save file path
  const auto localKmlFilePath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/HikingTracks.kmz";
  QFile localKmlFile(localKmlFilePath);
  if (localKmlFile.exists())
  {
    localKmlFile.remove();
  }
  // Save KML file to local storage
  // Write the KML document to the chosen path.
  kmlDocument->saveAsAsync(localKmlFilePath).then(this, [localKmlFilePath, this]()
  {
    qDebug() << "Saved KmlMultiTrack: " + QFileInfo(localKmlFilePath).filePath();
    stopNavigation();
  }).onFailed([](const ErrorException&)
  {
    qDebug() << "Error saving KML file";
  });
}

void CreateKmlMultiTrack::stopNavigation()
{
  if (m_simulatedLocationDataSource)
  {
    m_simulatedLocationDataSource->stop();
  }
  m_isShowTracksFromFileEnabled = true;
  emit isShowTracksFromFileEnabledChanged();
}

void CreateKmlMultiTrack::loadLocalKmlFile()
{
  const auto localKmlFilePath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/HikingTracks.kmz";
  const QFile localKmlFile(localKmlFilePath);
  if (!localKmlFile.exists())
  {
    qDebug() << "Error locating KML file";
    return;
  }

  auto* localKmlDataset = new KmlDataset(QUrl::fromLocalFile(localKmlFilePath), this);
  connect(localKmlDataset, &KmlDataset::doneLoading, this, [this, localKmlDataset]()
  {
    if (localKmlDataset->loadStatus() != LoadStatus::Loaded)
    {
      qDebug() << "Error parsing KML file " +  localKmlDataset->loadError().message();
      return;
    }

    const auto* kmlDocument = dynamic_cast<KmlDocument*>(localKmlDataset->rootNodes().at(0));
    if (!kmlDocument)
    {
      qDebug() << "Error: Root node is not a KmlDocument.";
      return;
    }

    const auto* kmlPlacemark = dynamic_cast<KmlPlacemark*>(kmlDocument->childNodesListModel()->at(0));
    if (!kmlPlacemark)
    {
      qDebug() << "Error: First child node is not a KmlPlacemark.";
      return;
    }

    const auto kmlMultiTrack = static_cast<KmlMultiTrack>(kmlPlacemark->kmlGeometry());

    QList<Geometry> geometries;
    for (const auto& track : kmlMultiTrack.tracks())
    {
      geometries.append(track.geometry());
    }
    const auto allTracksGeometry = GeometryEngine::unionOf(geometries);
    if (allTracksGeometry.isEmpty())
    {
      qDebug() << "KmlMultiTrack has no geometry";
      return;
    }

    m_mapView->setViewpointGeometryAsync(allTracksGeometry, 25.0);

    m_trackGeometries.append(allTracksGeometry);
    for (const auto& track : kmlMultiTrack.tracks())
    {
      m_trackGeometries.append(track.geometry());
    }
    emit trackGeometriesCountChanged();
  });

  localKmlDataset->load();
}

void CreateKmlMultiTrack::previewKmlTrack(int index)
{
  m_mapView->setViewpointGeometryAsync(m_trackGeometries[index], 25.0);
}

void CreateKmlMultiTrack::recenter()
{
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Navigation);
  m_isRecenterButtonEnabled = false;
  emit isRecenterButtonEnabledChanged();
}

void CreateKmlMultiTrack::reset()
{
  qDeleteAll(m_kmlTrackElements);
  m_kmlTrackElements.clear();
  m_kmlTracks.clear();
  qDeleteAll(m_polylineBuilderList);
  m_polylineBuilderList.clear();
  m_trackGeometries.clear();
  m_graphicsOverlay->graphics()->clear();
  m_isShowTracksFromFileEnabled = false;
  emit isShowTracksFromFileEnabledChanged();
}

int CreateKmlMultiTrack::elementsCount() const
{
  return m_kmlTrackElements.count();
}

int CreateKmlMultiTrack::multiTrackCount() const
{
  return m_kmlTracks.count();
}

int CreateKmlMultiTrack::trackGeometriesCount() const
{
  return m_trackGeometries.count();
}
