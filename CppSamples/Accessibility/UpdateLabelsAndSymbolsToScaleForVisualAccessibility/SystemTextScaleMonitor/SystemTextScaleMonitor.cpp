// [WriteFile Name=UpdateLabelsAndSymbolsToScaleForVisualAccessibility, Category=Accessibility]
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

// qt_common headers
#include "SystemTextScaleMonitor.h"

// Qt headers
#include <QtGlobal>

// STL headers
#include <cmath>
#include <memory>
#include <mutex>

namespace AccessibilitySample
{

  SystemTextScaleMonitor::SystemTextScaleMonitor() = default;

  SystemTextScaleMonitor::~SystemTextScaleMonitor() = default;

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::sharedInstance()
  {
    static std::mutex s_sharedInstanceMutex;
    static std::weak_ptr<SystemTextScaleMonitor> s_sharedWeakInstance;

    const std::scoped_lock lock(s_sharedInstanceMutex);
    auto sharedInstance = s_sharedWeakInstance.lock();
    if (!sharedInstance)
    {
      sharedInstance = createInitialized();
      s_sharedWeakInstance = sharedInstance;
    }

    return sharedInstance;
  }

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createInitialized()
  {
    auto monitor = createPlatformMonitor();
    // This queries virtual platform hooks, so it must run after the concrete monitor is fully constructed.
    monitor->initializeTextScale();
    return monitor;
  }

  float SystemTextScaleMonitor::textScale() const
  {
    return m_textScale;
  }

  void SystemTextScaleMonitor::initializeTextScale()
  {
    m_textScale = queryTextScale_();
  }

  void SystemTextScaleMonitor::refreshTextScaleAndNotifyIfChanged()
  {
    const auto newTextScale = queryTextScale_();
    if (qFuzzyCompare(m_textScale, newTextScale))
    {
      return;
    }

    m_textScale = newTextScale;
    emit textScaleChanged(m_textScale);
  }

  float SystemTextScaleMonitor::textScaleFromPlatformTextScale(float platformTextScale) const
  {
    return platformTextScale;
  }

  float SystemTextScaleMonitor::defaultTextScale()
  {
    return 1.0F;
  }

  float SystemTextScaleMonitor::queryTextScale_() const
  {
    // Convert the platform value to a text-scale multiplier.
    const auto convertedTextScale = textScaleFromPlatformTextScale(platformTextScale());

    // apply the default if the converted value is not in the documented range of valid values.
    return std::isfinite(convertedTextScale) && convertedTextScale > 0.0F ? convertedTextScale : defaultTextScale();
  }

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID) && !defined(Q_OS_WIN) && !defined(Q_OS_LINUX)
  namespace
  {
    class UnsupportedSystemTextScaleMonitor final : public SystemTextScaleMonitor
    {
    public:
      UnsupportedSystemTextScaleMonitor() = default;

    protected:
      float platformTextScale() const override
      {
        return defaultTextScale();
      }
    };
  } // namespace

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createPlatformMonitor()
  {
    return std::make_shared<UnsupportedSystemTextScaleMonitor>();
  }
#endif

} // namespace AccessibilitySample
