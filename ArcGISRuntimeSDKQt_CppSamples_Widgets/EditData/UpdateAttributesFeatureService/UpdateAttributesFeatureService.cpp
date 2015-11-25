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

#include "UpdateAttributesFeatureService.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Feature.h"
#include <QUrl>
#include <QVBoxLayout>
#include <QUuid>
#include <QInputDialog>
#include <QStringList>

using namespace Esri::ArcGISRuntime;

UpdateAttributesFeatureService::UpdateAttributesFeatureService(QWidget* parent) :
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

UpdateAttributesFeatureService::~UpdateAttributesFeatureService()
{
}

void UpdateAttributesFeatureService::createUi()
{
    // Create the input dialog
    m_inputDialog = new QInputDialog(this);
    m_inputDialog->setModal(true);
    m_inputDialog->setLabelText("Update Attribute");
    m_inputDialog->setComboBoxItems(QStringList()
                                    << "Destroyed"
                                    << "Major"
                                    << "Minor"
                                    << "Affected"
                                    << "Inaccessible");

    // create the layout
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);
}

void UpdateAttributesFeatureService::connectSignals()
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
        if (!identifyResults.empty())
        {
            // select the item in the result
            m_featureLayer->selectFeature(static_cast<Feature*>(identifyResults.at(0)));

            // obtain the selected feature with attributes
            QueryParameters queryParams;
            queryParams.setOutFields(QStringList() << "typdamage");
            QString whereClause = "objectid=" + static_cast<ArcGISFeature*>(identifyResults.at(0))->attributeValue("objectid").toString();
            queryParams.setWhereClause(whereClause);
            m_featureTable->queryFeatures(queryParams);
        }

        qDeleteAll(identifyResults);
    });

    connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, [this](QUuid, QSharedPointer<FeatureQueryResult> featureQueryResult)
    {
        if (featureQueryResult->iterator().hasNext())
        {
            // first delete if not nullptr
            if (m_selectedFeature != nullptr)
                delete m_selectedFeature;

            // set selected feature member
            m_selectedFeature = featureQueryResult->iterator().next(this);
            QString currentVal = m_selectedFeature->attributeValue("typdamage").toString();
            m_inputDialog->setTextValue(currentVal);
            m_inputDialog->show();
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
    connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, [this](QUuid, QList<QSharedPointer<FeatureEditResult>> featureEditResults)
    {
        // obtain the first item in the list
        auto featureEditResult = featureEditResults.first();
        // check if there were errors, and if not, log the new object ID
        if (!featureEditResult->isCompletedWithErrors())
            qDebug() << "Successfully updated attribute for Object ID:" << featureEditResult->objectId();
        else
            qDebug() << "Apply edits error.";
        m_featureLayer->clearSelection();
    });

    // Connect the input dialog accepted signal to lambda for setting the new attribute value
    connect(m_inputDialog, &QInputDialog::accepted, [this]()
    {
        // update the select feature's attribute value
        m_selectedFeature->setAttributeValue("typdamage", m_inputDialog->textValue());

        // update the feature table
        m_featureTable->updateFeature(m_selectedFeature);
    });
}
