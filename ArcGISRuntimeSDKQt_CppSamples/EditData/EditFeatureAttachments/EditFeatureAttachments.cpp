// [WriteFile Name=EditFeatureAttachments, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

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
#include <QMouseEvent>
#include <QFile>
#include <QFileInfo>

using namespace Esri::ArcGISRuntime;

namespace
{
  // Convenience RAII struct that deletes all pointers in given container.
  struct FeatureEditListResultLock
  {
    FeatureEditListResultLock(const QList<FeatureEditResult*>& list) : results(list) { }
    ~FeatureEditListResultLock() { qDeleteAll(results); }
    const QList<FeatureEditResult*>& results;
  };
}

EditFeatureAttachments::EditFeatureAttachments(QQuickItem* parent) :
  QQuickItem(parent)
{
}

EditFeatureAttachments::~EditFeatureAttachments() = default;

void EditFeatureAttachments::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<EditFeatureAttachments>("Esri.Samples", 1, 0, "EditFeatureAttachmentsSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
  qmlRegisterUncreatableType<CalloutData>("Esri.Samples", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
}

void EditFeatureAttachments::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // create a Map by passing in the Basemap
  m_map = new Map(Basemap::streets(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(-10800000, 4500000, SpatialReference(102100)), 3e7));

  // set map on the map view
  m_mapView->setMap(m_map);

  // create the ServiceFeatureTable
  m_featureTable = new ServiceFeatureTable(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);

  // create the FeatureLayer with the ServiceFeatureTable and add it to the Map
  m_featureLayer = new FeatureLayer(m_featureTable, this);
  m_map->operationalLayers()->append(m_featureLayer);

  connectSignals();
}

void EditFeatureAttachments::connectSignals()
{   
  // connect to the mouse clicked signal on the MapQuickView
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // first clear the selection
    m_featureLayer->clearSelection();

    // set the properties for qml
    emit hideWindow();

    // call identify on the map view
    m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, false, 1);
  });

  // connect to the viewpoint changed signal on the MapQuickView
  connect(m_mapView, &MapQuickView::viewpointChanged, this, [this]()
  {
    m_featureLayer->clearSelection();
    emit hideWindow();
  });

  // connect to the identifyLayerCompleted signal on the map view
  connect(m_mapView, &MapQuickView::identifyLayerCompleted,
          this, [this](QUuid, IdentifyLayerResult* identifyResult)
  {
    if(!identifyResult)
      return;

    if (!identifyResult->geoElements().empty())
    {
      // select the item in the result
      m_featureLayer->selectFeature(static_cast<Feature*>(identifyResult->geoElements().at(0)));

      // obtain the selected feature with attributes
      QueryParameters queryParams;
      m_whereClause = "objectid=" + identifyResult->geoElements().at(0)->attributes()->attributeValue("objectid").toString();
      queryParams.setWhereClause(m_whereClause);
      m_featureTable->queryFeatures(queryParams);
    }
  });

  // connect to the queryFeaturesCompleted signal on the feature table
  connect(m_featureTable, &FeatureTable::queryFeaturesCompleted,
          this, [this](QUuid, FeatureQueryResult* featureQueryResult)
  {
    if (featureQueryResult && featureQueryResult->iterator().hasNext())
    {
      // first delete if not nullptr
      if (m_selectedFeature)
        delete m_selectedFeature;

      // set selected feature and attachment model members
      m_selectedFeature = static_cast<ArcGISFeature*>(featureQueryResult->iterator().next(this));
      // Don't delete selected feature when parent featureQueryResult is deleted.
      m_selectedFeature->setParent(this);

      const QString featureType = m_selectedFeature->attributes()->attributeValue(QStringLiteral("typdamage")).toString();
      m_mapView->calloutData()->setLocation(m_selectedFeature->geometry().extent().center());
      m_mapView->calloutData()->setTitle(QString("<b>%1</b>").arg(featureType));

      emit featureSelected();
      emit attachmentModelChanged();

      // get the number of attachments
      connect(m_selectedFeature->attachments(), &AttachmentListModel::fetchAttachmentsCompleted,
              this, [this](QUuid, const QList<Attachment*>& attachments)
      {
        m_mapView->calloutData()->setDetail(QString("Number of attachments: %1").arg(attachments.size()));
        m_mapView->calloutData()->setVisible(true); // Resizes the calloutData after details has been set.
      });
    }
  });

  // connect to the applyEditsCompleted signal from the ServiceFeatureTable
  connect(m_featureTable, &ServiceFeatureTable::applyEditsCompleted,
          this, [this](QUuid, const QList<FeatureEditResult*>& featureEditResults)
  {
    // Lock is a convenience wrapper that deletes the contents of featureEditResults when we leave scope.
    FeatureEditListResultLock lock(featureEditResults);

    if (lock.results.length() > 0)
    {
      // obtain the first item in the list
      FeatureEditResult* featureEditResult = lock.results.first();
      // check if there were errors
      if (!featureEditResult->isCompletedWithErrors())
      {
        qDebug() << "Successfully edited feature attachments";

        // update the selected feature with attributes
        QueryParameters queryParams;
        queryParams.setWhereClause(m_whereClause);
        m_featureTable->queryFeatures(queryParams);
      }
      else
      {
        qDebug() << "Apply edits error:" << featureEditResult->error().code() << featureEditResult->error().message();
      }
    }
  });
}

QAbstractListModel* EditFeatureAttachments::attachmentModel() const
{    
  return m_selectedFeature ? m_selectedFeature->attachments() : nullptr;
}

void EditFeatureAttachments::addAttachment(const QUrl& fileUrl, const QString& contentType)
{
  if (QFile::exists(fileUrl.toLocalFile()))
  {
    const QFileInfo fileInfo(fileUrl.path());
    const QString fileName = fileInfo.fileName();

    disconnect(m_attachmentConnection); // disconnect previous connection if necessary

    if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
    {
      QFile file(fileUrl.toLocalFile());
      m_selectedFeature->attachments()->addAttachment(&file, contentType, fileName);
    }
    else
    {
      m_attachmentConnection = connect(m_selectedFeature, &ArcGISFeature::loadStatusChanged,
                                       this, [this, fileUrl, contentType, fileName](Esri::ArcGISRuntime::LoadStatus)
      {
        if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
        {
          QFile file(fileUrl.toLocalFile());
          disconnect(m_attachmentConnection);
          m_selectedFeature->attachments()->addAttachment(&file, contentType, fileName);
        }
      });
      m_selectedFeature->load();
    }
  }
}

void EditFeatureAttachments::deleteAttachment(int index)
{
  disconnect(m_attachmentConnection); // disconnect previous connection if necessary

  if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
  {
    qDebug() << "Attachments size: " << m_selectedFeature->attachments()->rowCount();
    m_selectedFeature->attachments()->deleteAttachment(index);
  }
  else
  {
    m_attachmentConnection = connect(m_selectedFeature, &ArcGISFeature::loadStatusChanged,
                                     this, [this, index](Esri::ArcGISRuntime::LoadStatus)
    {
      if (m_selectedFeature->loadStatus() == LoadStatus::Loaded)
      {
        disconnect(m_attachmentConnection);
        m_selectedFeature->attachments()->deleteAttachment(index);
      }
    });
    m_selectedFeature->load();
  }
}
