// [Legal]
// Copyright 2022 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

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

#include "CategoryListModel.h"
#include "SampleCategory.h"
#include "SampleListModel.h"

CategoryListModel::CategoryListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void CategoryListModel::setupRoles()
{
  m_roles[NameRole] = "name";
  m_roles[DisplayNameRole] = "displayName";
  m_roles[PathRole] = "path";
  m_roles[ThumbnailRole] = "thumbnailUrl";
  m_roles[BackgroundThumbnailRole] = "backgroundThumbnailUrl";
}

void CategoryListModel::addCategory(SampleCategory* category)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_categories << category;
  endInsertRows();
}
int CategoryListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_categories.count();
}

QVariant CategoryListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_categories.count())
    return QVariant();

  SampleCategory* sampleCategory = m_categories[index.row()];
  QVariant retVal;

  switch (role)
  {
  case NameRole:
    retVal = sampleCategory->name();
    break;
  case DisplayNameRole:
    retVal = sampleCategory->displayName();
    break;
  case PathRole:
    retVal = sampleCategory->path();
    break;
  case ThumbnailRole:
    retVal = sampleCategory->thumbnailUrl();
    break;
  case BackgroundThumbnailRole:
    retVal = sampleCategory->backgroundThumbnailUrl();
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> CategoryListModel::roleNames() const
{
  return m_roles;
}
