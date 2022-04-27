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

#ifndef SOURCECODE_H
#define SOURCECODE_H

#include <QObject>
#include <QVariant>

class SourceCode : public QObject
{
  Q_OBJECT

public:
  explicit SourceCode(const QString& name, const QString& code,
                      const QString& path, QObject* parent = nullptr);
  ~SourceCode() override = default;

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString code READ code CONSTANT)
  Q_PROPERTY(QString path READ path CONSTANT)

  QString name() const { return m_name; }
  QString code() const { return m_code; }
  QString path() const { return m_path; }

private:
  QString m_name;
  QString m_code;
  QString m_path;
};

#endif // SOURCECODE_H
