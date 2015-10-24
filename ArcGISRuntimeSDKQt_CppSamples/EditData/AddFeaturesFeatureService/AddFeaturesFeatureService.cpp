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

#include "AddFeaturesFeatureService.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Feature.h"
#include "Viewpoint.h"
#include <QUrl>
#include <QVBoxLayout>
#include <QMap>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

AddFeaturesFeatureService::AddFeaturesFeatureService(QWidget* parent) :
    QWidget(parent)
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
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);

    // connect to signals
    connectSignals();
}

AddFeaturesFeatureService::~AddFeaturesFeatureService()
{
}

void AddFeaturesFeatureService::connectSignals()
{
    // connect to the mouse press release signal on the MapGraphicsView
    connect(m_mapView, &MapGraphicsView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
        // obtain the map point
        Point newPoint = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());

        // create the feature attributes
        QMap<QString, QVariant> featureAttributes;
        featureAttributes.insert("typdamage", "Minor");
        featureAttributes.insert("primcause", "Earthquake");

        // create a new feature and add it to the feature table
        Feature* feature = m_featureTable->createFeature(featureAttributes,newPoint,this);
        m_featureTable->addFeature(feature);
    });

    // connect to the addFeatureCompleted signal from the ServiceFeatureTable
    connect(m_featureTable, &ServiceFeatureTable::addFeatureCompleted, [this](QUuid, bool success)
    {
       // if add feature was successful, call apply edits
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
            qDebug() << "New Object ID is:" << featureEditResult->objectId();
        else
            qDebug() << "Apply edits error.";
    });
}
