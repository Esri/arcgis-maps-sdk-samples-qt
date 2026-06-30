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

#include "PerformanceMonitor.h"

#include <QMutexLocker>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>

PerformanceMonitor::PerformanceMonitor(QObject* parent) :
  QObject(parent),
  m_publishTimer(new QTimer(this))
{
  m_phaseClock.start();

  m_publishTimer->setInterval(1000);
  connect(m_publishTimer, &QTimer::timeout, this, &PerformanceMonitor::publishMetrics);
  m_publishTimer->start();
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::attachWindow(QQuickWindow* window)
{
  if (window == m_window)
  {
    return;
  }

  // Release any prior connections (also safe if they were never made).
  for (const auto& connection : {&m_beforeSyncConn, &m_beforeRenderConn, &m_afterRenderConn, &m_frameSwapConn, &m_screenChangedConn})
  {
    if (*connection)
    {
      disconnect(*connection);
    }
  }

  m_window = window;

  // Reset accumulators so a stale window's frames don't pollute the new stats.
  {
    QMutexLocker lock(&m_sampleMutex);
    m_sumFrameNs = 0;
    m_frameSamples = 0;
    m_sumSyncNs = 0;
    m_sumRenderNs = 0;
    m_sumSwapNs = 0;
    m_phaseSamples = 0;
    m_frameTimerStarted = false;
    m_tBeforeSync = 0;
    m_tBeforeRender = 0;
    m_tAfterRender = 0;
  }

  if (!m_window)
  {
    m_refreshRateHz = 0.0;
    emit refreshRateHzChanged();
    return;
  }

  m_beforeSyncConn = connect(m_window, &QQuickWindow::beforeSynchronizing, this, &PerformanceMonitor::onBeforeSynchronizing, Qt::DirectConnection);
  m_beforeRenderConn = connect(m_window, &QQuickWindow::beforeRendering, this, &PerformanceMonitor::onBeforeRendering, Qt::DirectConnection);
  m_afterRenderConn = connect(m_window, &QQuickWindow::afterRendering, this, &PerformanceMonitor::onAfterRendering, Qt::DirectConnection);
  m_frameSwapConn = connect(m_window, &QQuickWindow::frameSwapped, this, &PerformanceMonitor::onFrameSwapped, Qt::DirectConnection);

  if (auto* screen = m_window->screen())
  {
    m_refreshRateHz = screen->refreshRate();
    emit refreshRateHzChanged();
  }
  m_screenChangedConn = connect(m_window, &QQuickWindow::screenChanged, this, [this](QScreen* screen)
  {
    m_refreshRateHz = screen ? screen->refreshRate() : 0.0;
    emit refreshRateHzChanged();
  });
}

// ---- Render-thread slots ------------------------------------------------

void PerformanceMonitor::onBeforeSynchronizing()
{
  QMutexLocker lock(&m_sampleMutex);
  m_tBeforeSync = m_phaseClock.nsecsElapsed();
}

void PerformanceMonitor::onBeforeRendering()
{
  QMutexLocker lock(&m_sampleMutex);
  m_tBeforeRender = m_phaseClock.nsecsElapsed();
}

void PerformanceMonitor::onAfterRendering()
{
  QMutexLocker lock(&m_sampleMutex);
  m_tAfterRender = m_phaseClock.nsecsElapsed();
}

void PerformanceMonitor::onFrameSwapped()
{
  const qint64 tSwap = m_phaseClock.nsecsElapsed();

  qint64 frameDtNs = 0;
  if (!m_frameTimerStarted)
  {
    m_frameTimer.start();
    m_frameTimerStarted = true;
  }
  else
  {
    frameDtNs = m_frameTimer.nsecsElapsed();
    m_frameTimer.restart();
  }

  QMutexLocker lock(&m_sampleMutex);

  // Deltas. Clamp negatives to zero in case an earlier signal was missed.
  const qint64 syncNs = (m_tBeforeRender > m_tBeforeSync) ? (m_tBeforeRender - m_tBeforeSync) : 0;
  const qint64 renderNs = (m_tAfterRender > m_tBeforeRender) ? (m_tAfterRender - m_tBeforeRender) : 0;
  const qint64 swapNs = (tSwap > m_tAfterRender) ? (tSwap - m_tAfterRender) : 0;

  m_sumSyncNs += static_cast<quint64>(syncNs);
  m_sumRenderNs += static_cast<quint64>(renderNs);
  m_sumSwapNs += static_cast<quint64>(swapNs);
  ++m_phaseSamples;

  if (frameDtNs > 0)
  {
    m_sumFrameNs += static_cast<quint64>(frameDtNs);
    ++m_frameSamples;
  }

  // Request the next frame to prevent idle
  if (m_window)
  {
    m_window->update();
  }
}

void PerformanceMonitor::publishMetrics()
{
  // Snapshot the running sums under the lock, reset them, then process unlocked.
  quint64 sumFrameNs = 0;
  quint64 sumSyncNs = 0;
  quint64 sumRenderNs = 0;
  quint64 sumSwapNs = 0;
  int frameSamples = 0;
  int phaseSamples = 0;
  {
    QMutexLocker lock(&m_sampleMutex);
    sumFrameNs = m_sumFrameNs;
    frameSamples = m_frameSamples;
    sumSyncNs = m_sumSyncNs;
    sumRenderNs = m_sumRenderNs;
    sumSwapNs = m_sumSwapNs;
    phaseSamples = m_phaseSamples;

    m_sumFrameNs = 0;
    m_frameSamples = 0;
    m_sumSyncNs = 0;
    m_sumRenderNs = 0;
    m_sumSwapNs = 0;
    m_phaseSamples = 0;
  }

  if (frameSamples > 0)
  {
    const double meanFrameNs = static_cast<double>(sumFrameNs) / frameSamples;
    m_fps = 1.0e9 / meanFrameNs;
    m_frameTimeMs = meanFrameNs / 1.0e6;
  }
  else
  {
    m_fps = 0.0;
    m_frameTimeMs = 0.0;
  }

  if (phaseSamples > 0)
  {
    m_syncMs = (static_cast<double>(sumSyncNs) / phaseSamples) / 1.0e6;
    m_renderMs = (static_cast<double>(sumRenderNs) / phaseSamples) / 1.0e6;
    m_swapMs = (static_cast<double>(sumSwapNs) / phaseSamples) / 1.0e6;
  }
  else
  {
    m_syncMs = 0.0;
    m_renderMs = 0.0;
    m_swapMs = 0.0;
  }

  emit metricsChanged();
}
