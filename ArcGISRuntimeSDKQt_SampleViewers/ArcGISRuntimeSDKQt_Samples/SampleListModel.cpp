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
#include <QVariant>
#include <QMetaEnum>

#include "SampleListModel.h"
#include "Sample.h"

SampleListModel::SampleListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void SampleListModel::setupRoles()
{
  // Converts the SampleRoles enum into QML-friendly role names. This removes
  // the last 4 characters of the Enumeration name (assumed to be "Role"), and
  // sets the first character to lowercase for the correct camel-case
  // representation.
  // E.G.
  // m_roles[NameRole] = "name";
  // m_roles[PathRole] = "path";
  // m_roles[SourceRole] = "source";
  // m_roles[DescriptionRole] = "description";
  // m_roles[ThumbnailUrlRole] = "thumbnailUrl";
  // m_roles[SampleRole] = "sample";
  const auto& sampleEnumsMeta = QMetaEnum::fromType<SampleRoles>();
  const int totalEnums = sampleEnumsMeta.keyCount();
  for (int i = 0; i < totalEnums; ++i)
  {
    auto roleName = QString(sampleEnumsMeta.key(i));
    roleName.chop(4);
    roleName[0] = roleName[0].toLower();
    m_roles[sampleEnumsMeta.value(i)] = roleName.toLatin1();
  }
}

void SampleListModel::addSample(Sample* sample)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_samples << sample;
  endInsertRows();
}
int SampleListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_samples.count();
}

QVariant SampleListModel::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= m_samples.count())
    return QVariant();

  Sample* sample = m_samples[index.row()];
  QVariant retVal;

  switch (role)
  {
  case NameRole:
  case Qt::DisplayRole:
    retVal = sample->name();
    break;
  case PathRole:
    retVal = sample->path();
    break;
  case SourceRole:
    retVal = sample->source();
    break;
  case DescriptionRole:
    retVal = sample->description();
    break;
  case ThumbnailUrlRole:
    retVal = sample->thumbnailUrl();
    break;
  case SampleRole:
    retVal = QVariant::fromValue(sample);
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> SampleListModel::roleNames() const
{
  return m_roles;
}
