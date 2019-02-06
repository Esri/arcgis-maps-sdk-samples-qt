// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
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

#ifndef DISPLAYLAYERVIEWDRAWSTATE_H
#define DISPLAYLAYERVIEWDRAWSTATE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class FeatureLayer;
    class ServiceFeatureTable;
    class ArcGISTiledLayer;
    class ArcGISMapImageLayer;
    class LayerListModel;
  }
}

#include <QMetaObject>
#include <QQuickItem>

class DisplayLayerViewDrawState : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(const QList<QObject*>& statusModel READ statusModel NOTIFY modelChanged)

public:
  explicit DisplayLayerViewDrawState(QQuickItem* parent = nullptr);
  ~DisplayLayerViewDrawState() override;

  void componentComplete() override;
  static void init();

signals:
  void modelChanged();

private:
  const QList<QObject*>& statusModel() const;
  void addLayers();
  void connectSignals();

private:
  QList<QObject*> m_statuses;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_imageLayer = nullptr;
  Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer = nullptr;
  Esri::ArcGISRuntime::ServiceFeatureTable* m_featureTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
};

class DisplayItem : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)

public:
  DisplayItem(QObject* parent = nullptr);
  DisplayItem(const QString& name, const QString& status, QObject* parent = nullptr);
  ~DisplayItem() override;

  void setName(const QString& name);
  QString name() const;

  void setStatus(const QString& status);
  QString status() const;

signals:
  void nameChanged();
  void statusChanged();

private:
  QString m_name;
  QString m_status;
};

#endif // DISPLAYLAYERVIEWDRAWSTATE_H
