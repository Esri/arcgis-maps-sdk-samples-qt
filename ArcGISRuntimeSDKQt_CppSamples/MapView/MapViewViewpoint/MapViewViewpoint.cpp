
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

#include "MapViewViewpoint.h"
#include <QPushButton>
#include "ArcGISTiledLayer.h"
#include <QDir>
#include <QUrl>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include <QFont>

MapViewViewpoint::MapViewViewpoint() :
  QWidget(nullptr),
  m_map(nullptr),
  m_mapView(nullptr),
  m_basemap(nullptr)
{
    Esri::ArcGISRuntime::ArcGISTiledLayer* layer = new Esri::ArcGISRuntime::ArcGISTiledLayer(QUrl("http://services.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer"), this);

    m_basemap = new Esri::ArcGISRuntime::Basemap(layer, this);
    m_map = new Esri::ArcGISRuntime::Map(m_basemap, this);
    m_mapView = new Esri::ArcGISRuntime::MapView(m_map, this);

    createWidget();
}

MapViewViewpoint::~MapViewViewpoint()
{
}

// onClicked slot for the viewpoint button
void MapViewViewpoint::onButtonSetViewpoint()
{
    Esri::ArcGISRuntime::Envelope env(6694660.685327099,-1042630.6576358064,16762334.554821558,4930464.480679419,m_mapView->spatialReference());
    Esri::ArcGISRuntime::Viewpoint vp(env, 180);
    m_mapView->setViewpointAnimated(vp, 3, Esri::ArcGISRuntime::AnimationCurve::EaseInSine);
}

// create UI elements
void MapViewViewpoint::createWidget()
{
    //add the viewpoint button
    m_setViewpointButton = new QPushButton("Set Viewpoint");
    m_setViewpointButton->setObjectName(QString::fromUtf8("m_setViewpointButton"));
    QFont font;
    font.setPixelSize(12);
    font.setBold(true);
    m_setViewpointButton->setFont(font);
    m_setViewpointButton->setStyleSheet("QPushButton#m_setViewpointButton { background-color: white; color: #000; }");
    connect(m_setViewpointButton, SIGNAL(clicked()), this, SLOT(onButtonSetViewpoint()));

    //create the layout and add the widget to the layout
    QWidget* widget = new QWidget();
    QVBoxLayout *vBoxButtonLayout = new QVBoxLayout();
    vBoxButtonLayout->addWidget(m_setViewpointButton);
    widget->setFixedSize(175,50);
    widget->setLayout(vBoxButtonLayout);
    widget->setPalette(QPalette(QPalette::Base));
    QGraphicsProxyWidget *proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(7, 7);
    proxy->setAcceptedMouseButtons(Qt::LeftButton);
    proxy->setFlag(QGraphicsItem::ItemIsSelectable, false);
    proxy->setOpacity(0.90);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}
