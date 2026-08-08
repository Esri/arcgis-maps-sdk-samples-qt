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

#ifndef SAMPLEVIEWERPERFADAPTER_H
#define SAMPLEVIEWERPERFADAPTER_H

#ifdef PERFORMANCE_MONITOR

class PerformanceMonitor;
class QQuickItem;
class QQuickWindow;
class SampleManager;

// ArcGIS Maps SDK headers
#include "MapViewTypes.h"

#include <QMetaObject>
#include <QObject>

// Feeds Sample Viewer events (sample changes, GeoView draw status) into the monitor's generic span API.
// App code, not module code: compiled from SampleViewer.pro's perfmonitor block only.
class SampleViewerPerfAdapter : public QObject
{
  Q_OBJECT

public:
  SampleViewerPerfAdapter(PerformanceMonitor* monitor, SampleManager* sampleManager, QQuickWindow* window, QObject* parent = nullptr);

private:
  Q_DISABLE_COPY_MOVE(SampleViewerPerfAdapter)

  void onSampleChanged();
  void onModeChanged();
  void attachToGeoView();
  template<typename ViewType>
  bool attachIfFound(QQuickItem* content);
  void onDrawStatusChanged(Esri::ArcGISRuntime::DrawStatus status);

  PerformanceMonitor* m_monitor = nullptr;
  SampleManager* m_sampleManager = nullptr;
  QQuickWindow* m_window = nullptr;
  QMetaObject::Connection m_drawStatusConn;

  // Set on sample change, cleared by the first completed draw; gates the re-arm in onModeChanged.
  bool m_loadPending = false;

  // GeoViews repeat Completed without a fresh InProgress; only the first one closes a draw cycle.
  bool m_drawOpen = false;
};

#endif // PERFORMANCE_MONITOR

#endif // SAMPLEVIEWERPERFADAPTER_H
