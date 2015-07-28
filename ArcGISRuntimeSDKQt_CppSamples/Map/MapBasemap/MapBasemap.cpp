
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

#include "MapBasemap.h"
#include "Map.h"
#include "MapView.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"
#include <QDir>
#include <QUrl>
#include <QVBoxLayout>

MapBasemap::MapBasemap() :
  QWidget(nullptr),
  m_map(nullptr),
  m_mapView(nullptr),
  m_basemap(nullptr)
{
    Esri::ArcGISRuntime::ArcGISTiledLayer* layer = new Esri::ArcGISRuntime::ArcGISTiledLayer(QUrl("http://services.arcgisonline.com/arcgis/rest/services/World_Imagery/MapServer"), this);

    m_basemap = new Esri::ArcGISRuntime::Basemap(layer, this);

    m_map = new Esri::ArcGISRuntime::Map(m_basemap, this);
    m_mapView = new Esri::ArcGISRuntime::MapView(m_map, this);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

MapBasemap::~MapBasemap()
{
}
