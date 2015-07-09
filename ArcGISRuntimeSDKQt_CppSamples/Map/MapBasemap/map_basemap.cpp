
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

#include "map_basemap.h"
#include "Map.h"
#include "MapView.h"
#include "Basemap.h"
#include "ArcGISLocalTiledLayer.h"

map_basemap::map_basemap() :
  QWidget(nullptr),
  m_map(nullptr),
  m_mapView(nullptr),
  m_basemap(nullptr)
{

    m_path = QDir::homePath() + QString("/arcgis/Runtime/Data/tpks");
    QString path = m_path + QString("/Topographic.tpk");
    QFile dataFile(path);
    if(!dataFile.exists()) {
        qDebug() << "Data does not exist in " + path;
        return;
    }
    Esri::ArcGISRuntime::ArcGISLocalTiledLayer* layer = new Esri::ArcGISRuntime::ArcGISLocalTiledLayer(path, this);

    m_basemap = new Esri::ArcGISRuntime::Basemap(layer, this);

    m_map = new Esri::ArcGISRuntime::Map(m_basemap, this);
    m_mapView = new Esri::ArcGISRuntime::MapView(m_map, this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

map_basemap::~map_basemap()
{
}
