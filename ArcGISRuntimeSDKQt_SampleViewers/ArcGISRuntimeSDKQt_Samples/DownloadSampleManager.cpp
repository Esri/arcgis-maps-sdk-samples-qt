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

#include "DataItemListModel.h"
#include "Sample.h"
#include "SampleListModel.h"
#include "pch.hpp"

// #include "DataItem.h"
#include "DownloadSampleManager.h"

#include <QQueue>

static QString homePath()
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
      // else
      // {
      //   emit doneDownloadingPortalItem();
      //   // start next download
      //   if (auto item = m_dataItems.dequeue(); !m_dataItems.isEmpty())
      //   {
      //     downloadData(item->itemId(), homePath() + item->path().mid(1));
      //   }
      //   emit doneDownloadingPortalItem();
      // }
      emit doneDownloadingPortalItem();
      // start next download
      // qDebug() << m_dataItems.size();
      // if (auto item = m_dataItems.dequeue(); !m_dataItems.isEmpty())
      // {
      //   downloadData(item->itemId(), homePath() + item->path().mid(1));
      // }
      downloadNextItem();
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

  // connect(this, &DownloadSampleManager::doneDownloadingPortalItem, this, [this]()
  // {

  // });
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
  // createPortalItem(itemId);
  auto portalItem = new PortalItem(m_portal, itemId, this);
  // m_portalItem = std::make_unique<PortalItem>(m_portal, itemId, this);
  connect(m_portalItem, &PortalItem::loadStatusChanged, this, [this](LoadStatus status)
  {
    qDebug() << "PortalItem load status changed: " << status;
  });

  connect(m_portalItem, &PortalItem::doneLoading, this,
          [this](const Error& error)
  {
    bool success = error.isEmpty();
    qDebug() << "m_portalItem done loading" << success;

    // Copy important parts before potential delete.
    auto type = m_portalItem->type();
    auto name = m_portalItem->name();
    auto id = m_portalItem->itemId();

    if (!success)
    {
      // delete m_portalItem;
      // m_portalItem = nullptr;
    }

    emit portalItemDoneLoading(
          success,
          id,
          type == PortalItemType::CodeSample ? name : QString());

  });

  connect(m_portalItem, &PortalItem::fetchDataProgressChanged, this,
          [this](const NetworkRequestProgress& progress)
  {
    emit portalItemFetchDataProgress(m_portalItem->itemId(), progress.progressPercentage());
  });

  // connect(m_portalItem, &PortalItem::fetchDataCompleted,
  //         this, [this](bool success)
  // {
  //   auto id = m_portalItem->itemId();
  //   delete m_portalItem;
  //   m_portalItem = nullptr;
  //   emit portalItemFetchDataCompleted(id, success);
  // });

  qDebug() << "calling m_portalItem->load()";
  m_portalItem->load();

}

// void DownloadSampleManager::downloadAllDataItems()
// {
//     if (!m_dataItems.isEmpty())
//       m_dataItems.clear();
//     // for ( int i = 0; i < samples()->size(); i++)
//     // {
//     //   auto sample = samples()->get(i);
//     // }
//     for ( auto sample : *samples())
//     {
//       for (auto dataItem : *sample->dataItems())
//       {
//         if (!dataItem->exists())
//         {
//           qDebug() << sample->name().toString() << "; " << dataItem->exists() << "; " << dataItem->path();
//           m_dataItems.enqueue(dataItem);
//         }
//       }
//     }

//     setDownloadInProgress(true);
//     downloadNextItem();
//     // if (!m_dataItems.isEmpty())
//     // {
//     //   qDebug() << m_dataItems.size();
//     //   auto item = m_dataItems.dequeue();
//     //   downloadData(item->itemId(), homePath() + item->path().mid(1));
//     // }

//     // for (auto item : items)
//     // {
//     //   downloadData(item->itemId(), homePath() + item->path().mid(1));
//     // }

// }

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

void DownloadSampleManager::downloadNextItem()
{
  if (!m_dataItems.isEmpty())
  {
    qDebug() << m_dataItems.size();
    setDownloadText(QString("Remaining items in queue: %1").arg(m_dataItems.size()));
    emit downloadTextChanged();
    auto item = m_dataItems.dequeue();
    downloadData(item->itemId(), homePath() + item->path().mid(1));
  }
  else {
    setDownloadInProgress(false);
    emit downloadInProgressChanged();
  }
}
