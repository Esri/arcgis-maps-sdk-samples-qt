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

#ifndef EDIT_FEATURE_ATTACHMENTS_H
#define EDIT_FEATURE_ATTACHMENTS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureLayer;
    class ServiceFeatureTable;
    class ArcGISFeature;
  }
}

class QString;

#include <QAbstractListModel>
#include <QQuickItem>

class EditFeatureAttachments : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(int screenX READ screenX NOTIFY screenXChanged)
  Q_PROPERTY(int screenY READ screenY NOTIFY screenYChanged)
  Q_PROPERTY(int attachmentCount READ attachmentCount NOTIFY attachmentCountChanged)
  Q_PROPERTY(QString featureType READ featureType NOTIFY featureTypeChanged)
  Q_PROPERTY(QAbstractListModel* attachmentModel READ attachmentModel NOTIFY attachmentModelChanged)

public:
  explicit EditFeatureAttachments(QQuickItem* parent = nullptr);
  ~EditFeatureAttachments();

  void componentComplete() Q_DECL_OVERRIDE;
  static void init();
  Q_INVOKABLE void addAttachment(QUrl fileUrl, QString contentType, QString fileName);
  Q_INVOKABLE void deleteAttachment(int index);

signals:
  void screenXChanged();
  void screenYChanged();
  void featureSelected();
  void featureTypeChanged();
  void hideWindow();
  void attachmentModelChanged();
  void attachmentCountChanged();

private:
  void connectSignals();
  QAbstractListModel* attachmentModel() const;
  int screenX() const;
  int screenY() const;
  int attachmentCount() const;
  QString featureType() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;
  int m_screenX = 0;
  int m_screenY = 0;
  int m_attachmentCount = 0;
  QString m_featureType;
  QString m_whereClause;
  QMetaObject::Connection m_attachmentConnection;
};

#endif // EDIT_FEATURE_ATTACHMENTS_H

