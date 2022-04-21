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

#ifndef SAMPLELISTMODEL_H
#define SAMPLELISTMODEL_H

class Sample;

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class SampleListModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(int size READ size NOTIFY sizeChanged)

public:
  enum SampleRoles
  {
    NameRole = Qt::UserRole + 1, // QString
    PathRole, // QString
    SourceRole,  // QString
    DescriptionRole, // QString
    ThumbnailUrlRole, // QString (despite name)
    SampleRole, // Sample*
  };
  Q_ENUM(SampleRoles)

  explicit SampleListModel(QObject* parent = nullptr);
  ~SampleListModel() override = default;

  Q_INVOKABLE Sample* get(int index) const { return m_samples.at(index); }
  void addSample(Sample* sample);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  int size() const { return m_samples.size(); }

signals:
  void sizeChanged();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  void setupRoles();

private:
  QHash<int, QByteArray> m_roles;
  QList<Sample*> m_samples;
};

#endif // SAMPLELISTMODEL_H
