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

#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

class PerfMetricsModel;
class QQmlApplicationEngine;
class QQuickWindow;
class QTimer;

#include <QAbstractItemModel>
#include <QElapsedTimer>
#include <QHash>
#include <QMutex>
#include <QObject>

#include <atomic>

// Passive frame-timing overlay for any Qt Quick app, compiled only in CONFIG+=perfmonitor builds.
class PerformanceMonitor : public QObject
{
  Q_OBJECT

  // FPS is deliberately not a property: the overlay derives it from frameTimeMs so the two cannot drift.
  Q_PROPERTY(double frameTimeMs MEMBER m_frameTimeMs NOTIFY metricsChanged)
  // App-reported metrics, one row per name, in first-report order.
  Q_PROPERTY(QAbstractItemModel* metrics READ metrics CONSTANT)
  Q_PROPERTY(double refreshRateHz MEMBER m_refreshRateHz NOTIFY refreshRateHzChanged)
  Q_PROPERTY(bool forceRender READ forceRender WRITE setForceRender NOTIFY forceRenderChanged)

public:
  // Call after QQmlApplicationEngine::load(); the returned monitor is owned by the engine's root window.
  static PerformanceMonitor* install(QQmlApplicationEngine* engine);

  ~PerformanceMonitor() override;

  // App-facing API, GUI thread only; names double as overlay row labels.
  Q_INVOKABLE void beginTiming(const QString& name);
  Q_INVOKABLE void endTiming(const QString& name);
  Q_INVOKABLE void setValue(const QString& name, double ms);
  Q_INVOKABLE void clearMetrics();

  QAbstractItemModel* metrics() const;

signals:
  void metricsChanged();
  void refreshRateHzChanged();
  void forceRenderChanged();

private:
  Q_DISABLE_COPY_MOVE(PerformanceMonitor)

  explicit PerformanceMonitor(QObject* parent = nullptr);

  void attachWindow(QQuickWindow* window);

  bool forceRender() const;
  void setForceRender(bool forceRender);

  // Runs on the render thread.
  void onFrameEnd();

  void updateRefreshRate();
  void publishMetrics();

  // Frame timing, written on the render thread and drained on the GUI thread under m_mutex.
  QMutex m_mutex;
  quint64 m_sumFrameNs = 0;
  int m_frameSamples = 0;
  QElapsedTimer m_frameTimer;

  // Read on the render thread, written on the GUI thread.
  std::atomic<bool> m_forceRender{false};

  QTimer* m_publishTimer = nullptr;
  QQuickWindow* m_window = nullptr;

  // Active timings and reported metrics, GUI thread only.
  QHash<QString, QElapsedTimer> m_activeTimings;
  PerfMetricsModel* m_metricsModel = nullptr;

  double m_frameTimeMs = 0.0;
  double m_refreshRateHz = 0.0;
};

#endif // PERFORMANCEMONITOR_H
