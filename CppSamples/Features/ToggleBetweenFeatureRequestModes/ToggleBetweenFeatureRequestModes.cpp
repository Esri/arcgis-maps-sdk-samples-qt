#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "ToggleBetweenFeatureRequestModes.h"

// ArcGIS Maps SDK headers
#include "FeatureLayer.h"
#include "GeodatabaseTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Polygon.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

ToggleBetweenFeatureRequestModes::ToggleBetweenFeatureRequestModes(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISTopographic, this))
{
  const QUrl serviceUrl = QUrl("https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Trees_of_Portland/FeatureServer/0");

  // Create a feature table for the trees of Portland feature service.
  // The feature request mode for this service feature table is OnInteractionCache by default.
  m_treeFeatureTable = new ServiceFeatureTable(serviceUrl, this);

  m_treeFeatureLayer = new FeatureLayer(m_treeFeatureTable, this);

  m_map->operationalLayers()->append(m_treeFeatureLayer);
}

ToggleBetweenFeatureRequestModes::~ToggleBetweenFeatureRequestModes() = default;

void ToggleBetweenFeatureRequestModes::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ToggleBetweenFeatureRequestModes>("Esri.Samples", 1, 0, "ToggleBetweenFeatureRequestModesSample");
}

MapQuickView* ToggleBetweenFeatureRequestModes::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ToggleBetweenFeatureRequestModes::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_map->setInitialViewpoint(Viewpoint(45.5266, -122.6219, 6000));

  emit mapViewChanged();
}

// Use this method for manual cache.
void ToggleBetweenFeatureRequestModes::fetchCacheManually()
{
  Geometry geometry = m_mapView->visibleArea();

  // Create new query object that contains parameters to query specific request types.
  QueryParameters params;
  params.setGeometry(geometry);

  // Create list of the fields that are returned from the service.
  // Using "*" will return all fields. This can be replaced to return certain fields.
  const QStringList outFields = {"*"};

  // Populate feature table with the data based on query.
  m_treeFeatureTable->populateFromServiceAsync(params, true, outFields, this);
}

// Use this method to set feature request mode.
void ToggleBetweenFeatureRequestModes::setFeatureRequestMode(Esri::ArcGISRuntime::FeatureRequestMode featureRequestMode)
{
  m_treeFeatureTable->setFeatureRequestMode(featureRequestMode);
}
