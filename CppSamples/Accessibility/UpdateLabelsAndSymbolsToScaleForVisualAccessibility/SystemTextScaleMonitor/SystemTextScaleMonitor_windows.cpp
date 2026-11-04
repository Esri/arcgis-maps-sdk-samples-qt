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
#include <QAbstractNativeEventFilter>
#include <QCoreApplication>

// STL headers
#include <Windows.h>
#include <memory>

namespace AccessibilitySample
{
  class WindowsSystemTextScaleMonitor final : public SystemTextScaleMonitor, public QAbstractNativeEventFilter
  {
  public:
    WindowsSystemTextScaleMonitor();
    ~WindowsSystemTextScaleMonitor() override;

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

  private:
    Q_DISABLE_COPY_MOVE(WindowsSystemTextScaleMonitor)

    float platformTextScale() const override;
    float textScaleFromPlatformTextScale(float platformTextScale) const override;
  };

  static constexpr DWORD s_windowsDefaultPlatformTextScale = 100;
  static constexpr auto s_windowsPercentScaleFactor = 100.0F;

  WindowsSystemTextScaleMonitor::WindowsSystemTextScaleMonitor()
  {
    if (auto* application = QCoreApplication::instance(); application)
    {
      application->installNativeEventFilter(this);
    }
  }

  WindowsSystemTextScaleMonitor::~WindowsSystemTextScaleMonitor()
  {
    if (auto* application = QCoreApplication::instance(); application)
    {
      application->removeNativeEventFilter(this);
    }
  }

  bool WindowsSystemTextScaleMonitor::nativeEventFilter(const QByteArray& /*eventType*/, void* message, qintptr* /*result*/)
  {
    // https://doc.qt.io/qt-6/qabstractnativeeventfilter.html#nativeEventFilter
    // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-settingchange
    const auto* msg = static_cast<MSG*>(message);
    if (msg && msg->message == WM_SETTINGCHANGE)
    {
      refreshTextScaleAndNotifyIfChanged();
    }

    return false;
  }

  float WindowsSystemTextScaleMonitor::platformTextScale() const
  {
    DWORD textScaleFactor = s_windowsDefaultPlatformTextScale;
    DWORD textScaleFactorSize = sizeof(textScaleFactor);
    const auto status = RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Accessibility", L"TextScaleFactor", RRF_RT_REG_DWORD, nullptr,
                                     &textScaleFactor, &textScaleFactorSize);
    return status == ERROR_SUCCESS ? static_cast<float>(textScaleFactor) : static_cast<float>(s_windowsDefaultPlatformTextScale);
  }

  float WindowsSystemTextScaleMonitor::textScaleFromPlatformTextScale(float platformTextScale) const
  {
    return platformTextScale / s_windowsPercentScaleFactor;
  }

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createPlatformMonitor()
  {
    return std::make_shared<WindowsSystemTextScaleMonitor>();
  }

} // namespace AccessibilitySample
