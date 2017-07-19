// [WriteFile Name=LocalServerFeatureLayer, Category=LocalServer]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef LOCAL_SERVER_GEOPROCESSING_H
#define LOCAL_SERVER_GEOPROCESSING_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class LocalServer;
class LocalGeoprocessingService;
class ArcGISTiledLayer;
class GeoprocessingTask;
}
}

#include <QQuickItem>
#include <QStringListModel>

class LocalServerGeoprocessing : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(double interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:
  explicit LocalServerGeoprocessing(QQuickItem* parent = nullptr);
  ~LocalServerGeoprocessing();

  void componentComplete() Q_DECL_OVERRIDE;
  Q_INVOKABLE void generateContours();
  Q_INVOKABLE void clearResults();

signals:
  void intervalChanged();

private:
  void connectSignals();
  double interval() { return m_interval; }
  void setInterval(const double& value);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer = nullptr;
  Esri::ArcGISRuntime::LocalGeoprocessingService* m_localGPService = nullptr;
  Esri::ArcGISRuntime::GeoprocessingTask* m_gpTask;

  double m_interval = 0.0;
};

#endif // LOCAL_SERVER_GEOPROCESSING_H

