// Copyright 2021 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "Untitled.h"

#include "MapQuickView.h"
#include "MobileMapPackage.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

Untitled::Untitled(QObject* parent /* = nullptr */):
  QObject(parent)
{
}

Untitled::~Untitled()
{
}

MapQuickView* Untitled::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void Untitled::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;
  setupMapFromMmpk(QDir::homePath() + "/ArcGIS/Runtime/Data/mmpk/MahouRivieraTrails.mmpk");

  emit mapViewChanged();
}

void Untitled::setupMapFromMmpk(QString pathToMmpk)
{
  // @@Start(add_mmpk_load)
  // Instantiate a MobileMapPackage object and establish the path to the MMPK file.
  // The package contains a list of maps that could be shown in the UI for selection.
  MobileMapPackage* mobileMapPackage = new MobileMapPackage(pathToMmpk);

  // Use connect to signal when the package is done loading so that m_mapView can be set to the first map (0).
  connect(mobileMapPackage, &MobileMapPackage::doneLoading, this, [mobileMapPackage, this]

  {
    // Obtain the first map in the list of maps, and set the map on the map view to display.
    // This could be set to any map in the list.
    m_mapView->setMap(mobileMapPackage->maps().at(0));

  });
  mobileMapPackage->load();
  // @@End(add_mmpk_load)
}
