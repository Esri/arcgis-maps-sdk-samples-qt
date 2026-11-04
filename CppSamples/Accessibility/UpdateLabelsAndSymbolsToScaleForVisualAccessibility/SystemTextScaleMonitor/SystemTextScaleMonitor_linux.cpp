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
#include <QLibrary>
#include <QString>

// STL headers
#include <cstring>
#include <memory>

namespace AccessibilitySample
{
  struct GClosure;
  struct GObject;
  struct GSettings;
  struct GSettingsSchema;
  struct GSettingsSchemaSource;
  struct GVariant;

  namespace
  {
    class LinuxGnomeSystemTextScaleMonitor final : public SystemTextScaleMonitor
    {
    public:
      LinuxGnomeSystemTextScaleMonitor();
      ~LinuxGnomeSystemTextScaleMonitor() override;

    private:
      Q_DISABLE_COPY_MOVE(LinuxGnomeSystemTextScaleMonitor)

      float platformTextScale() const override;

      // GSettings/GObject APIs are dynamically resolved to avoid a hard link dependency on GLib.
      // These aliases mirror the GLib C ABI types used in the linked declarations:
      // - gboolean is gint, and gint is int: https://docs.gtk.org/glib/types.html#gboolean
      // - signal handler IDs are gulong, and gulong is unsigned long: https://docs.gtk.org/gobject/func.signal_connect_data.html#gulong
      // - GConnectFlags is the enum accepted by g_signal_connect_data:
      //   https://docs.gtk.org/gobject/func.signal_connect_data.html
      // Function declarations:
      // - https://docs.gtk.org/gio/method.SettingsSchemaSource.lookup.html
      // - https://docs.gtk.org/gio/method.SettingsSchema.has_key.html
      // - https://docs.gtk.org/gio/ctor.Settings.new.html
      // - https://docs.gtk.org/gio/method.Settings.get_value.html
      // - https://docs.gtk.org/glib/method.Variant.get_double.html
      // Returned settings, schema, and variant values are caller-owned and must be released with the matching 'unref' function.
      using GBoolean = int;
      using GConnectFlags = int;
      using GSignalHandlerId = unsigned long; // NOLINT(google-runtime-int): mirrors GLib gulong ABI.
      using GSettingsSchemaSourceGetDefault = GSettingsSchemaSource* (*)();
      using GSettingsSchemaSourceLookup = GSettingsSchema* (*)(GSettingsSchemaSource*, const char*, GBoolean);
      using GSettingsSchemaHasKey = GBoolean (*)(GSettingsSchema*, const char*);
      using GSettingsSchemaUnref = void (*)(GSettingsSchema*);
      using GSettingsNew = GSettings* (*)(const char*);
      using GSettingsGetValue = GVariant* (*)(GSettings*, const char*);
      using GVariantGetDouble = double (*)(GVariant*);
      using GVariantGetTypeString = const char* (*)(GVariant*);
      using GVariantUnref = void (*)(GVariant*);
      using GCallback = void (*)();
      using GClosureNotify = void (*)(void*, GClosure*);
      using GSignalConnectData = GSignalHandlerId (*)(GObject*, const char*, GCallback, void*, GClosureNotify, GConnectFlags);
      using GSignalHandlerDisconnect = void (*)(GObject*, GSignalHandlerId);
      using GObjectUnref = void (*)(GObject*);

      // GSettings/GObject deleters for unique_ptr
      struct GSettingsSchemaDeleter
      {
        GSettingsSchemaUnref gSettingsSchemaUnref = nullptr;
        void operator()(GSettingsSchema* schema) const;
      };

      struct GVariantDeleter
      {
        GVariantUnref gVariantUnref = nullptr;
        void operator()(GVariant* value) const;
      };

      struct GSettingsDeleter
      {
        GObjectUnref gObjectUnref = nullptr;
        void operator()(GSettings* settings) const;
      };

      // RAII wrappers for GSettings/GObject types
      using GSettingsSchemaPtr = std::unique_ptr<GSettingsSchema, GSettingsSchemaDeleter>;
      using GVariantPtr = std::unique_ptr<GVariant, GVariantDeleter>;
      using GSettingsPtr = std::unique_ptr<GSettings, GSettingsDeleter>;

      static GObject* asGObject(GSettings* settings);
      static void changed(void* settings, const char* key, void* userData);

      bool loadFunctions_();
      void initializeSettings_();

      QLibrary m_gLibLibrary;
      QLibrary m_gioLibrary;
      QLibrary m_gObjectLibrary;
      GSettingsPtr m_settings{nullptr, GSettingsDeleter{nullptr}};
      GSignalHandlerId m_changedSignalId = 0;
      GSettingsSchemaSourceGetDefault m_gSettingsSchemaSourceGetDefault = nullptr;
      GSettingsSchemaSourceLookup m_gSettingsSchemaSourceLookup = nullptr;
      GSettingsSchemaHasKey m_gSettingsSchemaHasKey = nullptr;
      GSettingsSchemaUnref m_gSettingsSchemaUnref = nullptr;
      GSettingsNew m_gSettingsNew = nullptr;
      GSettingsGetValue m_gSettingsGetValue = nullptr;
      GVariantGetDouble m_gVariantGetDouble = nullptr;
      GVariantGetTypeString m_gVariantGetTypeString = nullptr;
      GVariantUnref m_gVariantUnref = nullptr;
      GSignalConnectData m_gSignalConnectData = nullptr;
      GSignalHandlerDisconnect m_gSignalHandlerDisconnect = nullptr;
      GObjectUnref m_gObjectUnref = nullptr;
    };
  } // namespace

  // GNOME defines this schema/key in gsettings-desktop-schemas:
  // https://gitlab.gnome.org/GNOME/gsettings-desktop-schemas/-/raw/master/schemas/org.gnome.desktop.interface.gschema.xml.in
  // The text-scaling-factor key is type "d", with range 0.5..3.0 and default 1.0.
  static constexpr auto* s_schemaId = "org.gnome.desktop.interface";
  static constexpr auto* s_key = "text-scaling-factor";
  static constexpr auto* s_doubleVariantType = "d";
  static constexpr auto s_gConnectDefault = 0;

  LinuxGnomeSystemTextScaleMonitor::LinuxGnomeSystemTextScaleMonitor()
  {
    if (loadFunctions_())
    {
      initializeSettings_();
    }
  }

  LinuxGnomeSystemTextScaleMonitor::~LinuxGnomeSystemTextScaleMonitor()
  {
    if (m_settings && m_changedSignalId != 0)
    {
      m_gSignalHandlerDisconnect(asGObject(m_settings.get()), m_changedSignalId);
    }
  }

  float LinuxGnomeSystemTextScaleMonitor::platformTextScale() const
  {
    if (!m_settings)
    {
      return defaultTextScale();
    }

    auto value = GVariantPtr{m_gSettingsGetValue(m_settings.get(), s_key), GVariantDeleter{m_gVariantUnref}};
    if (!value)
    {
      return defaultTextScale();
    }

    const auto* valueType = m_gVariantGetTypeString(value.get());
    const auto textScale =
      valueType && std::strcmp(valueType, s_doubleVariantType) == 0 ? static_cast<float>(m_gVariantGetDouble(value.get())) : defaultTextScale();
    return textScale;
  }

  GObject* LinuxGnomeSystemTextScaleMonitor::asGObject(GSettings* settings)
  {
    return reinterpret_cast<GObject*>(settings);
  }

  void LinuxGnomeSystemTextScaleMonitor::GSettingsSchemaDeleter::operator()(GSettingsSchema* schema) const
  {
    if (schema && gSettingsSchemaUnref)
    {
      gSettingsSchemaUnref(schema);
    }
  }

  void LinuxGnomeSystemTextScaleMonitor::GVariantDeleter::operator()(GVariant* value) const
  {
    if (value && gVariantUnref)
    {
      gVariantUnref(value);
    }
  }

  void LinuxGnomeSystemTextScaleMonitor::GSettingsDeleter::operator()(GSettings* settings) const
  {
    if (settings && gObjectUnref)
    {
      gObjectUnref(LinuxGnomeSystemTextScaleMonitor::asGObject(settings));
    }
  }

  void LinuxGnomeSystemTextScaleMonitor::changed(void* /*settings*/, const char* /*key*/, void* userData)
  {
    auto* textScale = static_cast<LinuxGnomeSystemTextScaleMonitor*>(userData);
    if (textScale)
    {
      textScale->refreshTextScaleAndNotifyIfChanged();
    }
  }

  bool LinuxGnomeSystemTextScaleMonitor::loadFunctions_()
  {
    m_gLibLibrary.setFileNameAndVersion(QStringLiteral("glib-2.0"), 0);
    m_gioLibrary.setFileNameAndVersion(QStringLiteral("gio-2.0"), 0);
    m_gObjectLibrary.setFileNameAndVersion(QStringLiteral("gobject-2.0"), 0);
    if (!m_gLibLibrary.load() || !m_gioLibrary.load() || !m_gObjectLibrary.load())
    {
      return false;
    }

    m_gSettingsSchemaSourceGetDefault =
      reinterpret_cast<GSettingsSchemaSourceGetDefault>(m_gioLibrary.resolve("g_settings_schema_source_get_default"));
    m_gSettingsSchemaSourceLookup = reinterpret_cast<GSettingsSchemaSourceLookup>(m_gioLibrary.resolve("g_settings_schema_source_lookup"));
    m_gSettingsSchemaHasKey = reinterpret_cast<GSettingsSchemaHasKey>(m_gioLibrary.resolve("g_settings_schema_has_key"));
    m_gSettingsSchemaUnref = reinterpret_cast<GSettingsSchemaUnref>(m_gioLibrary.resolve("g_settings_schema_unref"));
    m_gSettingsNew = reinterpret_cast<GSettingsNew>(m_gioLibrary.resolve("g_settings_new"));
    m_gSettingsGetValue = reinterpret_cast<GSettingsGetValue>(m_gioLibrary.resolve("g_settings_get_value"));
    m_gVariantGetDouble = reinterpret_cast<GVariantGetDouble>(m_gLibLibrary.resolve("g_variant_get_double"));
    m_gVariantGetTypeString = reinterpret_cast<GVariantGetTypeString>(m_gLibLibrary.resolve("g_variant_get_type_string"));
    m_gVariantUnref = reinterpret_cast<GVariantUnref>(m_gLibLibrary.resolve("g_variant_unref"));
    m_gSignalConnectData = reinterpret_cast<GSignalConnectData>(m_gObjectLibrary.resolve("g_signal_connect_data"));
    m_gSignalHandlerDisconnect = reinterpret_cast<GSignalHandlerDisconnect>(m_gObjectLibrary.resolve("g_signal_handler_disconnect"));
    m_gObjectUnref = reinterpret_cast<GObjectUnref>(m_gObjectLibrary.resolve("g_object_unref"));

    return m_gSettingsSchemaSourceGetDefault && m_gSettingsSchemaSourceLookup && m_gSettingsSchemaHasKey && m_gSettingsSchemaUnref &&
           m_gSettingsNew && m_gSettingsGetValue && m_gVariantGetDouble && m_gVariantGetTypeString && m_gVariantUnref && m_gSignalConnectData &&
           m_gSignalHandlerDisconnect && m_gObjectUnref;
  }

  void LinuxGnomeSystemTextScaleMonitor::initializeSettings_()
  {
    auto* schemaSource = m_gSettingsSchemaSourceGetDefault();
    if (!schemaSource)
    {
      return;
    }

    auto schema = GSettingsSchemaPtr{m_gSettingsSchemaSourceLookup(schemaSource, s_schemaId, true), GSettingsSchemaDeleter{m_gSettingsSchemaUnref}};
    if (!schema)
    {
      return;
    }

    const auto keyExists = m_gSettingsSchemaHasKey(schema.get(), s_key) != 0;
    if (!keyExists)
    {
      return;
    }

    m_settings = GSettingsPtr{m_gSettingsNew(s_schemaId), GSettingsDeleter{m_gObjectUnref}};
    if (m_settings)
    {
      m_changedSignalId = m_gSignalConnectData(asGObject(m_settings.get()), "changed::text-scaling-factor", reinterpret_cast<GCallback>(changed),
                                               this, nullptr, s_gConnectDefault);
    }
  }

  std::shared_ptr<SystemTextScaleMonitor> SystemTextScaleMonitor::createPlatformMonitor()
  {
    return std::make_shared<LinuxGnomeSystemTextScaleMonitor>();
  }
} // namespace AccessibilitySample
