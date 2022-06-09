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

#ifndef SAMPLEMANAGER_H
#define SAMPLEMANAGER_H

class QAbstractItemModel;
class CategoryListModel;
class QTemporaryDir;
class SampleListModel;
class SourceCode;
class SampleCategory;
class Sample;
class PermissionsHelper;

#include <QDir>
#include <QJsonDocument>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>

class SampleManager : public QObject
{
  Q_OBJECT

  Q_PROPERTY(SampleListModel* samples READ samples NOTIFY sampleInitComplete)
  Q_PROPERTY(CategoryListModel* categories READ categories NOTIFY sampleInitComplete)
  Q_PROPERTY(CurrentMode currentMode READ currentMode WRITE setCurrentMode NOTIFY currentModeChanged)
  Q_PROPERTY(Sample* currentSample READ currentSample WRITE setCurrentSample NOTIFY currentSampleChanged)
  Q_PROPERTY(SampleCategory* currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged)
  Q_PROPERTY(QString currentSourceCode READ currentSourceCode NOTIFY currentSourceCodeChanged)
  Q_PROPERTY(QUrl apiReferenceUrl READ apiReferenceUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(QUrl qtSdkUrl READ qtSdkUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(QUrl qtSamplesUrl READ qtSamplesUrl NOTIFY sampleInitComplete)
  Q_PROPERTY(bool downloadInProgress READ downloadInProgress WRITE setDownloadInProgress NOTIFY downloadInProgressChanged)
  Q_PROPERTY(QString downloadText READ downloadText WRITE setDownloadText NOTIFY downloadTextChanged)
  Q_PROPERTY(double downloadProgress READ downloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged)

public:
  explicit SampleManager(QObject* parent = nullptr);
  ~SampleManager() override;

  Q_INVOKABLE virtual void init();

  Q_INVOKABLE void setSourceCodeIndex(int i);
  Q_INVOKABLE void setupProxy(const QString& hostName, quint16 port, const QString& user, const QString& pw);
  Q_INVOKABLE void doneDownloading() { emit doneDownloadingChanged(); }
  Q_INVOKABLE void setApiKey(bool isSupportsApiKey = true);

  enum CurrentMode
  {
    LiveSampleView,
    SourceCodeView,
    DescriptionView,
    ManageOfflineDataView,
    NetworkRequiredView,
    DownloadDataView
  };
  Q_ENUM(CurrentMode)

signals:
  void sampleInitComplete();
  void currentModeChanged();
  void currentSampleChanged();
  void currentCategoryChanged();
  void currentSourceCodeChanged();
  void doneDownloadingChanged();
  void downloadInProgressChanged();
  void downloadTextChanged();
  void downloadProgressChanged();
  void apiKeyRequired(const QString& apiKey);

protected:
  void setDownloadProgress(double progress);
  virtual void buildCategoriesList();
  SampleCategory* createCategory(const QString& name, const QString& displayName, const QDir& dir);
  bool appendCategoryToManager(SampleCategory* category);

private:
  SampleListModel* buildSamplesList(const QDir& dir, const QString& prefix);
  QVariantMap toVariantMap(const QString& json);
  QVariant fileUrl(const QString& scheme, const QString& path);
  QString readTextFile(const QString& filePath);
  SampleListModel* samples() { return m_allSamples; }
  CategoryListModel* categories() { return m_categories; }
  CurrentMode currentMode() { return m_currentMode; }
  void setCurrentMode(const CurrentMode& mode);
  Sample* currentSample() const { return m_currentSample; }
  void setCurrentSample(Sample* sample);
  void setCurrentSample(const QVariant& sample);
  SampleCategory* currentCategory() const { return m_currentCategory; }
  void setCurrentCategory(SampleCategory* category);
  QString currentSourceCode() const { return m_currentSourceCode; }
  QUrl apiReferenceUrl() const { return m_apiReferenecUrl; }
  QUrl qtSdkUrl() const { return m_qtSdkUrl; }
  QUrl qtSamplesUrl() const { return m_qtSamplesUrl; }
  bool downloadInProgress() const { return m_downloadInProgress; }
  void setDownloadInProgress(bool inProgress);
  QString downloadText() const { return m_downloadText; }
  void setDownloadText(const QString& downloadText);
  double downloadProgress() const { return m_downloadProgress; }
  void createAndSetTempDirForLocalServer();

private:
  CategoryListModel* m_categories = nullptr;
  SampleListModel* m_allSamples = nullptr;
  CurrentMode m_currentMode = CurrentMode::LiveSampleView;
  Sample* m_currentSample = nullptr;
  SampleCategory* m_currentCategory = nullptr;
  QString m_currentSourceCode;
  QUrl m_apiReferenecUrl = QUrl("https://links.esri.com/qtApiRef");
  QUrl m_qtSdkUrl = QUrl("https://links.esri.com/qtDevelopersPage");
  QUrl m_qtSamplesUrl = QUrl("https://links.esri.com/qtSamples");
  bool m_downloadInProgress = false;
  QString m_downloadText = QString("Downloading");
  double m_downloadProgress = 0.0;
  PermissionsHelper* m_permissionsHelper = nullptr;
  std::unique_ptr<QTemporaryDir> m_tempDir;
};

#endif // SAMPLEMANAGER_H
