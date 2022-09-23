// [WriteFile Name=FeatureListModel, Category=Features]
// [Legal]
// Copyright 2016 Esri.

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

#include "FeatureListModel.h"

#include "Feature.h"
#include "AttributeListModel.h"

using namespace Esri::ArcGISRuntime;

FeatureListModel::FeatureListModel(QObject* parent /* = nullptr */) :
  QAbstractListModel(parent)
{
}

void FeatureListModel::addFeature(Feature* feature)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_features << feature;
  endInsertRows();
}

int FeatureListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_features.count();
}

QVariant FeatureListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_features.count())
    return QVariant();

  const Feature* feature = m_features[index.row()];

  if (role == FeatureOidRole)
    return feature->attributes()->attributeValue("oid");
  else if (role == FeatureTimestampRole)
    return feature->attributes()->attributeValue("collection_timestamp");

  return QVariant();
}

QHash<int, QByteArray> FeatureListModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[FeatureOidRole] = "featureOidRole";
  roles[FeatureTimestampRole] = "featureTimestampRole";
  return roles;
}

void FeatureListModel::clear()
{
  beginResetModel();
  m_features.clear();
  endResetModel();
}
