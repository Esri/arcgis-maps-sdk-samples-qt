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

#include "OpenExistingMap.h"
#include "Map.h"
#include "MapView.h"
#include "PortalItem.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QMap>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

OpenExistingMap::OpenExistingMap(QWidget* parent) :
    QWidget(parent)
{        
    // Create a map view
    m_mapView = new MapView(this);

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

    // Add the existing maps to the combo box
    m_inputDialog->setComboBoxItems(QStringList()
                                    << "Housing with Mortgages"
                                    << "USA Tapestry Segmentation"
                                    << "Geology of United States");

    // Connect the button clicked signal to lambda for showing input dialog
    connect(m_button, &QPushButton::clicked, [this]() {
        m_inputDialog->show();
    });

    // Connect the input dialog accepted signal to lambda for opening the selected map
    connect(m_inputDialog, &QInputDialog::accepted, [this]() {
        Map* newMap = new Map(m_portalMaps.value(m_inputDialog->textValue()), this);
        m_mapView->setMap(newMap);
    });

    // Set up the UI
    createUi();
}

void OpenExistingMap::createPortalMaps()
{
    // Housing with Mortgages
    PortalItem pitem1;
    pitem1.setUrl(QUrl("http://www.arcgis.com/sharing/rest/content/items/2d6fa24b357d427f9c737774e7b0f977"));
    m_portalMaps.insert("Housing with Mortgages", pitem1);

    // USA Tapestry Segmentation
    PortalItem pitem2;
    pitem2.setUrl(QUrl("http://www.arcgis.com/sharing/rest/content/items/01f052c8995e4b9e889d73c3e210ebe3"));
    m_portalMaps.insert("USA Tapestry Segmentation", pitem2);

    // Geology of United States
    PortalItem pitem3;
    pitem3.setUrl(QUrl("http://www.arcgis.com/sharing/rest/content/items/74a8f6645ab44c4f82d537f1aa0e375d"));
    m_portalMaps.insert("Geology of United States", pitem3);
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

OpenExistingMap::~OpenExistingMap()
{
}
