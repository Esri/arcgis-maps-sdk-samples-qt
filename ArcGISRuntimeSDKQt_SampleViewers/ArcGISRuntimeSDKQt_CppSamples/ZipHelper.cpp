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
/// \file ZipHelper.cpp

#include "pch.hpp" // IWYU pragma: keep
#include "ZipHelper.h"

// Qt headers
#include <QFile>

// enable logging
//#define ZIP_LOGGING_ENABLED

#ifdef Q_OS_WIN
#include <sys/utime.h>
#include <Windows.h>
#else
#include <utime.h>
#endif


//--------------------------------------------------------------------

ZipHelper::ZipHelper(QObject* parent) :
    QObject(parent),
    m_unzFile(nullptr),
    m_zipFile(nullptr)
{
}

ZipHelper::ZipHelper(const QString& path, QObject* parent) :
    QObject(parent),
    m_unzFile(nullptr),
    m_zipFile(nullptr)
{
  setPath(path);
}

//--------------------------------------------------------------------

ZipHelper::~ZipHelper()
{
    zwClose();
    zrClose();
}

//--------------------------------------------------------------------

bool ZipHelper::zrOpen(const QString &filePath)
{
    zrClose();

    m_unzFile = unzOpen64(qPrintable(filePath));

    return m_unzFile != nullptr;
}

//--------------------------------------------------------------------

void ZipHelper::zrClose()
{
    if (m_unzFile)
    {
        unzClose(m_unzFile);
        m_unzFile = nullptr;
    }
}

//--------------------------------------------------------------------

bool ZipHelper::extractCurrentFile(const QString& outputFilePath)
{
    if (!zrIsOpen())
    {
        return false;
    }

    auto result = unzOpenCurrentFile(zrHandle());

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
       qDebug() << "unzOpenCurrentFile" << result;
#endif

       return false;
    }

    unz_file_info64   fileInfo;

    result = unzGetCurrentFileInfo64(zrHandle(),
                                     &fileInfo,
                                     nullptr, 0,
                                     nullptr, 0,
                                     nullptr, 0);

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
       qDebug() << "unzGetCurrentFileInfo64" << result;
#endif


       return false;
    }

    QFile outputFile(outputFilePath);

    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << "Error opening:" << outputFilePath;
#endif
        return false;
    }

    QByteArray buffer(4096, 0);
    const auto fileName = currentFileName();
#ifdef ZIP_LOGGING_ENABLED
    qDebug() << "Extracting:" <<  fileName << " to " << outputFilePath << "Buffer size:" << buffer.size();
#endif

    quint64 readBytes = 0;

    do
    {

        // result = unzReadCurrentFile(zrHandle(), buffer.data(), buffer.size());

        if (result < 0)
        {
#ifdef ZIP_LOGGING_ENABLED
           qDebug() << "unzReadCurrentFile" << result;
#endif
           unzCloseCurrentFile(zrHandle());

           emit extractError(fileName, outputFilePath, (Result)result);

           return false;
        }
        else if (result > 0)
        {
            auto percent = ((qreal)readBytes / (qreal)fileInfo.uncompressed_size) * 100.0;

            emit extractProgress(fileName, outputFilePath, percent);

            outputFile.write(buffer.data(), result);
            readBytes += result;
        }
    }
    while (result > 0);

    outputFile.flush();
    outputFile.close();
    unzCloseCurrentFile(zrHandle());

    // Set file time stamp

    auto fileDateTime = currentFileDateTime();

    setDateTime(outputFilePath, fileDateTime, fileDateTime);

    emit extractProgress(fileName, outputFilePath, 100);

    return true;
}

//--------------------------------------------------------------------

bool ZipHelper::extractAll(QDir& outputDir)
{
    bool haveFile = gotoFirstFile();
    while (haveFile)
    {
        const auto fileName = currentFileName();
        const auto path = QFileInfo(fileName).path();
        if (!path.isEmpty() && path.compare(".") != 0)
        {
            outputDir.mkpath(path);
        }

        auto outputFilePath = outputDir.filePath(fileName);

        extractCurrentFile(outputFilePath);

        haveFile = gotoNextFile();
    }

    emit extractCompleted();

    return true;
}

//--------------------------------------------------------------------

QString ZipHelper::currentFileName()
{
    if (!zrIsOpen())
    {
        return QString();
    }

    unz_file_info64   fileInfo { currentFileInfo() };
    QByteArray        fileName(fileInfo.size_filename, '\0');

    auto result = unzGetCurrentFileInfo64(zrHandle(),
                                          &fileInfo,
                                          fileName.data(),
                                          fileName.size(),
                                          nullptr, 0,
                                          nullptr, 0);

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGetCurrentFileInfo \n").arg(result);
#endif
    }

    return fileName;
}

//--------------------------------------------------------------------

unz_file_info64 ZipHelper::currentFileInfo()
{
    unz_file_info64   fileInfo = {};

    if (!zrIsOpen())
    {
        return fileInfo;
    }

    auto result = unzGetCurrentFileInfo64(zrHandle(),
                                          &fileInfo,
                                          nullptr, 0,
                                          nullptr, 0,
                                          nullptr, 0);

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGetCurrentFileInfo \n").arg(result);
#endif
        return fileInfo;
    }

    return fileInfo;
}

//--------------------------------------------------------------------

QDateTime ZipHelper::currentFileDateTime()
{
    if (!zrIsOpen())
    {
        return QDateTime();
    }

    unz_file_info64   fileInfo;

    auto result = unzGetCurrentFileInfo64(zrHandle(),
                                          &fileInfo,
                                          nullptr, 0,
                                          nullptr, 0,
                                          nullptr, 0);

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGetCurrentFileInfo \n").arg(result);
#endif
    }

    QDateTime dateTime;

    dateTime.setDate(QDate(fileInfo.tmu_date.tm_year, fileInfo.tmu_date.tm_mon + 1, fileInfo.tmu_date.tm_mday + 1));
    dateTime.setTime(QTime(fileInfo.tmu_date.tm_hour, fileInfo.tmu_date.tm_min, fileInfo.tmu_date.tm_sec));

    return dateTime;
}

//--------------------------------------------------------------------

bool ZipHelper::gotoFirstFile()
{
    if (!zrIsOpen())
    {
        return false;
    }

    unz_global_info64 globalInfo;

    auto result = unzGetGlobalInfo64(zrHandle(), &globalInfo);

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGetGlobalInfo \n").arg(result);
#endif
        return false;
    }

    result = unzGoToFirstFile(zrHandle());

    if (result != UNZ_OK)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGoToFirstFile \n").arg(result);
#endif
        return false;
    }

    return true;
}

//--------------------------------------------------------------------

bool ZipHelper::gotoNextFile()
{
    if (!zrIsOpen())
    {
        return false;
    }

    auto result = unzGoToNextFile(zrHandle());

    if (result != UNZ_OK && result != UNZ_END_OF_LIST_OF_FILE)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << QString("Error %d with zipfile in unzGoToNextFile \n").arg(result);
#endif
    }

    return result == UNZ_OK;
}

void ZipHelper::zwClose(const QString& globalComment)
{
    if (m_zipFile)
    {
        zipClose(m_zipFile, qPrintable(globalComment));
        m_zipFile = nullptr;
    }
}

//--------------------------------------------------------------------

/*!
  \qmlmethod void ZipArchive::exists()
  \brief Gets whether archive exists.
  \internal
*/

bool ZipHelper::exists() const
{
    if (path().isEmpty())
    {
        return false;
    }

    return QFileInfo::exists(path());
}

//--------------------------------------------------------------------

bool ZipHelper::zrOpen()
{
    if (zrIsOpen())
    {
        return true;
    }

    zwClose();

    if (!exists())
    {
        return false;
    }

    auto ok = zrOpen(path());

    if (!ok)
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << "Unable to open:" << path();
#endif
    }

    return ok;
}

void ZipHelper::setPath(const QString& path)
{
    auto resolvedPath = ZipHelper::resolvedPath(path);

    if (resolvedPath == m_Path)
    {
        return;
    }

    m_Path = resolvedPath;

    zwClose();
    zrClose();

    emit pathChanged();
}

//--------------------------------------------------------------------
/*!
  \qmlmethod bool ZipArchive::extractAll(string outputPath)
  \brief Extracts all files from the archive.
  \list
    \li \a outputPath The path to the destination folder.
  \endlist

  Returns \c true if successful; otherwise \c false.
*/

bool ZipHelper::extractAll(const QString &outputPath)
{
    if (!zrOpen())
    {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << "ZIP not open" << path();
#endif
        return false;
    }

    QDir outputDir(outputPath);

#ifdef ZIP_LOGGING_ENABLED
    qDebug() << outputPath << outputDir;
#endif

    return extractAll(outputDir);
}

// Methods moved from FileUtility to enable ZipHelper::setPath to work
//--------------------------------------------------------------------

#define kResourceScheme                         QStringLiteral("qrc")
#define kResourcePathPrefix                     QStringLiteral(":/")

#define kHttpScheme                             QStringLiteral("http")
#define kHttpsScheme                            QStringLiteral("https")

#define kPathHomePrefix                         QStringLiteral("~")
#define kPathVarPrefix                          QStringLiteral("$")
#define kPathVarHome                            QStringLiteral("HOME")
#define kPathVarTemp                            QStringLiteral("TEMP")

#define kGroupName                              "Esri/ArcGISRuntime/pathVariables"

#define kPropertyPathVariablePrefix             kGroupName "."

#define kSettingsGroupPathVariables             QStringLiteral(kGroupName)

//------------------------------------------------------------------------------

QString ZipHelper::resolvedPath(const QString& path, bool useFallbacks)
{
  if (path.startsWith(kPathVarPrefix) || path.startsWith(kPathHomePrefix))
  {
    QString resolvedPath = path;

    QString varName;
    int subLength = 1;

    auto separatorIndex = resolvedPath.indexOf('/');
    if (separatorIndex < 0)
    {
      separatorIndex = resolvedPath.indexOf('\\'); // Windows
    }

    if (separatorIndex < 0)
    {
      varName = resolvedPath.mid(1);
      if (varName.isEmpty() && path.startsWith(kPathHomePrefix))
      {
        varName = kPathVarHome;
      }

      subLength = resolvedPath.length();
    }
    else if (separatorIndex < 2)
    {
      if (path.startsWith(kPathHomePrefix))
      {
        varName = kPathVarHome;
      }

      subLength = 1;
    }
    else
    {
      subLength = separatorIndex;
      varName = resolvedPath.mid(1, subLength - 1);
    }

    QString varValue;

    if (!varName.isEmpty())
    {
      varValue = pathVariable(varName, QString(), useFallbacks);

      if (!pathVariableExists(varName, useFallbacks))
      {
#ifdef ZIP_LOGGING_ENABLED
        qDebug() << "Nonexistent Path variable" << varName;
#endif
      }
    }
    else
    {
#ifdef ZIP_LOGGING_ENABLED
      qDebug() << "Path variable name not specified";
#endif
    }

    return resolvedPath.replace(0, subLength, varValue);
  }

  return path;
}

bool ZipHelper::setDateTime(const QString& filePath, const QDateTime& accessed, const QDateTime& modified)
{
  utimbuf updateTime;

  updateTime.actime = accessed.toSecsSinceEpoch();
  updateTime.modtime = modified.toSecsSinceEpoch();

  int result = utime(qPrintable(filePath), &updateTime);

  if (!result)
  {
#ifdef ZIP_LOGGING_ENABLED
    qDebug() << "error" << errno;
#endif
  }

  return result == 0;
}

QString ZipHelper::pathVariable(const QString& name, const QString& defaultValue, bool useFallbacks)
{
  if (name.isEmpty())
  {
#ifdef ZIP_LOGGING_ENABLED
    qDebug() << "Empty name";
#endif
    return defaultValue;
  }

  QString value;

  auto propertyValue = QCoreApplication::instance()->property(pathPropertyName(name));
  if (propertyValue.isValid() && !propertyValue.isNull())
  {
    value = propertyValue.toString();
  }

  if (useFallbacks)
  {


    if (value.isEmpty())
    {
      QSettings   settings;

      settings.beginGroup(kSettingsGroupPathVariables);

      value = settings.value(name).toString().trimmed();

      settings.endGroup();
    }

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)

    if (value.isEmpty() && !qEnvironmentVariableIsEmpty(name.toUtf8()))
    {
      value = QString::fromUtf8(qgetenv(name.toUtf8()));
    }
#endif
  }

  if (value.isEmpty())
  {
    value = defaultPathVariable(name);
    if (!value.isEmpty())
    {
#ifdef ZIP_LOGGING_ENABLED
      qDebug() << "Setting default" << name << "=" << value;
#endif
      setPathVariable(name, value);
    }
  }

  return value;
}

void ZipHelper::setPathVariable(const QString& name, const QString& value)
{
  if (name.isEmpty())
  {
#ifdef ZIP_LOGGING_ENABLED
    qDebug() << "Empty name, value=" << value;
#endif
    return;
  }

  if (value.isEmpty())
  {
    QCoreApplication::instance()->setProperty(pathPropertyName(name), QVariant());
  }
  else
  {
    QCoreApplication::instance()->setProperty(pathPropertyName(name), value);
  }
}

ZipHelper::PathVariableType ZipHelper::pathVariableType(const QString& name, bool useFallbacks)
{
  auto propertyValue = QCoreApplication::instance()->property(pathPropertyName(name));

  if (propertyValue.isValid() && !propertyValue.isNull())
  {
    return PathVariableType::ApplicationProperty;
  }

  if (useFallbacks)
  {


    QSettings   settings;

    settings.beginGroup(kSettingsGroupPathVariables);

    if (!settings.value(name).toString().isEmpty())
    {
      return PathVariableType::Settings;
    }

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)

    if (!qEnvironmentVariableIsEmpty(name.toUtf8()))
    {
      return PathVariableType::Environment;
    }
#endif
  }

  return PathVariableType::NotFound;
}

bool ZipHelper::pathVariableExists(const QString& name, bool useFallbacks)
{
  return pathVariableType(name, useFallbacks) != PathVariableType::NotFound;
}

QString ZipHelper::defaultPathVariable(const QString& name)
{
  if (name == kPathVarHome)
  {
    return defaultHomePath();
  }

  if (name == kPathVarTemp)
  {
    return defaultTempPath();
  }

  return QString();
}

QString ZipHelper::defaultHomePath()
{
  QString homePath;

#ifdef Q_OS_ANDROID
  homePath = "/sdcard";
#elif defined Q_OS_IOS
  homePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  homePath = QDir::homePath();
#endif

  return homePath;
}

QString ZipHelper::defaultTempPath()
{
  return QDir::tempPath();
}

QByteArray ZipHelper::pathPropertyName(const QString& name)
{
  return (QString(kPropertyPathVariablePrefix) +  name).toUtf8();
}

//--------------------------------------------------------------------
