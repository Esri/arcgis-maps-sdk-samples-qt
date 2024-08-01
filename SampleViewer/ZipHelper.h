// [Legal]
// Copyright 2024 Esri.

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
/// \file ZipHelper.h

#ifndef ZipHelper_H
#define ZipHelper_H

// Qt headers
#include <QDateTime>
#include <QDir>
#include <QObject>
#include <QString>
#include <QStringList>

// minizip-ng headers
#include <zip.h>
#include <unzip.h>

//--------------------------------------------------------------------

class ZipHelper : public QObject
{
  Q_OBJECT

public:
  enum class Result
  {
    EndOfListOfFile     = UNZ_END_OF_LIST_OF_FILE,
    ErrNo               = UNZ_ERRNO,
    EndOfFile           = UNZ_EOF,
    ParameterError      = UNZ_PARAMERROR,
    BadZipFile          = UNZ_BADZIPFILE,
    InternalError       = UNZ_INTERNALERROR,
    CrcError            = UNZ_CRCERROR
  };
  Q_ENUM(Result)

public:
  Q_INVOKABLE bool extractAll(const QString &outputPath);
  void setPath(const QString& path);

  explicit ZipHelper(QObject* parent = nullptr);
  ZipHelper(const QString& path, QObject* parent = nullptr);
  ~ZipHelper();

signals:
  void pathChanged();

  void extractError(const QString& fileName, const QString& outputFileName, ZipHelper::Result result);
  void extractProgress(const QString& fileName, const QString& outputFileName, qreal percent);
  void extractCompleted();

private:
  bool zrOpen(const QString& filePath);
  void zrClose();

  unzFile zrHandle() const { return m_unzFile; }
  bool zrIsOpen() const { return m_unzFile != nullptr; }

  bool extractAll(QDir& outputDir);
  bool extractCurrentFile(const QString& outputFilePath);

  QString currentFileName();
  QDateTime currentFileDateTime();
  unz_file_info64 currentFileInfo();

  bool gotoFirstFile();
  bool gotoNextFile();

  //--------------------------------------------------------------------

public:
  enum class PathVariableType
  {
    NotFound            = 0,
    ApplicationProperty = 1,
    Settings            = 2,
    Environment         = 3
  };
private:
  static bool setDateTime(const QString& filePath, const QDateTime& accessed, const QDateTime& modified);
  static QString resolvedPath(const QString& path, bool useFallbacks = true);
  static QString pathVariable(const QString& name, const QString& defaultValue = QString(), bool useFallbacks = true);
  static void setPathVariable(const QString& name, const QString& value);

  static PathVariableType pathVariableType(const QString& name, bool useFallbacks = true);
  static bool pathVariableExists(const QString& name, bool useFallbacks = true);

  static QString defaultPathVariable(const QString& name);
  static QString defaultHomePath();
  static QString defaultTempPath();
  static QByteArray pathPropertyName(const QString& name);

  void zwClose(const QString& globalComment = QString());
  bool exists() const;
  bool zrOpen();
  const QString& path() const { return m_Path; }

private:
  QString                     m_Path;
  unzFile                     m_unzFile;
  zipFile                     m_zipFile;
};

//--------------------------------------------------------------------

#endif // ZipHelper_H
