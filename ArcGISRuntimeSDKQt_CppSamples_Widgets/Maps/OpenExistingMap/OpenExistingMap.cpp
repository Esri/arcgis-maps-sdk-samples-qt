// [WriteFile Name=OpenExistingMap, Category=Maps]
// [Legal]
// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "OpenExistingMap.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QInputDialog>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

OpenExistingMap::OpenExistingMap(QWidget* parent) :
    QWidget(parent)
{        
    // Create a map view
    m_mapView = new MapGraphicsView(this);

    // Create the portal items
    createPortalMaps();

    // Create the button to display the input dialog
    m_button = new QPushButton(this);
    m_button->setText("Open a Map");
    m_button->setStyleSheet("QPushbutton#text {color: black;}");

    // Create the input dialog
    m_inputDialog = new QInputDialog(this);
    m_inputDialog->setModal(true);
    m_inputDialog->setLabelText("Select a map to open");

    m_inputDialog->setComboBoxItems(m_portalIds.keys());

    // Connect the button clicked signal to lambda for showing input dialog
    connect(m_button, &QPushButton::clicked, [this]() {
        m_inputDialog->show();
    });

    // Connect the input dialog accepted signal to lambda for opening the selected map
    connect(m_inputDialog, &QInputDialog::accepted, [this]()
      {
        // create a portal item with the item id
        QString portalId = m_portalIds.value(m_inputDialog->textValue() );
        if(portalId.isEmpty())
            return;

        PortalItem* portalItem = new PortalItem(QUrl("http://arcgis.com/sharing/rest/content/items/" + portalId), this);

        // create a new map from the portal item
        Map* map = new Map(portalItem, this);

        // set the map to the map view
        m_mapView->setMap(map);
      }
    );

    // Set up the UI
    createUi();
}

OpenExistingMap::~OpenExistingMap()
{
}

void OpenExistingMap::createPortalMaps()
{
    m_portalIds.insert("Population Pressure", "392451c381ad4109bf04f7bd442bc038");
    m_portalIds.insert("USA Tapestry Segmentation", "01f052c8995e4b9e889d73c3e210ebe3");
    m_portalIds.insert("Geology of United States", "92ad152b9da94dee89b9e387dfe21acd");
}

void OpenExistingMap::createUi()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(10);
    layout->addWidget(m_button);
    widget->setPalette(QPalette(QPalette::Base));
    widget->setLayout(layout);

    QGraphicsProxyWidget *proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(10, 10);
    proxy->setOpacity(0.95);

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}
