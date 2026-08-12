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

// Qt headers
#include <QDebug>
#include <QMutexLocker>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QTimer>

// Other headers
#include "PerfMetricsModel.h"
#include "PerformanceMonitor.h"

static constexpr int PUBLISH_INTERVAL_MS = 1000;
static constexpr qreal OVERLAY_Z = 9999.0;

PerformanceMonitor* PerformanceMonitor::install(QQmlApplicationEngine* engine)
{
  if (!engine || engine->rootObjects().isEmpty())
  {
    qWarning() << "PerformanceMonitor: engine has no root objects; call install() after load()";
    return nullptr;
  }

  auto* window = qobject_cast<QQuickWindow*>(engine->rootObjects().constFirst());
  if (!window)
  {
    qWarning() << "PerformanceMonitor: root object is not a QQuickWindow; overlay not installed";
    return nullptr;
  }

  auto* monitor = new PerformanceMonitor(window);
  monitor->attachWindow(window);

  // A child context keeps perfMonitor out of the root context, which the rest of the app shares.
  auto* context = new QQmlContext(engine->rootContext(), engine);
  context->setContextProperty("perfMonitor", monitor);

  QQmlComponent component(engine, QUrl("qrc:/perf/PerformanceOverlay.qml"));
  QObject* created = component.create(context);
  auto* overlay = qobject_cast<QQuickItem*>(created);
  if (!overlay)
  {
    qWarning() << "PerformanceMonitor: overlay not created:" << (created ? QStringLiteral("root object is not an Item") : component.errorString());
    delete created;
    return monitor;
  }

  QQuickItem* host = window->contentItem();

  const QList<QQuickItem*> hostChildren = host->childItems();
  for (QQuickItem* child : hostChildren)
  {
    if (child->inherits("QQuickOverlay"))
    {
      host = child;
      break;
    }
  }

  overlay->setParent(window);
  overlay->setParentItem(host);
  overlay->setZ(OVERLAY_Z);

  return monitor;
}

PerformanceMonitor::PerformanceMonitor(QObject* parent) :
  QObject(parent),
  m_publishTimer(new QTimer(this)),
  m_metricsModel(new PerfMetricsModel(this))
{
  m_publishTimer->setInterval(PUBLISH_INTERVAL_MS);
  connect(m_publishTimer, &QTimer::timeout, this, &PerformanceMonitor::publishMetrics);
  m_publishTimer->start();
}

PerformanceMonitor::~PerformanceMonitor() = default;

QAbstractItemModel* PerformanceMonitor::metrics() const
{
  return m_metricsModel;
}

void PerformanceMonitor::beginTiming(const QString& name)
{
  m_activeTimings[name].restart();
}

void PerformanceMonitor::endTiming(const QString& name)
{
  const auto it = m_activeTimings.constFind(name);
  if (it == m_activeTimings.constEnd())
  {
    qDebug() << "PerformanceMonitor: endTiming for timing that was never begun:" << name << "- ignored";
    return;
  }

  const double ms = static_cast<double>(it->nsecsElapsed()) / 1.0e6;
  m_activeTimings.erase(it);
  m_metricsModel->upsert(name, ms);
}

void PerformanceMonitor::setValue(const QString& name, double ms)
{
  m_metricsModel->upsert(name, ms);
}

void PerformanceMonitor::clearMetrics()
{
  m_activeTimings.clear();
  m_metricsModel->clear();
}

void PerformanceMonitor::attachWindow(QQuickWindow* window)
{
  m_window = window;
  if (!m_window)
  {
    return;
  }

  // Direct connection: onFrameEnd runs on the render thread, safe only while the window owns this object.
  connect(m_window, &QQuickWindow::afterFrameEnd, this, &PerformanceMonitor::onFrameEnd, Qt::DirectConnection);

  updateRefreshRate();
  connect(m_window, &QQuickWindow::screenChanged, this, &PerformanceMonitor::updateRefreshRate);
}

bool PerformanceMonitor::forceRender() const
{
  return m_forceRender.load(std::memory_order_relaxed);
}

void PerformanceMonitor::setForceRender(bool forceRender)
{
  if (m_forceRender.exchange(forceRender, std::memory_order_relaxed) == forceRender)
  {
    return;
  }

  if (forceRender && m_window)
  {
    m_window->update();
  }

  emit forceRenderChanged();
}

void PerformanceMonitor::onFrameEnd()
{
  {
    const QMutexLocker lock(&m_mutex);

    const qint64 frameDtNs = m_frameTimer.isValid() ? m_frameTimer.nsecsElapsed() : 0;
    m_frameTimer.restart();

    if (frameDtNs > 0)
    {
      m_sumFrameNs += static_cast<quint64>(frameDtNs);
      ++m_frameSamples;
    }
  }

  // Queued rather than a direct call because update() is being requested from the render thread.
  if (m_forceRender.load(std::memory_order_relaxed) && m_window)
  {
    QMetaObject::invokeMethod(m_window, &QQuickWindow::update, Qt::QueuedConnection);
  }
}

void PerformanceMonitor::updateRefreshRate()
{
  const QScreen* screen = m_window ? m_window->screen() : nullptr;
  m_refreshRateHz = screen ? screen->refreshRate() : 0.0;
  emit refreshRateHzChanged();
}

void PerformanceMonitor::publishMetrics()
{
  // Snapshot and reset under the lock, then do the arithmetic unlocked.
  quint64 sumFrameNs = 0;
  int frameSamples = 0;
  {
    const QMutexLocker lock(&m_mutex);
    sumFrameNs = m_sumFrameNs;
    frameSamples = m_frameSamples;

    m_sumFrameNs = 0;
    m_frameSamples = 0;
  }

  m_frameTimeMs = frameSamples > 0 ? (static_cast<double>(sumFrameNs) / frameSamples) / 1.0e6 : 0.0;

  emit metricsChanged();
}
