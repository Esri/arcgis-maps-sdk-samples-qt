
// Copyright 2015 ESRI 
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

#ifndef MAP_BASEMAP_H
#define MAP_BASEMAP_H

#include "pch.h"
#include "rtsample.h"
#include "MapTypes.h"
#include "MapViewTypes.h"

class map_basemap : public RTSample
{
  Q_OBJECT

public:
  explicit map_basemap(const QString& title, const QString& categoryList, const QString& shortDesc, const QString& longDesc, const QString& thumbnail);
  virtual ~map_basemap();
  virtual void run();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;
  Esri::ArcGISRuntime::Basemap* m_basemap;
  QString m_path;
};

#endif // MAP_BASEMAP_H
