// Copyright 2026 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QElapsedTimer>
#include <QMutex>
#include <QObject>

class QQuickWindow;
class QTimer;

class PerformanceMonitor : public QObject
{
  Q_OBJECT

  Q_PROPERTY(double fps MEMBER m_fps NOTIFY metricsChanged)
  Q_PROPERTY(double frameTimeMs MEMBER m_frameTimeMs NOTIFY metricsChanged)
  Q_PROPERTY(double syncMs MEMBER m_syncMs NOTIFY metricsChanged)
  Q_PROPERTY(double renderMs MEMBER m_renderMs NOTIFY metricsChanged)
  Q_PROPERTY(double swapMs MEMBER m_swapMs NOTIFY metricsChanged)
  Q_PROPERTY(double refreshRateHz MEMBER m_refreshRateHz NOTIFY refreshRateHzChanged)

public:
  explicit PerformanceMonitor(QObject* parent = nullptr);
  ~PerformanceMonitor() override;

  Q_INVOKABLE void attachWindow(QQuickWindow* window);

signals:
  void metricsChanged();
  void refreshRateHzChanged();

private:
  void onBeforeSynchronizing();
  void onBeforeRendering();
  void onAfterRendering();
  void onFrameSwapped();

  void publishMetrics();

  // Render-thread state. Guarded by m_sampleMutex.
  qint64 m_tBeforeSync = 0;
  qint64 m_tBeforeRender = 0;
  qint64 m_tAfterRender = 0;

  // Accumulated since the last publish
  quint64 m_sumFrameNs = 0;
  int m_frameSamples = 0;
  quint64 m_sumSyncNs = 0;
  quint64 m_sumRenderNs = 0;
  quint64 m_sumSwapNs = 0;
  int m_phaseSamples = 0;

  QElapsedTimer m_frameTimer;
  bool m_frameTimerStarted = false;

  QElapsedTimer m_phaseClock;

  QMutex m_sampleMutex;

  QTimer* m_publishTimer = nullptr;

  QQuickWindow* m_window = nullptr;
  QMetaObject::Connection m_beforeSyncConn;
  QMetaObject::Connection m_beforeRenderConn;
  QMetaObject::Connection m_afterRenderConn;
  QMetaObject::Connection m_frameSwapConn;
  QMetaObject::Connection m_screenChangedConn;

  double m_fps = 0.0;
  double m_frameTimeMs = 0.0;
  double m_syncMs = 0.0;
  double m_renderMs = 0.0;
  double m_swapMs = 0.0;
  double m_refreshRateHz = 0.0;
};

#endif // PERFORMANCEMONITOR_H
