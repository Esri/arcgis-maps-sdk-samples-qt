// [WriteFile Name=BrowseOGCAPIFeatureService, Category=Layers]
// [Legal]
// Copyright 2021 Esri.

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

#ifndef BROWSEOGCAPIFEATURESERVICE_H
#define BROWSEOGCAPIFEATURESERVICE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Error;
class FeatureLayer;
class Map;
class MapQuickView;
class OgcFeatureCollectionTable;
class OgcFeatureCollectionInfo;
class OgcFeatureService;
class OgcFeatureServiceInfo;
}
}

#include <QObject>
#include <QUrl>

class BrowseOGCAPIFeatureService : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage NOTIFY errorMessageChanged)
  Q_PROPERTY(QUrl featureServiceUrl READ featureServiceUrl NOTIFY urlChanged)
  Q_PROPERTY(QStringList featureCollectionList READ featureCollectionList NOTIFY featureCollectionListChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
  explicit BrowseOGCAPIFeatureService(QObject* parent = nullptr);
  ~BrowseOGCAPIFeatureService();

  static void init();

  Q_INVOKABLE void loadService(const QUrl& urlFromInterface);
  Q_INVOKABLE void loadFeatureCollection(int selectedFeature);

signals:
  void mapViewChanged();
  void errorMessageChanged();
  void urlChanged();
  void featureCollectionListChanged();
  void loadingChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QUrl featureServiceUrl() const;
  QStringList featureCollectionList() const;
  QString errorMessage() const;
  void setErrorMessage(const QString& message);
  void handleError(const Esri::ArcGISRuntime::Error& error);
  void loadFeatureService(const QUrl& url);
  void clearExistingFeatureService();
  void retrieveCollectionInfos();
  void createFeatureCollectionList();
  void clearExistingFeatureLayer();
  void addFeatureLayerToMap();
  bool loading() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QString m_errorMessage = "";
  QUrl m_featureServiceUrl;
  Esri::ArcGISRuntime::OgcFeatureService* m_featureService = nullptr;
  Esri::ArcGISRuntime::OgcFeatureServiceInfo* m_serviceInfo = nullptr;
  QList<Esri::ArcGISRuntime::OgcFeatureCollectionInfo*> m_collectionInfo;
  QStringList m_featureCollectionList;
  Esri::ArcGISRuntime::OgcFeatureCollectionTable* m_featureCollectionTable = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_featureLayer = nullptr;
  bool isLoading = false;
};

#endif // BROWSEOGCAPIFEATURESERVICE_H
