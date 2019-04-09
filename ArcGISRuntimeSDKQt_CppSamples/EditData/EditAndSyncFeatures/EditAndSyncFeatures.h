// [WriteFile Name=EditAndSyncFeatures, Category=EditData]
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

#ifndef EDITANDSYNCFEATURES_H
#define EDITANDSYNCFEATURES_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GeodatabaseSyncTask;
    class Geodatabase;
    class ArcGISFeature;
  }
}

#include "GenerateGeodatabaseParameters.h"
#include "SyncGeodatabaseParameters.h"
#include <QQuickItem>
#include <QString>
#include <QUrl>

class EditAndSyncFeatures : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool isOffline READ isOffline NOTIFY isOfflineChanged)

public:
  explicit EditAndSyncFeatures(QQuickItem* parent = nullptr);
  ~EditAndSyncFeatures() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void generateGeodatabaseFromCorners(double xCorner1, double yCorner1, double xCorner2, double yCorner2);
  Q_INVOKABLE void executeSync();

signals:
  void updateStatus(QString status);
  void hideWindow(int time, bool success);
  void updateInstruction(QString instruction);
  void isOfflineChanged();
  void showButton();

private:
  void connectSignals();
  Esri::ArcGISRuntime::GenerateGeodatabaseParameters getGenerateParameters(Esri::ArcGISRuntime::Envelope gdbExtent);
  Esri::ArcGISRuntime::SyncGeodatabaseParameters getSyncParameters();
  void addOfflineData();
  bool isOffline() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GeodatabaseSyncTask* m_syncTask = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_offlineGdb = nullptr;
  Esri::ArcGISRuntime::ArcGISFeature* m_selectedFeature = nullptr;
  QString m_dataPath;
  qint64 m_featureLayerId = 0;
  QString m_featureServiceUrl = QStringLiteral("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Sync/WildfireSync/FeatureServer/");
  bool m_isOffline = false;
};

#endif // EDITANDSYNCFEATURES_H
