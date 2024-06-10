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

#include <QObject>
#include <QVariant>
#include <QStandardPaths>

#include "DataItem.h"

static QString homePath()
{
  QString homePath;

#ifdef Q_OS_IOS
  homePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

  return homePath;
}

DataItem::DataItem(const QString& itemId, const QString& path,
                   QObject* parent /*= nullptr*/) :
  QObject(parent),
  m_itemId(itemId),
  m_path(path)
{
}

bool DataItem::exists() const
{
  return QFile::exists(homePath() + m_path.mid(1));
}

void DataItem::setExists(bool exists)
{
  m_exists = exists;
  emit existsChanged();
}
