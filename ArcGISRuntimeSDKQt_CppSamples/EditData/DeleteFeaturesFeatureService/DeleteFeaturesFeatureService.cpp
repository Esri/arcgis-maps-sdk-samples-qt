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

#include "DeleteFeaturesFeatureService.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Feature.h"
#include "QueryParameters.h"
#include "Envelope.h"
#include <QUrl>
#include <QVBoxLayout>
#include <QUuid>
#include <QStringList>
#include <QPushButton>
#include <QGraphicsProxyWidget>

using namespace Esri::ArcGISRuntime;

DeleteFeaturesFeatureService::DeleteFeaturesFeatureService(QWidget* parent) :
    QWidget(parent),
    m_selectedFeature(nullptr)
{    
    // create a Viewpoint
    Viewpoint vp(Point(544871,6806138,SpatialReference(3857)), 2e6);

    // create a Map by passing in the Basemap
    m_map = new Map(Basemap::streets(this), this);
    m_map->setInitialViewpoint(vp);

    // add the Map to a MapGraphicsView
    m_mapView = new MapGraphicsView(m_map, this);

    // create the ServiceFeatureTable
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

    // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
    m_featureLayer = new FeatureLayer(m_featureTable, this);
    m_featureLayer->setSelectionColor(QColor("cyan"));
    m_featureLayer->setSelectionWidth(3);
    m_map->operationalLayers()->append(m_featureLayer);

    // setup the UI
    createUi();

    // connect to signals
    connectSignals();
}

DeleteFeaturesFeatureService::~DeleteFeaturesFeatureService()
{
}

void DeleteFeaturesFeatureService::connectSignals()
{
    // connect to the mouse press release signal on the MapGraphicsView
    connect(m_mapView, &MapGraphicsView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
        // first clear the selection
        m_featureLayer->clearSelection();

        // call identify on the map view
        m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, 1);
    });

    // connect to the identifyLayerCompleted signal on the map view
    connect(m_mapView, &MapGraphicsView::identifyLayerCompleted, [this](QUuid, QList<GeoElement*> identifyResults)
    {
        if (identifyResults.size() > 0)
        {
            // delete selected feature member if not nullptr
            if (m_selectedFeature != nullptr)
                delete m_selectedFeature;

            // select the item in the result
            m_featureLayer->selectFeature(static_cast<Feature*>(identifyResults.at(0)));

            // set selected feature member
            m_selectedFeature = static_cast<ArcGISFeature*>(identifyResults.at(0));
        }
    });

    // connect to the addFeatureCompleted signal from the ServiceFeatureTable
    connect(m_featureTable, &ServiceFeatureTable::deleteFeatureCompleted, [this](QUuid, bool success)
    {
        // if delete feature was successful, call apply edits
        if (success)
            m_featureTable->applyEdits();
    });

    // connect to the applyEditsCompleted signal from the ServiceFeatureTable
    connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, [this](QUuid, QList<QSharedPointer<FeatureEditResult>> featureEditResults)
    {
        // obtain the first item in the list
        QSharedPointer<FeatureEditResult> featureEditResult = featureEditResults.first();
        // check if there were errors, and if not, log the new object ID
        if (!featureEditResult->isCompletedWithErrors())
            qDebug() << "Successfully deleted Object ID:" << featureEditResult->objectId();
        else
            qDebug() << "Apply edits error.";
    });

    // connect to the button's clicked signal for deleting the selected feature
    connect(m_DeleteButton, &QPushButton::clicked, [this]()
    {
        // delete the currently selected feature
        m_featureTable->deleteFeature(m_selectedFeature);
    });
}

void DeleteFeaturesFeatureService::createUi()
{
    // Create the button to delete the selected feature
    m_DeleteButton = new QPushButton("Delete Selected", this);
    m_DeleteButton->setStyleSheet("QPushbutton#text {color: black;}");

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(10);
    layout->addWidget(m_DeleteButton);
    widget->setPalette(QPalette(QPalette::Base));
    widget->setLayout(layout);

    QGraphicsProxyWidget *proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(10, 10);
    proxy->setOpacity(0.95);

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}
