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

#include "EditFeatureAttachments.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Feature.h"
#include "ArcGISFeature.h"
#include "FeatureEditResult.h"
#include "FeatureQueryResult.h"
#include "AttachmentListModel.h"
#include <QUrl>
#include <QUuid>
#include <QSharedPointer>
#include <QMouseEvent>
#include <QFile>

using namespace Esri::ArcGISRuntime;

EditFeatureAttachments::EditFeatureAttachments(QQuickItem* parent) :
    QQuickItem(parent),
    m_selectedFeature(nullptr),
    m_attachmentCount(0)
{
}

EditFeatureAttachments::~EditFeatureAttachments()
{
}

void EditFeatureAttachments::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // create a Viewpoint
    Viewpoint vp(Point(14902768,4050890,SpatialReference(3857)), 2e6);

    // create a Map by passing in the Basemap
    m_map = new Map(Basemap::streets(this), this);
    m_map->setInitialViewpoint(vp);

    // set map on the map view
    m_mapView->setMap(m_map);

    // create the ServiceFeatureTable
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

    // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
    m_featureLayer = new FeatureLayer(m_featureTable, this);
    m_featureLayer->setSelectionWidth(3);
    m_map->operationalLayers()->append(m_featureLayer);

    connectSignals();
}

void EditFeatureAttachments::connectSignals()
{   
    // connect to the mouse press release signal on the MapQuickView
    connect(m_mapView, &MapQuickView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
        // first clear the selection
        m_featureLayer->clearSelection();

        // set the properties for qml
        m_screenX = mouseEvent.x();
        emit screenXChanged();
        m_screenY = mouseEvent.y();
        emit screenYChanged();
        emit hideWindow();

        // call identify on the map view
        m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, 1);
    });

    // connect to the visible area changed signal on the MapQuickView
    connect(m_mapView, &MapQuickView::visibleAreaChanged, [this]()
    {
        m_featureLayer->clearSelection();
        emit hideWindow();
    });

    // connect to the identifyLayerCompleted signal on the map view
    connect(m_mapView, &MapQuickView::identifyLayerCompleted, [this](QUuid, QList<GeoElement*> identifyResults)
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

    // connect to the queryFeaturesCompleted signal on the feature table
    connect(m_featureTable, &FeatureTable::queryFeaturesCompleted, [this](QUuid, QSharedPointer<FeatureQueryResult> featureQueryResult)
    {
        if (featureQueryResult->iterator().hasNext())
        {
            // first delete if not nullptr
            if (m_selectedFeature != nullptr)
                delete m_selectedFeature;

            // set selected feature and attachment model members
            m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next());
            m_selectedFeature->setParent(this);
            m_featureType = m_selectedFeature->attributeValue("typdamage").toString();
            emit featureTypeChanged();
            emit featureSelected();
            emit attachmentModelChanged();

            // get the number of attachments
            connect(m_selectedFeature->attachmentListModel(), &AttachmentListModel::fetchAttachmentInfosCompleted, [this](QUuid, const QList<QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>>)
            {
                m_attachmentCount = m_selectedFeature->attachmentListModel()->rowCount();
                emit attachmentCountChanged();
            });
        }
    });

    // connect to the applyEditsCompleted signal from the ServiceFeatureTable
    connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted, [this](QUuid, QList<QSharedPointer<FeatureEditResult>> featureEditResults)
    {
        if (featureEditResults.length() > 0)
        {
            // obtain the first item in the list
            QSharedPointer<FeatureEditResult> featureEditResult = featureEditResults.first();
            // check if there were errors
            if (!featureEditResult->isCompletedWithErrors())
            {
                qDebug() << "Successfully edited feature attachments";
            }
            else
            {
                qDebug() << "Apply edits error:" << featureEditResult->error().code() << featureEditResult->error().description();
            }
        }
    });
}

int EditFeatureAttachments::screenX() const
{
    return m_screenX;
}

int EditFeatureAttachments::screenY() const
{
    return m_screenY;
}

QString EditFeatureAttachments::featureType() const
{
    return m_featureType;
}

AttachmentListModel* EditFeatureAttachments::attachmentModel() const
{    
    return m_selectedFeature ? m_selectedFeature->attachmentListModel() : nullptr;
}

void EditFeatureAttachments::addAttachment(QString fileUrl, QString contentType, QString fileName)
{
    QFile* file = new QFile(fileUrl);
    m_selectedFeature->attachmentListModel()->addAttachment(file, contentType, fileName);
}

void EditFeatureAttachments::deleteAttachment(int index)
{
    m_selectedFeature->attachmentListModel()->deleteAttachment(index);
}

int EditFeatureAttachments::attachmentCount() const
{
    return m_attachmentCount;
}
