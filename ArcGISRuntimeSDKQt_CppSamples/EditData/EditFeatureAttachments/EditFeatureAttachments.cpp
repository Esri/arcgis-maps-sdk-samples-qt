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
#include "MapView.h"
#include "Basemap.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "Point.h"
#include "Feature.h"
#include "QueryParameters.h"
#include "Envelope.h"
#include "AttachmentInfo.h"
#include "AttachmentListModel.h"
#include <QUrl>
#include <QVBoxLayout>
#include <QUuid>
#include <QStringList>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include <QMessageBox>
#include <QImage>

using namespace Esri::ArcGISRuntime;

EditFeatureAttachments::EditFeatureAttachments(QWidget* parent) :
    QWidget(parent),
    m_selectedFeature(nullptr)
{    
    // create a Viewpoint
    Viewpoint vp(Point(14902768,4050890,SpatialReference(3857)), 2e6);

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

    // setup the UI
    createUi();

    // connect to signals
    connectSignals();
}

EditFeatureAttachments::~EditFeatureAttachments()
{
}

void EditFeatureAttachments::connectSignals()
{
    // connect to the mouse press release signal on the MapView
    connect(m_mapView, &MapView::mousePressRelease, [this](QMouseEvent& mouseEvent)
    {
        // first clear the selection
        m_featureLayer->clearSelection();

        // call identify on the map view
        m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 5, 1);
    });

    // connect to the identifyLayerCompleted signal on the map view
    connect(m_mapView, &MapView::identifyLayerCompleted, [this](QUuid, QList<GeoElement*> identifyResults)
    {
        if (!identifyResults.empty())
        {
            // first delete if not nullptr
            if (m_selectedFeature != nullptr)
                delete m_selectedFeature;

            // select the item in the result
            m_featureLayer->selectFeature(static_cast<Feature*>(identifyResults.at(0)));

            // set selected feature member
            m_selectedFeature = static_cast<ArcGISFeature*>(identifyResults.at(0));

            // get attachment info
            m_selectedFeature->attachmentListModel()->fetchAttachmentInfos();

            // update the combo box with attachments once complete
            connect(m_selectedFeature->attachmentListModel(), &AttachmentListModel::fetchAttachmentInfosCompleted, [this](QUuid, const QList<QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>>& attachments)
            {
                updateComboBox(attachments);
            });


            // connect to deleteAttachmentCompleted to know when the delete has occurred successfully
            connect(m_selectedFeature->attachmentListModel(), &AttachmentListModel::deleteAttachmentCompleted, [this](QUuid)
            {
                // call apply edits on the feature table to apply the edits to the service
                m_featureTable->applyEdits();
            });

            // connect to the addAttachmentCompleted signal from the AttachmentListModel
            connect(m_selectedFeature->attachmentListModel(), &AttachmentListModel::addAttachmentCompleted, [this](QUuid, const QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>&)
            {
                // once it is complete, call apply edits on the feature table
                m_featureTable->applyEdits();
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
                m_selectedFeature->attachmentListModel()->fetchAttachmentInfos();
            }
            else
            {
                qDebug() << "Apply edits error:" << featureEditResult->error().code() << featureEditResult->error().description();
            }
        }
    });

    // connect to the button's clicked signal for deleting the selected attachment
    connect(m_deleteButton, &QPushButton::clicked, [this]()
    {
        if (m_selectedFeature != nullptr)
        {
            if (m_comboBox->currentText() != "No Feature Selected" && m_comboBox->currentText() != "No Attachments")
            {
                for (int i = 0; i < m_attachmentInfos.length(); i++)
                {
                    if (m_attachmentInfos.at(i)->name() == m_comboBox->currentText())
                    {
                        m_deleteButton->setEnabled(false);
                        m_addButton->setEnabled(false);
                        m_viewButton->setEnabled(false);

                        // call deleteAttachment
                        m_selectedFeature->attachmentListModel()->deleteAttachment(i);
                    }
                }
            }
        }
    });

    // connect to the button's clicked signal for adding an attachment
    connect(m_addButton, &QPushButton::clicked, [this]()
    {
        if (m_selectedFeature)
            m_fileDialog->show();
    });

    // connect to the button's clicked signal for viewing the selected attachment
    connect(m_viewButton, &QPushButton::clicked, [this]()
    {
        if (m_attachmentInfos.size() > 0)
        {
            if (m_comboBox->currentText() != "No Feature Selected" && m_comboBox->currentText() != "No Attachments")
            {
                AttachmentInfo* attachment1 = m_attachmentInfos[m_comboBox->currentIndex()].data();
                attachment1->fetchData();

                if (m_fetchDataConnection)
                    disconnect(m_fetchDataConnection);

                m_fetchDataConnection = connect(attachment1, &AttachmentInfo::fetchDataCompleted, [this](QUuid, const QByteArray& data)
                {
                    QImage image = QImage::fromData(data);
                    QPixmap pm = QPixmap::fromImage(image);
                    m_messageBox->setWindowTitle(m_comboBox->currentText());
                    m_messageBox->setIconPixmap(pm);
                    m_messageBox->show();
                });
            }
        }
    });

    // connect to the accepted signal from the file dialog
    connect(m_fileDialog, &QFileDialog::accepted, [this]()
    {
        if (m_fileDialog->selectedFiles().size() > 0)
        {
            QFile* fileToAdd = new QFile(m_fileDialog->selectedFiles().at(0), this);
            QFileInfo fileInfo(m_fileDialog->selectedFiles().at(0));
            QString fileName =fileInfo.fileName();
            QString contentType = "image/" + fileInfo.completeSuffix();

            m_deleteButton->setEnabled(false);
            m_addButton->setEnabled(false);
            m_viewButton->setEnabled(false);

            m_selectedFeature->attachmentListModel()->addAttachment(fileToAdd, contentType, fileName);
        }
    });
}

void EditFeatureAttachments::updateComboBox(const QList<QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>>& attachments)
{
    m_attachmentInfos.clear();
    m_attachmentInfos = attachments;
    m_deleteButton->setEnabled(true);
    m_addButton->setEnabled(true);
    m_viewButton->setEnabled(true);
    m_comboBox->clear();

    QStringList comboBoxItems;
    for (int i = 0; i < m_attachmentInfos.size(); i++)
    {
        comboBoxItems << m_attachmentInfos.at(i)->name();
    }
    if (comboBoxItems.size() == 0)
        comboBoxItems << "No Attachments";

    m_comboBox->addItems(comboBoxItems);
}

void EditFeatureAttachments::createUi()
{    
    // Create the combo box to list the attachments
    m_comboBox = new QComboBox(this);
    m_comboBox->setStyleSheet("QComboBox#text {color: black;}");
    m_comboBox->addItems(QStringList() << "No Feature Selected");

    // Create the button to delete the current attachment
    m_deleteButton = new QPushButton("Remove Attachment", this);
    m_deleteButton->setStyleSheet("QPushbutton#text {color: black;}");

    // Create the button to add a new attachment
    m_addButton = new QPushButton("Add Attachment", this);
    m_addButton->setStyleSheet("QPushbutton#text {color: black;}");

    // Create the button to view the attachment
    m_viewButton = new QPushButton("View Attachment", this);
    m_viewButton->setStyleSheet("QPushbutton#text {color: black;}");

    // Create the layout
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->addWidget(m_comboBox);
    layout->addWidget(m_addButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_viewButton);
    widget->setPalette(QPalette(QPalette::Base));
    widget->setLayout(layout);
    QGraphicsProxyWidget *proxy = m_mapView->scene()->addWidget(widget);
    proxy->setPos(10, 10);
    proxy->setOpacity(0.95);
    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(m_mapView);
    setLayout(vBoxLayout);

    // Create the file dialog for adding new attachments
    m_fileDialog = new QFileDialog(this);
    m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
    m_fileDialog->setNameFilter(tr("Images (*.png *.jpeg *.jpg *.tiff *.bmp)"));

    // Create the Message Box
    m_messageBox = new QMessageBox(this);
    m_messageBox->setStandardButtons(QMessageBox::StandardButton::Close);
}
