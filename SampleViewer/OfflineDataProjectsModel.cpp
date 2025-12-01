// [Legal]
// Copyright 2025 Esri.
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

#include "pch.hpp"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include "OfflineDataProjectsModel.h"
#include "Sample.h"

OfflineDataProjectsModel::OfflineDataProjectsModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void OfflineDataProjectsModel::setupRoles()
{
  m_roles[SampleRole] = "sample";
  m_roles[DownloadedRole] = "downloaded";
  m_roles[DownloadingRole] = "downloading";
  m_roles[DownloadProgressRole] = "downloadProgress";
  m_roles[DownloadedItemsCountRole] = "downloadedItemsCount";
  m_roles[TotalItemsCountRole] = "totalItemsCount";
}

void OfflineDataProjectsModel::addProject(Sample* sample, bool downloaded, bool downloading, double progress, int downloadedCount, int totalCount)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  OfflineDataProject project;
  project.sample = sample;
  project.downloaded = downloaded;
  project.downloading = downloading;
  project.downloadProgress = progress;
  project.downloadedItemsCount = downloadedCount;
  project.totalItemsCount = totalCount;
  m_projects << project;
  endInsertRows();
}

void OfflineDataProjectsModel::updateProject(int index, bool downloaded, bool downloading, double progress, int downloadedCount, int totalCount)
{
  if (index < 0 || index >= m_projects.count())
    return;

  m_projects[index].downloaded = downloaded;
  m_projects[index].downloading = downloading;
  m_projects[index].downloadProgress = progress;
  m_projects[index].downloadedItemsCount = downloadedCount;
  m_projects[index].totalItemsCount = totalCount;

  QModelIndex modelIndex = this->index(index);
  emit dataChanged(modelIndex, modelIndex, {DownloadedRole, DownloadingRole, DownloadProgressRole, DownloadedItemsCountRole, TotalItemsCountRole});
}

int OfflineDataProjectsModel::findProjectBySample(Sample* sample) const
{
  for (int i = 0; i < m_projects.count(); ++i)
  {
    if (m_projects[i].sample == sample)
      return i;
  }
  return -1;
}

void OfflineDataProjectsModel::clear()
{
  beginResetModel();
  m_projects.clear();
  endResetModel();
}

int OfflineDataProjectsModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return static_cast<int>(m_projects.count());
}

QVariant OfflineDataProjectsModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || (index.row() >= m_projects.count()))
  {
    return QVariant{};
  }

  const OfflineDataProject& project = m_projects[index.row()];
  QVariant retVal;

  switch (role)
  {
  case SampleRole:
    retVal = QVariant::fromValue(project.sample);
    break;
  case DownloadedRole:
    retVal = project.downloaded;
    break;
  case DownloadingRole:
    retVal = project.downloading;
    break;
  case DownloadProgressRole:
    retVal = project.downloadProgress;
    break;
  case DownloadedItemsCountRole:
    retVal = project.downloadedItemsCount;
    break;
  case TotalItemsCountRole:
    retVal = project.totalItemsCount;
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> OfflineDataProjectsModel::roleNames() const
{
  return m_roles;
}
