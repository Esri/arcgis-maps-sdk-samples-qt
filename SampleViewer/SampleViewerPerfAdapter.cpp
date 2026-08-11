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

#ifdef PERFORMANCE_MONITOR

#include "pch.hpp"

#include "SampleViewerPerfAdapter.h"

#include "PerformanceMonitor.h"
#include "SampleManager.h"

// ArcGIS Maps SDK headers
#include "LocalSceneQuickView.h"
#include "MapQuickView.h"
#include "SceneQuickView.h"

// Qt headers
#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>
#include <QVariant>

using namespace Esri::ArcGISRuntime;

static const QString DRAW_SPAN = QStringLiteral("post-nav draw");
static const QString INITIAL_LOAD_SPAN = QStringLiteral("initial load");

static constexpr int ATTACH_RETRY_MS = 100;
static constexpr int ATTACH_MAX_ATTEMPTS = 50;

// Walks the visual tree, not the QObject tree.
template<typename ViewType>
static ViewType* findViewInTree(QQuickItem* item)
{
  if (!item)
  {
    return nullptr;
  }

  if (auto* match = qobject_cast<ViewType*>(item))
  {
    return match;
  }

  const QList<QQuickItem*> children = item->childItems();
  for (QQuickItem* child : children)
  {
    if (auto* match = findViewInTree<ViewType>(child))
    {
      return match;
    }
  }

  return nullptr;
}

SampleViewerPerfAdapter::SampleViewerPerfAdapter(PerformanceMonitor* monitor, SampleManager* sampleManager, QQuickWindow* window, QObject* parent) :
  QObject(parent),
  m_monitor(monitor),
  m_sampleManager(sampleManager),
  m_window(window)
{
  if (!m_monitor || !m_sampleManager)
  {
    qWarning() << "SampleViewerPerfAdapter: monitor or SampleManager unavailable; sample metrics will not be reported";
    return;
  }

  connect(m_sampleManager, &SampleManager::currentSampleChanged, this, &SampleViewerPerfAdapter::onSampleChanged);
  connect(m_sampleManager, &SampleManager::currentModeChanged, this, &SampleViewerPerfAdapter::onModeChanged);
}

void SampleViewerPerfAdapter::onSampleChanged()
{
  m_monitor->clearMetrics();
  m_monitor->beginSpan(INITIAL_LOAD_SPAN);
  m_loadPending = true;
  m_initialDrawStarted = false;
  m_navigating = false;
  m_drawOpen = false;

  m_staleView = m_attachedView;
  m_attachedView.clear();
  m_attachAttempts = 0;

  disconnect(m_drawStatusConn);
  disconnect(m_navigationConn);

  QTimer::singleShot(0, this, &SampleViewerPerfAdapter::attachToGeoView);
}

void SampleViewerPerfAdapter::onModeChanged()
{
  if (!m_loadPending)
  {
    return;
  }

  // currentMode() is private (Q_PROPERTY-only), so read it the way QML does.
  const QVariant mode = m_sampleManager->property("currentMode");
  if (!mode.isValid())
  {
    qWarning() << "SampleViewerPerfAdapter: SampleManager has no currentMode property; re-arm disabled";
    return;
  }

  if (mode.value<SampleManager::CurrentMode>() != SampleManager::LiveSampleView)
  {
    return;
  }

  // The sample Loader gets its source only after data is in place, so restarting here excludes download time.
  m_monitor->beginSpan(INITIAL_LOAD_SPAN);
  m_initialDrawStarted = false;
  m_navigating = false;
  m_drawOpen = false;
  m_attachAttempts = 0;
  disconnect(m_drawStatusConn);
  disconnect(m_navigationConn);

  QTimer::singleShot(0, this, &SampleViewerPerfAdapter::attachToGeoView);
}

void SampleViewerPerfAdapter::attachToGeoView()
{
  if (!m_window || m_attachedView)
  {
    return;
  }

  // Two queued attach attempts can land in one event-loop turn; never stack connections.
  disconnect(m_drawStatusConn);
  disconnect(m_navigationConn);

  QQuickItem* content = m_window->contentItem();

  if (!attachIfFound<MapQuickView>(content) && !attachIfFound<SceneQuickView>(content) && !attachIfFound<LocalSceneQuickView>(content))
  {
    if (m_loadPending && ++m_attachAttempts < ATTACH_MAX_ATTEMPTS)
    {
      QTimer::singleShot(ATTACH_RETRY_MS, this, &SampleViewerPerfAdapter::attachToGeoView);
      return;
    }

    qDebug() << "SampleViewerPerfAdapter: no GeoView found for this sample";
  }
}

template<typename ViewType>
bool SampleViewerPerfAdapter::attachIfFound(QQuickItem* content)
{
  auto* view = findViewInTree<ViewType>(content);

  if (!view || view == m_staleView)
  {
    return false;
  }

  m_attachedView = view;
  m_navigating = view->isNavigating();
  m_drawStatusConn = connect(view, &ViewType::drawStatusChanged, this, &SampleViewerPerfAdapter::onDrawStatusChanged);
  m_navigationConn = connect(view, &ViewType::navigatingChanged, this, [this, view]()
  {
    onNavigationChanged(view->isNavigating(), view->drawStatus());
  });
  onDrawStatusChanged(view->drawStatus());
  return true;
}

void SampleViewerPerfAdapter::onNavigationChanged(bool navigating, DrawStatus drawStatus)
{
  m_navigating = navigating;

  if (m_loadPending)
  {
    return;
  }

  if (m_navigating)
  {
    m_drawOpen = false;
    m_monitor->setValue(DRAW_SPAN, 0.0);
    return;
  }

  if (drawStatus == DrawStatus::InProgress)
  {
    m_monitor->beginSpan(DRAW_SPAN);
    m_drawOpen = true;
  }
}

void SampleViewerPerfAdapter::onDrawStatusChanged(DrawStatus status)
{
  if (status == DrawStatus::InProgress)
  {
    if (m_loadPending)
    {
      m_initialDrawStarted = true;
    }
    else if (!m_navigating)
    {
      m_monitor->beginSpan(DRAW_SPAN);
      m_drawOpen = true;
    }
    return;
  }

  if (status != DrawStatus::Completed)
  {
    return;
  }

  if (m_drawOpen)
  {
    m_drawOpen = false;
    m_monitor->endSpan(DRAW_SPAN);
  }

  if (m_loadPending && m_initialDrawStarted)
  {
    m_loadPending = false;
    m_initialDrawStarted = false;
    m_monitor->endSpan(INITIAL_LOAD_SPAN);
  }
}

#endif // PERFORMANCE_MONITOR
