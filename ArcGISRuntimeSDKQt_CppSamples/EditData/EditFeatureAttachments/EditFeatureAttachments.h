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

#ifndef EDIT_FEATURE_ATTACHMENTS_H
#define EDIT_FEATURE_ATTACHMENTS_H

#include "MapTypes.h"
#include "MapViewTypes.h"
#include "Map.h"
#include "MapView.h"
#include "ServiceFeatureTable.h"
#include "FeatureLayer.h"
#include "ArcGISFeature.h"
#include "AttachmentInfo.h"
#include "EditFeatureAttachments.h"
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QList>
#include <QSharedPointer>
#include <QMessageBox>

class EditFeatureAttachments : public QWidget
{
  Q_OBJECT

public:
  explicit EditFeatureAttachments(QWidget* parent = 0);
  virtual ~EditFeatureAttachments();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature;
  QList<QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>> m_attachmentInfos;
  QMessageBox* m_messageBox;
  QComboBox* m_comboBox;
  QPushButton* m_addButton;
  QPushButton* m_deleteButton;
  QPushButton* m_viewButton;
  QFileDialog* m_fileDialog;
  void createUi();
  void connectSignals();
  void updateComboBox(const QList<QSharedPointer<Esri::ArcGISRuntime::AttachmentInfo>>&);
};

#endif // EDIT_FEATURE_ATTACHMENTS_H
