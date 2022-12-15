// [WriteFile Name=GenerateOfflineMap, Category=Maps]
// [Legal]
// Copyright 2017 Esri.

#ifndef GENERATEOFFLINEMAP_H
#define GENERATEOFFLINEMAP_H

namespace Esri::ArcGISRuntime
{
class Map;
class MapQuickView;
class OfflineMapTask;
}

#include <QQuickItem>
#include <QTemporaryDir>

class GenerateOfflineMap : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool mapLoaded MEMBER m_mapLoaded NOTIFY mapLoadedChanged)

public:
  explicit GenerateOfflineMap(QQuickItem* parent = nullptr);
  ~GenerateOfflineMap() override = default;

  void componentComplete() override;
  static void init();

public:
  Q_INVOKABLE void generateMapByExtent(double xCorner1, double yCorner1, double xCorner2, double yCorner2);

signals:
  void mapLoadedChanged();
  void hideWindow(int time, bool success);
  void updateStatus(const QString& status);
  void updateProgress(int progress);
  void showLayerErrors(const QString& error);

private:
  static const QString webMapId() { return s_webMapId; }

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  static const QString s_webMapId;
  bool m_mapLoaded = false;
  QTemporaryDir m_tempPath;
};

#endif // GENERATEOFFLINEMAP_H
