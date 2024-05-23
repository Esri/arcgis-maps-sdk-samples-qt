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

#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>
#include <QVariant>

class DataItem : public QObject
{
  Q_OBJECT

public:
  explicit DataItem(const QString& itemId, const QString& path,
                    QObject* parent = nullptr);
  ~DataItem() override = default;

  Q_PROPERTY(QString itemId READ itemId CONSTANT)
  Q_PROPERTY(QString path READ path CONSTANT)
  Q_PROPERTY(bool exists READ exists WRITE setExists NOTIFY existsChanged)

  QString itemId() const { return m_itemId; }
  QString path() const { return m_path; }
  bool exists() const { return m_exists; }
  void setExists(bool exists);

signals:
  void existsChanged();

private:
  QString m_itemId;
  QString m_path;
  bool m_exists = false;
};

#endif // DATAITEM_H
