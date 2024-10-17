// [WriteFile Name=SearchForWebmap, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef SEARCHFORWEBMAP_H
#define SEARCHFORWEBMAP_H

// Qt headers
#include <QAbstractListModel>
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class Portal;
  class PortalItem;
  class PortalItemListModel;
  class PortalQueryResultSetForItems;
}

class SearchForWebmap : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool portalLoaded READ portalLoaded NOTIFY portalLoadedChanged)
  Q_PROPERTY(QAbstractListModel* webmaps READ webmaps NOTIFY webmapsChanged)
  Q_PROPERTY(bool hasMoreResults READ hasMoreResults NOTIFY hasMoreResultsChanged)
  Q_PROPERTY(QString mapLoadError READ mapLoadError NOTIFY mapLoadErrorChanged)

public:
  explicit SearchForWebmap(QQuickItem* parent = nullptr);
  ~SearchForWebmap() override;

  void componentComplete() override;
  static void init();

  bool portalLoaded() const;
  QAbstractListModel* webmaps() const;
  bool hasMoreResults() const;
  QString mapLoadError() const;

  Q_INVOKABLE void search(const QString& keyword);
  Q_INVOKABLE void searchNext();
  Q_INVOKABLE void loadSelectedWebmap(int index);
  Q_INVOKABLE void errorAccepted();

signals:
  void portalLoadedChanged();
  void webmapsChanged();
  void hasMoreResultsChanged();
  void mapLoadErrorChanged();

private:
  void onFindItemsCompleted(Esri::ArcGISRuntime::PortalQueryResultSetForItems* webmapResults);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Portal* m_portal = nullptr;
  Esri::ArcGISRuntime::PortalQueryResultSetForItems* m_webmapResults = nullptr;
  Esri::ArcGISRuntime::PortalItemListModel* m_webmaps = nullptr;
  Esri::ArcGISRuntime::PortalItem* m_selectedItem = nullptr;
  bool m_portalLoaded = false;
  QString m_mapLoadeError;
};

#endif // SEARCHFORWEBMAP_H
