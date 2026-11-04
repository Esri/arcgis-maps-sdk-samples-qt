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
#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>
#include <QMetaObject>
#include <QtGlobal>

// STL headers
#include <array>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace AccessibilitySample
{
  static QJniObject applicationContext()
  {
    const QJniObject context{QNativeInterface::QAndroidApplication::context()};
    if (!context.isValid())
    {
      return {};
    }

    return context.callObjectMethod("getApplicationContext", "()Landroid/content/Context;");
  }

  // AndroidSystemTextScaleMonitor owns the Java SystemTextScaleMonitor bridge used to receive Android
  // configuration changes. create() constructs the C++ monitor, then creates the Java object with the
  // application context and an opaque token that resolves to this C++ monitor. It registers the native callback,
  // then calls Java start().
  // start() registers Android ComponentCallbacks so Java can observe Configuration changes. When Android
  // reports a change, Java passes Configuration.fontScale and the opaque token back to C++; the C++ callback
  // resolves the token through a weak registry and queues the update to this QObject's thread. The destructor
  // invalidates the token and calls Java stop() so Java unregisters those callbacks.
  class AndroidSystemTextScaleMonitor final : public SystemTextScaleMonitor
  {
  public:
    static std::shared_ptr<AndroidSystemTextScaleMonitor> create();

    AndroidSystemTextScaleMonitor();
    ~AndroidSystemTextScaleMonitor() override;

  private:
    Q_DISABLE_COPY_MOVE(AndroidSystemTextScaleMonitor)

    float platformTextScale() const override;
    void applyAndroidFontScale_(float fontScale);
    float queryAndroidFontScale_() const;
    void startJavaBridge_(const std::shared_ptr<AndroidSystemTextScaleMonitor>& textScaleMonitor);
    void stopJavaBridge_();

    static std::mutex& javaCallbackTokenMutex_();
    static std::unordered_map<jlong, std::weak_ptr<AndroidSystemTextScaleMonitor>>& javaCallbackTokens_();
    static jlong nextJavaCallbackToken_();
    static jlong registerJavaCallbackToken_(const std::shared_ptr<AndroidSystemTextScaleMonitor>& textScaleMonitor);
    static void unregisterJavaCallbackToken_(jlong token);
    static std::shared_ptr<AndroidSystemTextScaleMonitor> resolveJavaCallbackToken_(jlong token);
    static void onAndroidTextScaleChanged_(JNIEnv* env, jobject thiz, jlong nativeCallbackToken, jfloat fontScale);

    QJniObject m_javaTextScaleMonitor;
    jlong m_javaCallbackToken = 0;
    float m_platformTextScale = 1.0F;
    bool m_started = false;
  };

  std::mutex& AndroidSystemTextScaleMonitor::javaCallbackTokenMutex_()
  {
    static std::mutex mutex;
    return mutex;
  }

  std::unordered_map<jlong, std::weak_ptr<AndroidSystemTextScaleMonitor>>& AndroidSystemTextScaleMonitor::javaCallbackTokens_()
  {
    static std::unordered_map<jlong, std::weak_ptr<AndroidSystemTextScaleMonitor>> tokens;
    return tokens;
  }

  jlong AndroidSystemTextScaleMonitor::nextJavaCallbackToken_()
  {
    static jlong token = 0;
    return ++token;
  }

  jlong AndroidSystemTextScaleMonitor::registerJavaCallbackToken_(const std::shared_ptr<AndroidSystemTextScaleMonitor>& textScaleMonitor)
  {
    const std::scoped_lock lock(javaCallbackTokenMutex_());
    const auto token = nextJavaCallbackToken_();
    javaCallbackTokens_().emplace(token, textScaleMonitor);
    return token;
  }

  void AndroidSystemTextScaleMonitor::unregisterJavaCallbackToken_(jlong token)
  {
    if (token == 0)
    {
      return;
    }

    const std::scoped_lock lock(javaCallbackTokenMutex_());
    javaCallbackTokens_().erase(token);
  }

  std::shared_ptr<AndroidSystemTextScaleMonitor> AndroidSystemTextScaleMonitor::resolveJavaCallbackToken_(jlong token)
  {
    if (token == 0)
    {
      return nullptr;
    }

    const std::scoped_lock lock(javaCallbackTokenMutex_());
    auto& tokens = javaCallbackTokens_();
    const auto it = tokens.find(token);
    if (it == tokens.end())
    {
      return nullptr;
    }

    auto textScaleMonitor = it->second.lock();
    if (!textScaleMonitor)
    {
      tokens.erase(it);
    }
    return textScaleMonitor;
  }

  AndroidSystemTextScaleMonitor::AndroidSystemTextScaleMonitor() :
    m_platformTextScale(queryAndroidFontScale_())
  {
  }

  std::shared_ptr<AndroidSystemTextScaleMonitor> AndroidSystemTextScaleMonitor::create()
  {
    auto textScaleMonitor = std::make_shared<AndroidSystemTextScaleMonitor>();
    textScaleMonitor->startJavaBridge_(textScaleMonitor);
    return textScaleMonitor;
  }

  void AndroidSystemTextScaleMonitor::startJavaBridge_(const std::shared_ptr<AndroidSystemTextScaleMonitor>& textScaleMonitor)
  {
    m_javaCallbackToken = registerJavaCallbackToken_(textScaleMonitor);
    m_javaTextScaleMonitor = QJniObject("com/esri/samples/accessibility/SystemTextScaleMonitor",
                                        "(Landroid/content/Context;J)V", // JNI signature: function(Context, jlong) -> void.
                                        applicationContext().object(), m_javaCallbackToken);
    if (!m_javaTextScaleMonitor.isValid())
    {
      unregisterJavaCallbackToken_(m_javaCallbackToken);
      m_javaCallbackToken = 0;
      return;
    }

    // Map the Java native method declared by SystemTextScaleMonitor to the C++ callback that implements it.
    // Each JNINativeMethod entry is {Java method name, JNI method signature, native function pointer}.
    const JNINativeMethod textScaleChangedMethod{"textScaleChangedFromJava",
                                                 "(JF)V", // JNI signature: function(jlong, float) -> void.
                                                 reinterpret_cast<void*>(&AndroidSystemTextScaleMonitor::onAndroidTextScaleChanged_)};
    const std::array<JNINativeMethod, 1> methods{textScaleChangedMethod};

    QJniEnvironment env;
    jclass objectClass = env->GetObjectClass(m_javaTextScaleMonitor.object());
    // Install the mapping before SystemTextScaleMonitor.start() can receive configuration callbacks from Java.
    const auto registerStatus = env->RegisterNatives(objectClass, methods.data(), static_cast<jint>(methods.size()));
    if (registerStatus != JNI_OK || env->ExceptionCheck())
    {
      env->ExceptionClear();
      unregisterJavaCallbackToken_(m_javaCallbackToken);
      m_javaCallbackToken = 0;
      return;
    }

    m_javaTextScaleMonitor.callMethod<void>("start");
    if (env->ExceptionCheck())
    {
      env->ExceptionClear();
      unregisterJavaCallbackToken_(m_javaCallbackToken);
      m_javaCallbackToken = 0;
      return;
    }

    m_started = true;
  }

  AndroidSystemTextScaleMonitor::~AndroidSystemTextScaleMonitor()
  {
    stopJavaBridge_();
  }

  void AndroidSystemTextScaleMonitor::stopJavaBridge_()
  {
    unregisterJavaCallbackToken_(m_javaCallbackToken);
    m_javaCallbackToken = 0;

    if (!m_javaTextScaleMonitor.isValid() || !m_started)
    {
      return;
    }

    m_javaTextScaleMonitor.callMethod<void>("stop");
    QJniEnvironment env;
    if (env->ExceptionCheck())
    {
      env->ExceptionClear();
    }
    m_started = false;
  }

  float AndroidSystemTextScaleMonitor::platformTextScale() const
  {
    return m_platformTextScale;
  }

  // Query Android's current Configuration.fontScale value during initialization. Later updates arrive through
  // the Java ComponentCallbacks bridge.
  float AndroidSystemTextScaleMonitor::queryAndroidFontScale_() const
  {
    const auto context = applicationContext();
    if (!context.isValid())
    {
      return defaultTextScale();
    }

    const auto resources = context.callObjectMethod("getResources",
                                                    "()Landroid/content/res/Resources;" // JNI signature: function(/*no args*/) -> Resources.
    );
    if (!resources.isValid())
    {
      return defaultTextScale();
    }

    const auto configuration =
      resources.callObjectMethod("getConfiguration",
                                 "()Landroid/content/res/Configuration;" // JNI signature: function(/*no args*/) -> Configuration.
      );
    if (!configuration.isValid())
    {
      return defaultTextScale();
    }

    return configuration.getField<jfloat>("fontScale");
  }

  void AndroidSystemTextScaleMonitor::applyAndroidFontScale_(float fontScale)
  {
    m_platformTextScale = fontScale;
    refreshTextScaleAndNotifyIfChanged();
  }

  void AndroidSystemTextScaleMonitor::onAndroidTextScaleChanged_(JNIEnv* /*env*/, jobject /*thiz*/, jlong nativeCallbackToken, jfloat fontScale)
  {
    if (auto textScaleMonitor = resolveJavaCallbackToken_(nativeCallbackToken); textScaleMonitor)
    {
      QMetaObject::invokeMethod(
        textScaleMonitor.get(),
        [weakTextScaleMonitor = std::weak_ptr<AndroidSystemTextScaleMonitor>{textScaleMonitor}, platformTextScale = static_cast<float>(fontScale)]()
      {
        if (const auto textScaleMonitor = weakTextScaleMonitor.lock(); textScaleMonitor)
        {
          textScaleMonitor->applyAndroidFontScale_(platformTextScale);
        }
      }, Qt::QueuedConnection);
    }
  }

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createPlatformMonitor()
  {
    return AndroidSystemTextScaleMonitor::create();
  }
} // namespace AccessibilitySample
