
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

#ifndef MAPVIEW_VIEWPOINT_H
#define MAPVIEW_VIEWPOINT_H

#include "pch.h"
#include "rtsample.h"
#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Map.h"
#include "MapView.h"
#include <QPushButton>

class mapview_viewpoint : public QWidget
{
    Q_OBJECT
    
public:
    explicit mapview_viewpoint();
    virtual ~mapview_viewpoint();

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapView* m_mapView;
    Esri::ArcGISRuntime::Basemap* m_basemap;
    void createWidget();
    QPushButton* m_setViewpointButton;
    QString m_path;

private slots:
    void onButtonSetViewpoint();
};

#endif // MAPVIEW_VIEWPOINT_H
