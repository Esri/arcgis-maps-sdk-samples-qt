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

#include "SourceCodeListModel.h"
#include "SourceCode.h"

SourceCodeListModel::SourceCodeListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void SourceCodeListModel::setupRoles()
{
  m_roles[NameRole] = "name";
  m_roles[CodeRole] = "code";
  m_roles[PathRole] = "path";
  m_roles[SizeRole] = "size";
}

void SourceCodeListModel::addCodeFile(SourceCode* codeFile)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_codeFiles << codeFile;
  endInsertRows();
}
int SourceCodeListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent)
  return m_codeFiles.count();
}

QVariant SourceCodeListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_codeFiles.count())
    return QVariant();

  SourceCode* sourceCode = m_codeFiles[index.row()];
  QVariant retVal;

  switch (role)
  {
  case NameRole:
    retVal = sourceCode->name();
    break;
  case CodeRole:
    retVal = sourceCode->code();
    break;
  case PathRole:
    retVal = sourceCode->path();
    break;
  case SizeRole:
    retVal = m_codeFiles.length();
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> SourceCodeListModel::roleNames() const
{
  return m_roles;
}
