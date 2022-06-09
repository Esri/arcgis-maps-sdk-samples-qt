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

#ifndef SAMPLECATEGORY_H
#define SAMPLECATEGORY_H

class SampleListModel;

#include <QObject>
#include <QString>

class SampleCategory : public QObject
{
  Q_OBJECT

public:
  explicit SampleCategory(const QString& name, const QString& displayName,
                          const QString& path, const QString& thumbnailUrl,
                          const QString& backgroundThumbnailUrl,
                          SampleListModel* samples, QObject* parent = nullptr);
  ~SampleCategory() override = default;

  Q_PROPERTY(SampleListModel* samples READ samples NOTIFY samplesChanged)

  QString name() const { return m_name; }
  QString displayName() const { return m_displayName; }
  QString path() const { return m_path; }
  QString thumbnailUrl() const { return m_thumbnail; }
  QString backgroundThumbnailUrl() const { return m_backgroundThumbnail; }
  SampleListModel* samples() const { return m_samples; }

signals:
  void samplesChanged();

private:
  QString m_name;
  QString m_displayName;
  QString m_path;
  QString m_thumbnail;
  QString m_backgroundThumbnail;
  SampleListModel* m_samples = nullptr;
};

#endif // SAMPLECATEGORY_H
