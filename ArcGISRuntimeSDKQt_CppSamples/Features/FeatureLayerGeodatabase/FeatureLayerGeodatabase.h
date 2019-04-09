// [WriteFile Name=FeatureLayerGeodatabase, Category=Features]
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

#ifndef FEATURE_LAYER_GEODATABASE_H
#define FEATURE_LAYER_GEODATABASE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class Geodatabase;
  }
}

#include <QQuickItem>

#include "Error.h"

class FeatureLayerGeodatabase : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit FeatureLayerGeodatabase(QQuickItem* parent = nullptr);
  ~FeatureLayerGeodatabase() override;

  void componentComplete() override;
  static void init();

signals:
  void errorMessageChanged();

private slots:
  void logError(Esri::ArcGISRuntime::Error error);

private:
  QString errorMessage() const;
  void setErrorMessage(const QString& msg);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
  QString m_dataPath;
  QString m_errorMsg;
};

#endif // FEATURE_LAYER_GEODATABASE_H

