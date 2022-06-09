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

#ifndef SAMPLE_H
#define SAMPLE_H

class SourceCodeListModel;
class DataItemListModel;

#include <QObject>
#include <QVariant>

class Sample : public QObject
{
  Q_OBJECT

public:
  explicit Sample(const QVariant& name, const QVariant& path,
                  const QVariant& source, const QVariant& description,
                  SourceCodeListModel* codeFiles, const QVariant& thumbnailUrl,
                  DataItemListModel* dataItems, QObject* parent = nullptr);
  ~Sample() override = default;

  Q_PROPERTY(QVariant source READ source CONSTANT)
  Q_PROPERTY(SourceCodeListModel* codeFiles READ codeFiles CONSTANT)
  Q_PROPERTY(QVariant description READ description CONSTANT)
  Q_PROPERTY(QVariant thumbnailUrl READ thumbnailUrl CONSTANT)
  Q_PROPERTY(DataItemListModel* dataItems READ dataItems CONSTANT)

  QVariant name() const { return m_name; }
  QVariant path() const { return m_path; }
  QVariant source() const { return m_source; }
  QVariant description() const { return m_description; }
  SourceCodeListModel* codeFiles() const { return m_codeFiles; }
  QVariant thumbnailUrl() const { return m_thumbnailUrl; }
  DataItemListModel* dataItems() const { return m_dataItems; }

private:
  QVariant m_name;
  QVariant m_path;
  QVariant m_source;
  QVariant m_description;
  SourceCodeListModel* m_codeFiles = nullptr;
  QVariant m_thumbnailUrl;
  DataItemListModel* m_dataItems = nullptr;
};

#endif // SAMPLE_H
