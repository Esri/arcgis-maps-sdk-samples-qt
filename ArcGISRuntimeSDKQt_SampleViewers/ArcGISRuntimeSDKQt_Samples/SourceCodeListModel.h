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

#ifndef SOURCECODELISTMODEL_H
#define SOURCECODELISTMODEL_H

class SourceCode;

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>

class SourceCodeListModel : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(int size READ size CONSTANT)

public:
  enum CategoryRoles
  {
    NameRole = Qt::UserRole + 1,
    CodeRole,
    PathRole,
    SizeRole
  };

  explicit SourceCodeListModel(QObject* parent = nullptr);
  ~SourceCodeListModel() override = default;

  Q_INVOKABLE SourceCode* get(int index) const { return m_codeFiles.at(index); }
  void addCodeFile(SourceCode* codeFile);
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  int size() const { return m_codeFiles.size(); }
  void setupRoles();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QHash<int, QByteArray> m_roles;
  QList<SourceCode*> m_codeFiles;
};

#endif // SOURCECODELISTMODEL_H
