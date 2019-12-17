// [WriteFile Name=ListRelatedFeatures, Category=Features]
// [Legal]
// Copyright 2017 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include "RelatedFeatureListModel.h"
#include "RelatedFeature.h"

RelatedFeatureListModel::RelatedFeatureListModel(QObject* parent) :
  QAbstractListModel(parent)
{
}

void RelatedFeatureListModel::addRelatedFeature(RelatedFeature relatedFeature)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_relatedFeatures << relatedFeature;
  endInsertRows();
}
int RelatedFeatureListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_relatedFeatures.count();
}

QVariant RelatedFeatureListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_relatedFeatures.count())
    return QVariant();

  RelatedFeature relatedFeature = m_relatedFeatures[index.row()];

  if (role == DisplayFieldNameRole)
    return relatedFeature.displayFieldName();
  else if (role == DisplayFieldValueRole)
    return relatedFeature.displayFieldValue();
  else if (role == ServiceLayerName)
    return relatedFeature.serviceLayerName();
  return QVariant();
}

QHash<int, QByteArray> RelatedFeatureListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[DisplayFieldNameRole] = "displayFieldName";
  roles[DisplayFieldValueRole] = "displayFieldValue";
  roles[ServiceLayerName] = "serviceLayerName";
  return roles;
}

void RelatedFeatureListModel::clear()
{
  beginResetModel();
  m_relatedFeatures.clear();
  endResetModel();
}
