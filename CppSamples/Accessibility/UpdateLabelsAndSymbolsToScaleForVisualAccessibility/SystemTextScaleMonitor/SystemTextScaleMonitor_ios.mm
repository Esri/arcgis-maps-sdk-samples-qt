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
/// \file SystemTextScaleMonitor_ios.mm

#include "SystemTextScaleMonitor.h"

// STL headers
#include <memory>

// Platform specific headers
#import <UIKit/UIKit.h>

namespace AccessibilitySample
{
  class IosSystemTextScaleMonitor final : public SystemTextScaleMonitor
  {
  public:
    IosSystemTextScaleMonitor();
    ~IosSystemTextScaleMonitor() override;

  private:
    Q_DISABLE_COPY_MOVE(IosSystemTextScaleMonitor)

    float platformTextScale() const override;
    static float textScaleForContentSizeCategory_(UIContentSizeCategory category);

    // Objective-C type: id<NSObject>. ARC is not enabled, so this raw observer token is retained in the constructor
    // and removed/released in the destructor.
    id<NSObject> m_observer = nil;
  };

  static UIContentSizeCategory currentContentSizeCategory()
  {
    UIContentSizeCategory category = UIApplication.sharedApplication.preferredContentSizeCategory;
    return category != nil ? category : UIContentSizeCategoryLarge; // 'Large' is the default size.
  }

  static bool contentSizeCategoryEquals(UIContentSizeCategory category, UIContentSizeCategory expected)
  {
    return [category isEqualToString:expected];
  }

  float IosSystemTextScaleMonitor::textScaleForContentSizeCategory_(UIContentSizeCategory category)
  {
    // Do not shrink the text scale below the default 'Large' size.

    // Check for the default 'Large' content size category first. This is not a switch,
    // so beneficial to avoid superfluous string comparisons for other categories.
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryLarge))
    {
      return defaultTextScale();
    }

    if (contentSizeCategoryEquals(category, UIContentSizeCategoryExtraLarge))
    {
      return 1.11F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryExtraExtraLarge))
    {
      return 1.21F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryExtraExtraExtraLarge))
    {
      return 1.3F;
    }

    // SwiftUI DynamicTypeSize accessibility1...accessibility5 correspond to UIKit's accessibility
    // Medium, Large, ExtraLarge, ExtraExtraLarge, and ExtraExtraExtraLarge categories, respectively.
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryAccessibilityMedium))
    {
      return 1.5F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryAccessibilityLarge))
    {
      return 1.69F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryAccessibilityExtraLarge))
    {
      return 1.86F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryAccessibilityExtraExtraLarge))
    {
      return 2.01F;
    }
    if (contentSizeCategoryEquals(category, UIContentSizeCategoryAccessibilityExtraExtraExtraLarge))
    {
      return 2.15F;
    }

    return defaultTextScale();
  }

  IosSystemTextScaleMonitor::IosSystemTextScaleMonitor()
  {
    IosSystemTextScaleMonitor* textScaleForBlock = this;
    m_observer = [NSNotificationCenter.defaultCenter addObserverForName:UIContentSizeCategoryDidChangeNotification
                                                                 object:nil
                                                                  queue:nil
                                                             usingBlock:^(NSNotification* /*notification*/) {
                                                               textScaleForBlock->refreshTextScaleAndNotifyIfChanged();
                                                             }];
    [m_observer retain];
  }

  IosSystemTextScaleMonitor::~IosSystemTextScaleMonitor()
  {
    if (m_observer != nil)
    {
      [NSNotificationCenter.defaultCenter removeObserver:m_observer];
      [m_observer release];
      m_observer = nil;
    }
  }

  float IosSystemTextScaleMonitor::platformTextScale() const
  {
    return textScaleForContentSizeCategory_(currentContentSizeCategory());
  }

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createPlatformMonitor()
  {
    return std::make_shared<IosSystemTextScaleMonitor>();
  }

} // namespace AccessibilitySample
