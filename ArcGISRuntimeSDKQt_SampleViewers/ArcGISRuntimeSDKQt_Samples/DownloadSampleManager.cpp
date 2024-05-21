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

#include "DownloadSampleManager.h"

DownloadSampleManager::DownloadSampleManager(QObject* parent) :
  SampleManager(parent)
{
}

DownloadSampleManager::~DownloadSampleManager() = default;

void DownloadSampleManager::init()
{
  SampleManager::init();

  connect(this, &DownloadSampleManager::portalItemDoneLoading,
          this, [this](bool success,
                       const QString& itemId,
                       const QString& folderName)
  {
    if (success)
    {
      m_formattedPath = formattedPath(m_outputPath, folderName);
      fetchData(m_formattedPath);
    }
    else
    {
      m_outputPath = "";
      m_formattedPath = "";
      emit downloadDataFailed(itemId);
    }
  });

  connect(this, &DownloadSampleManager::portalItemFetchDataCompleted,
          this, [this](const QString& itemId, bool success)
  {
    QString formattedPath = m_formattedPath;
    m_outputPath = "";
    m_formattedPath = "";
    if (success)
    {
      if (QFileInfo(formattedPath).suffix() == "zip")
      {
        emit unzipFile(formattedPath,
                       QFileInfo(formattedPath).dir().absolutePath());
      }
      else
      {
        emit doneDownloadingPortalItem();
      }
    }
    else
    {
      emit downloadDataFailed(itemId);
    }
  });

  connect(this, &DownloadSampleManager::portalItemFetchDataProgress, this,
          [this](const QString& /*portalId*/, int progress)
  {
    setDownloadProgress(progress);
  });
}

void DownloadSampleManager::downloadData(const QString& itemId, const QString& outputPath)
{
  if (!isPortalLoaded())
  {
    // first load portal if not already loaded
    connect(this, &DownloadSampleManager::portalDoneLoading,
            this, [itemId, outputPath, this](bool success)
    {
      if (success)
        downloadData(itemId, outputPath);
      else
        emit downloadDataFailed(itemId);
    });

    loadPortal();
    return;
  }

  m_outputPath = outputPath;
  createPortalItem(itemId);
}

QString DownloadSampleManager::formattedPath(const QString& outputPath,
                                             const QString& folderName)
{
  // first make sure output path exists
  if (!QFile::exists(outputPath))
  {
    QFileInfo fileInfo = QFileInfo(outputPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
      dir.mkpath(".");
  }

  // create the download data task
  QString formattedFilePath = outputPath;

  // check if the item is code sample
  if (!folderName.isEmpty())
  {
    QFileInfo fileInfo = QFileInfo(outputPath);
    QString dir = fileInfo.dir().absolutePath();
    formattedFilePath = dir + "/" + folderName;
  }

  return formattedFilePath;
}

QString DownloadSampleManager::api() const
{
  return QString();
}
