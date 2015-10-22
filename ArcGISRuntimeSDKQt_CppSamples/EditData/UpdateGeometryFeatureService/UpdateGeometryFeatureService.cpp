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

#include "UpdateGeometryFeatureService.h"
#include "Map.h"
#include "MapView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Feature.h"
#include <QUrl>
#include <QVBoxLayout>
#include <QMap>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

UpdateGeometryFeatureService::UpdateGeometryFeatureService(QWidget* parent) :
    QWidget(parent),
    m_selectedFeature(nullptr),
    m_isFeatureSelected(false)
{    
    // create a Viewpoint
    Viewpoint vp(Point(-9030446.96,943791.32,SpatialReference(3857)), 2e6);

    // create a Map by passing in the Basemap
    m_map = new Map(Basemap::streets(this), this);
    m_map->setInitialViewpoint(vp);

    // add the Map to a MapView
    m_mapView = new MapView(m_map, this);

    // create the ServiceFeatureTable
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

    // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
    m_featureLayer = new FeatureLayer(m_featureTable, this);
    m_featureLayer->setSelectionColor(QColor("cyan"));
    m_featureLayer->setSelectionWidth(3);
    m_map->operationalLayers()->append(m_featureLayer);

    // connect to signals
    connectSignals();

    // setup the UI
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

UpdateGeometryFeatureService::~UpdateGeometryFeatureService()
{
}

void UpdateGeometryFeatureService::connectSignals()
{
    // connect to the mouse press release signal on the MapView
    connect(m_mapView, &MapView::mousePressRelease, [this](QMouseEvent& mouseEvent)
    {
        // get the point from the mouse point
        Point mapPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

        // if a feature is already selected, move the selected feature to the new geometry
        if (m_isFeatureSelected)
        {
            // set the selected feature's geometry to the tapped map point
            m_selectedFeature->setGeometry(mapPoint);

            // update the feature table with the new feature
            m_featureTable->updateFeature(m_selectedFeature);

            // reset the feature layer
            m_featureLayer->clearSelection();
            m_isFeatureSelected = false;
        }

        // else select a new feature
        else
        {
            // first clear the selection
            m_featureLayer->clearSelection();

            // call identify on the map view
            m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, 1);
        }
    });

    // connect to the identifyLayerCompleted signal on the map view
    connect(m_mapView, &MapView::identifyLayerCompleted, [this](QUuid, QList<GeoElement*> identifyResults)
    {
        if (identifyResults.size() > 0)
        {
            // first delete if not nullptr
            if (m_selectedFeature != nullptr)
                delete m_selectedFeature;

            // select the item in the result
            m_featureLayer->selectFeature(static_cast<Feature*>(identifyResults.at(0)));

            // set selected feature member
            m_selectedFeature = static_cast<ArcGISFeature*>(identifyResults.at(0));
            m_isFeatureSelected = true;
        }
    });

    // connect to the updateFeatureCompleted signal to determine if the update was successful
    connect(m_featureTable, &ServiceFeatureTable::updateFeatureCompleted, [this](QUuid, bool success)
    {
        // if the update was successful, call apply edits to apply to the service
        if (success)
            m_featureTable->applyEdits();
    });

    // connect to the applyEditsCompleted signal from the ServiceFeatureTable
    connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, [this](QUuid, QList<std::shared_ptr<FeatureEditResult>> featureEditResults)
    {
        // obtain the first item in the list
        std::shared_ptr<FeatureEditResult> featureEditResult = featureEditResults.first();
        // check if there were errors, and if not, log the new object ID
        if (!featureEditResult->isCompletedWithErrors())
            qDebug() << "Successfully updated geometry for Object ID:" << featureEditResult->objectId();
        else
            qDebug() << "Apply edits error.";
    });
}
