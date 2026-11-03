// [WriteFile Name=FilterMosaicDatasetRasterByTimeExtent, Category=DisplayInformation]
// [Legal]
// Copyright 2026 Esri.
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

#ifndef FILTERMOSAICDATASETRASTERBYTIMEEXTENT_H
#define FILTERMOSAICDATASETRASTERBYTIMEEXTENT_H

// Qt headers
#include <QDateTime>
#include <QList>
#include <QObject>

namespace Esri::ArcGISRuntime
{
  class Map;
  class MapQuickView;
  class MosaicDatasetRaster;
  class RasterLayer;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h")

class FilterMosaicDatasetRasterByTimeExtent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(int timestampCount READ timestampCount NOTIFY timestampsChanged)
  Q_PROPERTY(QString currentTimestamp READ currentTimestamp NOTIFY currentIndexChanged)
  Q_PROPERTY(bool loading MEMBER m_loading NOTIFY loadingChanged)
  Q_PROPERTY(QString loadError MEMBER m_loadError NOTIFY loadingChanged)

public:
  explicit FilterMosaicDatasetRasterByTimeExtent(QObject* parent = nullptr);
  ~FilterMosaicDatasetRasterByTimeExtent() override;

  static void init();

  Q_INVOKABLE void setRasterOpacity(double opacity);

signals:
  void mapViewChanged();
  void currentIndexChanged();
  void timestampsChanged();
  void loadingChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  int currentIndex() const;
  void setCurrentIndex(int index);
  int timestampCount() const;
  QString currentTimestamp() const;

  void finishLoading();
  void failLoading();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MosaicDatasetRaster* m_mosaicDatasetRaster = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_rasterLayer = nullptr;
  QList<QDateTime> m_timestamps;
  int m_currentIndex = -1;
  bool m_loading = true;
  QString m_loadError;
};

#endif // FILTERMOSAICDATASETRASTERBYTIMEEXTENT_H
