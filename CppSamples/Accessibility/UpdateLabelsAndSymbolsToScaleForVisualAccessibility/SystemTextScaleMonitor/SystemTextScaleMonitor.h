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

#ifndef AccessibilitySample_SystemTextScaleMonitor_H
#define AccessibilitySample_SystemTextScaleMonitor_H

// Qt headers
#include <QObject>

// STL headers
#include <memory>

namespace AccessibilitySample
{
  // Shared sample helper that reports the current system text scale as a positive multiplier.

  class SystemTextScaleMonitor : public QObject
  {
    Q_OBJECT

  public:
    ~SystemTextScaleMonitor() override;

    // Returns a shared monitor for tracking system text-size changes.
    [[nodiscard]] static std::shared_ptr<SystemTextScaleMonitor> sharedInstance();

    float textScale() const;

  signals:
    void textScaleChanged(float textScale);

  private:
    Q_DISABLE_COPY_MOVE(SystemTextScaleMonitor)

  protected:
    SystemTextScaleMonitor();

    void refreshTextScaleAndNotifyIfChanged();

    // OS-native value, before conversion to a text-scale multiplier.
    virtual float platformTextScale() const = 0;

    // Converts the OS-native value to a multiplier where 1.0 means no text-size change.
    virtual float textScaleFromPlatformTextScale(float platformTextScale) const;

    static float defaultTextScale();

  private:
    static std::shared_ptr<SystemTextScaleMonitor> createInitialized();
    static std::shared_ptr<SystemTextScaleMonitor> createPlatformMonitor();

    void initializeTextScale();

    float queryTextScale_() const;

    float m_textScale = 1.0F;
  };

} // namespace AccessibilitySample

#endif // AccessibilitySample_SystemTextScaleMonitor_H
