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

#include "ManageBookmarks.h"
#include "Map.h"
#include "MapView.h"
#include "Viewpoint.h"
#include "Bookmark.h"
#include "SpatialReference.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QMap>

using namespace Esri::ArcGISRuntime;

ManageBookmarks::ManageBookmarks(QWidget* parent) :
    QWidget(parent)
{        
    // Create a map using the imagery with labels basemap
    m_map = new Map(Basemap::imageryWithLabels(this), this);
    Envelope env1(3743486.562940341, 3170771.8344022506, 3744328.0714771077, 3171402.9658048255, SpatialReference(102100));
    m_map->setInitialViewpoint(Viewpoint(env1));

    // Create a map view, and pass in the map
    m_mapView = new MapView(m_map, this);

    // Create the bookmark combo box
    m_bookmarkCombo = new QComboBox(this);
    m_bookmarkCombo->setFixedWidth(200);
    m_bookmarkCombo->setStyleSheet("QComboBox#combo {color: black; background-color:#000000;}");

    // Create the button to display the input dialog
    m_button = new QPushButton(this);
    m_button->setText("Add Bookmark");
    m_button->setStyleSheet("QPushbutton#text {color: black;}");

    // Create the input dialog
    m_inputDialog = new QInputDialog(this);
    m_inputDialog->setModal(true);
    m_inputDialog->setLabelText("Provide the bookmark name:");

    // Create the initial bookmarks
    createInitialBookmarks();

    // Connect the combo box signal to lambda for switching between bookmarks
    connect(m_bookmarkCombo, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), [=](QString name) {
        // Use the map of bookmarks created to obtain the viewpoint of a given name
        m_mapView->setViewpoint(m_bookmarks.value(name));
    });

    // Connect the button clicked signal to lambda for showing input dialog
    connect(m_button, &QPushButton::clicked, [this]() {
        m_inputDialog->show();
    });

    // Connect the input dialog for newly added bookmarks
    connect(m_inputDialog, &QInputDialog::accepted, [this]() {
        createBookmark(m_inputDialog->textValue(), m_mapView->currentViewpoint(ViewpointType::BoundingGeometry));
    });

    // Set up the UI
    createUi();
}

void ManageBookmarks::createInitialBookmarks()
{
    // Mysterious Desert Pattern
    Envelope env1(3743486.562940341, 3170771.8344022506, 3744328.0714771077, 3171402.9658048255, SpatialReference(102100));
    Viewpoint viewpoint1(env1);
    createBookmark("Mysterious Desert Pattern", viewpoint1);

    // Strange Symbol
    Envelope env2(-13009913.860076642, 4495026.9307899885, -13009442.089218518, 4495404.031910696, SpatialReference(102100));
    Viewpoint viewpoint2(env2);
    createBookmark("Strange Symbol", viewpoint2);

    // Guitar-Shaped Forest
    Envelope env3(-7124497.45137465, -4012221.6124684606, -7121131.417429369, -4009697.0870095, SpatialReference(102100));
    Viewpoint viewpoint3(env3);
    createBookmark("Guitar-Shaped Forest", viewpoint3);

    // Grand Prismatic Spring
    Envelope env4(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100));
    Viewpoint viewpoint4(env4);
    createBookmark("Grand Prismatic Spring", viewpoint4);
}

void ManageBookmarks::createBookmark(QString name, Viewpoint viewpoint)
{
    // Create the bookmark from the name and viewpoint
    Bookmark* bookmark = new Bookmark(name, viewpoint, this);

    // Add it to the map's bookmark list
    m_map->bookmarks()->append(bookmark);

    // Add it to the combo box
    m_bookmarkCombo->addItem(name);

    // Add it to a bookmark map so it can be accessed later
    m_bookmarks.insert(name,viewpoint);
}

void ManageBookmarks::createUi()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(10);
    layout->addWidget(m_bookmarkCombo);
    layout->addWidget(m_button);
    widget->setPalette(QPalette(QPalette::Base));
    widget->setLayout(layout);

    QGraphicsProxyWidget* proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(10, 10);
    proxy->setOpacity(0.95);

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

ManageBookmarks::~ManageBookmarks()
{
}
