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

package com.esri.samples.accessibility;

import android.content.ComponentCallbacks;
import android.content.Context;
import android.content.res.Configuration;

/**
 * Java-side bridge for Android system text-scale notifications.
 *
 * start() registers this object for Android configuration callbacks. stop() unregisters it and clears the
 * native callback token because the backing C++ AndroidSystemTextScaleMonitor is being destroyed. When Android
 * reports a configuration change, onConfigurationChanged forwards Configuration.fontScale and the token to the
 * registered native C++ callback.
 */
public class SystemTextScaleMonitor implements ComponentCallbacks
{
  private Context m_context;

  /**
   * Opaque token used by C++ to resolve the backing AndroidSystemTextScaleMonitor instance.
   */
  private long m_nativeCallbackToken;

  /**
   * Creates the Java bridge object for the native AndroidSystemTextScaleMonitor instance.
   *
   * @param context The Android application context used to register component callbacks.
   * @param nativeCallbackToken Opaque token used by C++ to resolve the backing AndroidSystemTextScaleMonitor instance.
   */
  public SystemTextScaleMonitor(Context context, long nativeCallbackToken)
  {
    m_context = context;
    m_nativeCallbackToken = nativeCallbackToken;
  }

  public void start()
  {
    if (m_context != null)
    {
      m_context.registerComponentCallbacks(this);
    }
  }

  public void stop()
  {
    if (m_context != null)
    {
      m_context.unregisterComponentCallbacks(this);
    }
    m_nativeCallbackToken = 0;
  }

  // https://developer.android.com/reference/android/content/res/Configuration#fontScale
  // Note: Please do not use this to hardcode font size equations. The equation for font
  // scaling is now non-linear; this coefficient is no longer used as a direct multiplier
  // to determine font size. It exists for informational purposes only.
  @Override public void onConfigurationChanged(Configuration newConfig)
  {
    if (m_nativeCallbackToken != 0)
    {
      textScaleChangedFromJava(m_nativeCallbackToken, newConfig.fontScale);
    }
  }

  // Required by ComponentCallbacks despite deprecation in API 35; text scaling uses onConfigurationChanged instead.
  @SuppressWarnings("deprecation")
  @Override public void onLowMemory()
  {
  }

  /**
   * Native Qt callback invoked when Android configuration changes may affect text scale.
   *
   * @param nativeCallbackToken Opaque token used by C++ to resolve the backing AndroidSystemTextScaleMonitor instance.
   * @param fontScale Current user preference for font scaling from Android's configuration.
   */
  public native void textScaleChangedFromJava(long nativeCallbackToken, float fontScale);
}
