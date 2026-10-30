// [WriteFile Name=AddFeaturesWithSharedTemplates, Category=EditData]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

// sample headers
#include "AddFeaturesWithSharedTemplates.h"

// ArcGIS Maps SDK headers
#include "Error.h"
#include "ErrorException.h"
#include "FeatureEditResult.h"
#include "FeatureLayer.h"
#include "FeatureTableEditResult.h"
#include "GeodatabaseTypes.h"
#include "Geometry.h"
#include "GeometryBuilder.h"
#include "GeometryConstructionTool.h"
#include "GeometryEditor.h"
#include "GeometryTypes.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "PortalItem.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "SharedTemplate.h"
#include "SharedTemplateFeatureCreationSet.h"
#include "VertexTool.h"

// Qt headers
#include <QBuffer>
#include <QDebug>
#include <QFuture>
#include <QImage>
#include <QMap>
#include <QScopeGuard>
#include <QSet>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>

// STL headers
#include <memory>

using namespace Esri::ArcGISRuntime;

namespace
{
  constexpr auto createFeatureError = QT_TRANSLATE_NOOP("AddFeaturesWithSharedTemplates", "Unable to create or add features.");
} // namespace

AddFeaturesWithSharedTemplates::AddFeaturesWithSharedTemplates(QObject* parent) :
  QObject(parent),
  m_map(new Map(new PortalItem(QStringLiteral("b635be46dfb545b888077389ac7f0962"), this), this)),
  m_geometryEditor(new GeometryEditor(this)),
  m_statusResetTimer(new QTimer(this)),
  m_statusText(tr("Loading shared templates..."))
{
  m_geometryEditor->setTool(new VertexTool(this));
  connect(m_geometryEditor, &GeometryEditor::geometryChanged, this, &AddFeaturesWithSharedTemplates::updateCanComplete_);
  connect(m_geometryEditor, &GeometryEditor::isStartedChanged, this, &AddFeaturesWithSharedTemplates::updateCanComplete_);

  m_statusResetTimer->setSingleShot(true);
  m_statusResetTimer->setInterval(2000);
  connect(m_statusResetTimer, &QTimer::timeout, this, [this]
  {
    if (!m_busy && !drawing() && !m_hasPendingEdits)
    {
      setStatusText_(tr("Open Shared Templates and select a template to create features."));
    }
  });

  connect(m_map, &Map::doneLoading, this, [this](const Error& error)
  {
    if (!error.isEmpty())
    {
      reportError_(tr("Unable to load templates."), error.message());
      return;
    }

    for (auto* layer : *m_map->operationalLayers())
    {
      auto* featureLayer = qobject_cast<FeatureLayer*>(layer);
      auto* table = featureLayer ? qobject_cast<ServiceFeatureTable*>(featureLayer->featureTable()) : nullptr;
      if (table && table->serviceGeodatabase())
      {
        m_serviceGeodatabase = table->serviceGeodatabase();
        loadTemplates_();
        return;
      }
    }
    reportError_(tr("Unable to load templates."), tr("The map does not contain a shared template source."));
  });
}

AddFeaturesWithSharedTemplates::~AddFeaturesWithSharedTemplates() = default;

void AddFeaturesWithSharedTemplates::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AddFeaturesWithSharedTemplates>("Esri.Samples", 1, 0, "AddFeaturesWithSharedTemplatesSample");
}

MapQuickView* AddFeaturesWithSharedTemplates::mapView() const
{
  return m_mapView;
}

void AddFeaturesWithSharedTemplates::setMapView(MapQuickView* mapView)
{
  if (!mapView || m_mapView == mapView)
  {
    return;
  }
  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setGeometryEditor(m_geometryEditor);
  emit mapViewChanged();
  m_map->load();
}

QVariantList AddFeaturesWithSharedTemplates::templates() const
{
  return m_templates;
}

QString AddFeaturesWithSharedTemplates::statusText() const
{
  return m_statusText;
}

bool AddFeaturesWithSharedTemplates::busy() const
{
  return m_busy;
}

bool AddFeaturesWithSharedTemplates::ready() const
{
  return m_ready;
}

bool AddFeaturesWithSharedTemplates::drawing() const
{
  return m_activeTemplateIndex >= 0;
}

bool AddFeaturesWithSharedTemplates::hasPendingEdits() const
{
  return m_hasPendingEdits;
}

bool AddFeaturesWithSharedTemplates::canComplete() const
{
  return m_canComplete;
}

void AddFeaturesWithSharedTemplates::loadTemplates_()
{
  m_serviceGeodatabase->querySharedTemplatesAsync(nullptr, this)
    .then(this,
          [this](const QMap<qint64, QList<SharedTemplate*>>& templatesByLayer)
  {
    QSet<SharedTemplateType> includedTypes;
    for (auto it = templatesByLayer.cbegin(); it != templatesByLayer.cend(); ++it)
    {
      for (auto* sharedTemplate : it.value())
      {
        const auto type = sharedTemplate->type();
        if ((type != SharedTemplateType::Preset && type != SharedTemplateType::Group) || includedTypes.contains(type))
        {
          continue;
        }
        includedTypes.insert(type);
        m_templateItems.append({sharedTemplate, it.key()});
        m_templates.append(QVariantMap{{QStringLiteral("name"), sharedTemplate->name()},
                                       {QStringLiteral("kind"), type == SharedTemplateType::Preset ? tr("Preset") : tr("Group")},
                                       {QStringLiteral("description"), sharedTemplate->description()},
                                       {QStringLiteral("swatch"), QString()},});
      }
    }
    emit templatesChanged();
    if (!m_ready)
    {
      m_ready = true;
      emit readyChanged();
    }
    finishOperation_(m_templates.isEmpty() ? tr("This map has no preset or group shared templates.") :
                                             tr("Open Shared Templates and select a template to create features."));
    for (qsizetype index = 0; index < m_templateItems.size(); ++index)
    {
      createSwatch_(index);
    }
  })
    .onFailed(this, [this](const ErrorException& error)
  {
    reportError_(tr("Unable to load templates."), error.error().message());
  });
}

void AddFeaturesWithSharedTemplates::createSwatch_(qsizetype index)
{
  const auto& item = m_templateItems.at(index);
  item.sharedTemplate->createSwatchAsync(item.layerId)
    .then(this,
          [this, index](const QImage& image)
  {
    if (image.isNull())
    {
      qWarning() << "Shared template has no swatch:" << m_templateItems.at(index).sharedTemplate->name();
      return;
    }
    QBuffer buffer;
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG"))
    {
      qWarning() << "Unable to encode shared template swatch.";
      return;
    }
    auto entry = m_templates.at(index).toMap();
    entry.insert(QStringLiteral("swatch"), QStringLiteral("data:image/png;base64,") + QString::fromLatin1(buffer.data().toBase64()));
    m_templates[index] = entry;
    emit templatesChanged();
  })
    .onFailed(this, [this, index](const ErrorException& error)
  {
    // A swatch failure leaves the visible placeholder and must not prevent template selection.
    qWarning() << "Unable to create shared template swatch:" << m_templateItems.at(index).sharedTemplate->name()
               << "layer ID:" << m_templateItems.at(index).layerId << error.error();
  });
}

void AddFeaturesWithSharedTemplates::startDrawing(int index)
{
  if (m_busy || drawing() || m_hasPendingEdits)
  {
    return;
  }
  if (index < 0 || index >= m_templateItems.size())
  {
    reportError_(tr("Unable to start drawing."), tr("Select an available shared template."));
    return;
  }
  const auto& item = m_templateItems.at(index);
  auto* tool = item.sharedTemplate->defaultConstructionTool(item.layerId);
  if (!tool || (tool->toolType() != GeometryConstructionToolType::Point && tool->toolType() != GeometryConstructionToolType::Line))
  {
    reportError_(tr("Unable to start drawing."), tr("The template's default construction tool is not supported by this sample."));
    return;
  }
  const bool isPoint = tool->toolType() == GeometryConstructionToolType::Point;
  m_activeTemplateIndex = index;
  setStatusText_(isPoint ? tr("Place a point, then tap Complete or Cancel.") : tr("Sketch a line, then tap Complete or Cancel."));
  m_geometryEditor->start(isPoint ? GeometryType::Point : GeometryType::Polyline);
  emit drawingChanged();
}

void AddFeaturesWithSharedTemplates::cancelDrawing()
{
  if (m_busy || !drawing())
  {
    return;
  }
  m_geometryEditor->stop();
  m_activeTemplateIndex = -1;
  emit drawingChanged();
  setStatusText_(tr("Draw canceled."));
  m_statusResetTimer->start();
}

void AddFeaturesWithSharedTemplates::completeDrawing()
{
  if (m_busy || !drawing())
  {
    return;
  }
  if (!canComplete())
  {
    setStatusText_(tr("Draw a valid geometry, then tap Complete or Cancel."));
    return;
  }
  auto* sharedTemplate = m_templateItems.at(m_activeTemplateIndex).sharedTemplate;
  setBusy_(true);
  const Geometry geometry = m_geometryEditor->stop();
  m_activeTemplateIndex = -1;
  emit drawingChanged();
  if (geometry.isEmpty())
  {
    finishOperation_(tr("No geometry was drawn."));
    return;
  }
  setStatusText_(tr("Creating features..."));

  m_serviceGeodatabase->createFeaturesAsync(sharedTemplate, geometry, this)
    .then(this,
          [this](SharedTemplateFeatureCreationSet* creationSet)
  {
    if (!creationSet)
    {
      reportError_(tr(createFeatureError), tr("No feature creation set was returned."));
      return;
    }
    m_serviceGeodatabase->addFeaturesAsync(creationSet)
      .then(this,
            [this, creationSet]
    {
      creationSet->deleteLater();
      finishOperation_(tr("Features added."));
    })
      .onFailed(this, [this, creationSet](const ErrorException& error)
    {
      creationSet->deleteLater();
      reportError_(tr(createFeatureError), error.error().message());
    });
  })
    .onFailed(this, [this](const ErrorException& error)
  {
    reportError_(tr(createFeatureError), error.error().message());
  });
}

void AddFeaturesWithSharedTemplates::saveEdits()
{
  if (m_busy || !m_hasPendingEdits)
  {
    return;
  }
  setBusy_(true);
  setStatusText_(tr("Saving edits..."));
  m_serviceGeodatabase->applyEditsAsync()
    .then(this,
          [this](const QList<FeatureTableEditResult*>& results)
  {
    const QScopeGuard cleanup{[&results]
    {
      qDeleteAll(results);
    }};
    QStringList errors;
    for (const auto* tableResult : results)
    {
      for (const auto* featureResult : tableResult->editResults())
      {
        if (featureResult->isCompletedWithErrors())
        {
          errors.append(featureResult->error().message());
        }
      }
    }
    if (!errors.isEmpty())
    {
      reportError_(tr("Unable to save edits."), errors.join(QLatin1Char('\n')));
      return;
    }
    finishOperation_(tr("Edits saved."));
  })
    .onFailed(this, [this](const ErrorException& error)
  {
    reportError_(tr("Unable to save edits."), error.error().message());
  });
}

void AddFeaturesWithSharedTemplates::undoEdits()
{
  if (m_busy || !m_hasPendingEdits)
  {
    return;
  }
  setBusy_(true);
  setStatusText_(tr("Undoing local edits..."));
  m_serviceGeodatabase->undoLocalEditsAsync()
    .then(this,
          [this]
  {
    finishOperation_(tr("Edits undone."));
  })
    .onFailed(this, [this](const ErrorException& error)
  {
    reportError_(tr("Unable to undo edits."), error.error().message());
  });
}

void AddFeaturesWithSharedTemplates::updateCanComplete_()
{
  bool canComplete = false;
  if (m_geometryEditor->isStarted())
  {
    const std::unique_ptr<GeometryBuilder> builder(GeometryBuilder::create(m_geometryEditor->geometry()));
    canComplete = builder && builder->isSketchValid();
  }
  if (m_canComplete != canComplete)
  {
    m_canComplete = canComplete;
    emit canCompleteChanged();
  }
}

void AddFeaturesWithSharedTemplates::setStatusText_(const QString& status)
{
  m_statusResetTimer->stop();
  if (m_statusText != status)
  {
    m_statusText = status;
    emit statusTextChanged();
  }
}

void AddFeaturesWithSharedTemplates::setBusy_(bool busy)
{
  if (m_busy != busy)
  {
    m_busy = busy;
    emit busyChanged();
  }
}

void AddFeaturesWithSharedTemplates::setHasPendingEdits_(bool hasPendingEdits)
{
  if (m_hasPendingEdits != hasPendingEdits)
  {
    m_hasPendingEdits = hasPendingEdits;
    emit hasPendingEditsChanged();
  }
}

void AddFeaturesWithSharedTemplates::finishOperation_(const QString& status)
{
  setHasPendingEdits_(m_serviceGeodatabase && m_serviceGeodatabase->hasLocalEdits());
  setStatusText_(status);
  setBusy_(false);
}

void AddFeaturesWithSharedTemplates::reportError_(const QString& status, const QString& message)
{
  finishOperation_(status);
  qWarning() << status << message;
  emit errorOccurred(message);
}
