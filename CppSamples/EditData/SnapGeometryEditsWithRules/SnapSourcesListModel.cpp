// [Legal]
// Copyright 2024 Esri.
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

// sample headers
#include "SnapSourcesListModel.h"

// ArcGIS Maps SDK headers
#include "FeatureLayer.h"
#include "GraphicsOverlay.h"
#include "SnapSourceSettings.h"
#include "SubtypeSublayer.h"

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

// STL headers
#include <utility>

using namespace Esri::ArcGISRuntime;

SnapSourcesListModel::SnapSourcesListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void SnapSourcesListModel::setupRoles()
{
  m_roles[NameRole] = "name";
  m_roles[IsEnabledRole] = "isEnabled";
}

int SnapSourcesListModel::size() const
{
  return m_snapSourceSettings.size();
}

void SnapSourcesListModel::clear()
{
  beginResetModel();
  m_snapSourceSettings.clear();
  endResetModel();
}

int SnapSourcesListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_snapSourceSettings.count();
}

QVariant SnapSourcesListModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_snapSourceSettings.count())
  {
    return {};
  }

  auto* snapSourceSetting = m_snapSourceSettings.at(index.row());
  if (role == NameRole)
  {
    return determineName(snapSourceSetting);
  }
  if (role == IsEnabledRole)
  {
    return snapSourceSetting->isEnabled();
  }
  return {};
}

bool SnapSourcesListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_snapSourceSettings.count())
  {
    return false;
  }

  if (role == IsEnabledRole)
  {
    auto* snapSourceSettingsWrapper = m_snapSourceSettings.at(index.row());
    if (snapSourceSettingsWrapper)
    {
      snapSourceSettingsWrapper->setEnabled(value.toBool());
      emit dataChanged(index, index, { role });
      return true;
    }
  }
  return false;
}

QHash<int, QByteArray> SnapSourcesListModel::roleNames() const
{
  return m_roles;
}

void SnapSourcesListModel::setSnapSourceSettings(QList<SnapSourceSettings*> snapSourceSettings)
{
  beginResetModel();
  m_snapSourceSettings.clear();
  m_snapSourceSettings = std::move(snapSourceSettings);
  endResetModel();
}

QList<SnapSourceSettings*> SnapSourcesListModel::snapSourceSettings() const
{
  return m_snapSourceSettings;
}

QString SnapSourcesListModel::determineName(Esri::ArcGISRuntime::SnapSourceSettings* snapSourceSettings) const
{
  if (auto graphicsOverlay = dynamic_cast<Esri::ArcGISRuntime::GraphicsOverlay*>(snapSourceSettings->source()); graphicsOverlay)
  {
    QString overlayId = graphicsOverlay->overlayId();
    return overlayId.isEmpty() ? "Default Graphics Overlay" : overlayId;
  }
  if (auto subtypeSublayer = dynamic_cast<Esri::ArcGISRuntime::SubtypeSublayer*>(snapSourceSettings->source()); subtypeSublayer)
  {
    return subtypeSublayer->name();
  }
  return {};
}
