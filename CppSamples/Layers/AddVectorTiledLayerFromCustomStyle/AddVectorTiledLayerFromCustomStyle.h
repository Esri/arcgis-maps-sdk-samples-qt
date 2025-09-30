// [WriteFile Name=AddVectorTiledLayerFromCustomStyle, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef ADDVECTORTILEDLAYERFROMCUSTOMSTYLE_H
#define ADDVECTORTILEDLAYERFROMCUSTOMSTYLE_H

#include <QTemporaryDir>

namespace Esri::ArcGISRuntime
{
  class ExportVectorTilesJob;
  class ExportVectorTilesResult;
  class ItemResourceCache;
  class Map;
  class MapQuickView;
  class PortalItem;
  class Viewpoint;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("MapQuickView.h");

class AddVectorTiledLayerFromCustomStyle : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QStringList styleNames READ styleNames NOTIFY styleNamesChanged)

public:
  explicit AddVectorTiledLayerFromCustomStyle(QObject* parent = nullptr);
  ~AddVectorTiledLayerFromCustomStyle() override;

  static void init();
  Q_INVOKABLE void setStyle(const QString& style);

signals:
  void mapViewChanged();
  void styleNamesChanged();
  void styleExported(const QString& styleName, Esri::ArcGISRuntime::ItemResourceCache* cache);

private:
  void exportStyle(Esri::ArcGISRuntime::PortalItem* vectorTiledLayer, const QString& styleName);
  void setupPortalItemsAndExportStyles();
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void populatePortalIdMap();
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QStringList styleNames() const;

  const QString m_dataPath;
  Esri::ArcGISRuntime::ExportVectorTilesResult* m_darkVectorTilesResult = nullptr;
  Esri::ArcGISRuntime::ItemResourceCache* m_darkStyleResourceCache = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesJob* m_exportJob = nullptr;
  Esri::ArcGISRuntime::ItemResourceCache* m_lightStyleResourceCache = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesResult* m_lightVectorTilesResult = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QMap<QString, QString> m_portalIds;
  QStringList m_styleNames;
  QTemporaryDir m_tempDir;
  QMap<QString, Esri::ArcGISRuntime::PortalItem*> m_vectorTiledLayers;
};

#endif // ADDVECTORTILEDLAYERFROMCUSTOMSTYLE_H
