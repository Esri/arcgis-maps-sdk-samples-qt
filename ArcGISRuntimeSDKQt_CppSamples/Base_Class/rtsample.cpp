
// Copyright 2015 ESRI 
// 
// All rights reserved under the copyright laws of the United States 
// and applicable international laws, treaties, and conventions. 
// 
// You may freely redistribute and use this sample code, with or 
// without modification, provided you include the original copyright 
// notice and use restrictions. 
// 
// See the Sample code usage restrictions document for further information. 
//

#include "rtsample.h"
#include "ui_rtsample.h"
//#include "ArcGISRuntime.h"
//#include "LocalServer.h"
#include "syntax_highlighter.h"

QString RTSample::s_pathSampleData;
QString RTSample::s_pathSource;
QMutex RTSample::s_sampleMutex;

RTSample::RTSample(const QString& title, const QString& categoryList, const QString& shortDesc, const QString& longDesc, const QString& thumbnail) :
  QWidget(nullptr),
  m_map(nullptr),
  m_mapView(nullptr),
  m_ui(new Ui::RTSample),
  m_title(title),
  m_categoryList(categoryList),
  m_shortDesc(shortDesc),
  m_longDesc(longDesc),
  m_thumbnail(thumbnail),
  m_curWidget(nullptr),
  m_running(false)
{
  m_ui->setupUi(this);
  m_ui->description->setText(m_longDesc);
  m_ui->description->setTextInteractionFlags(Qt::TextBrowserInteraction);
  m_ui->thumbnail->setPixmap(m_thumbnail);

  qDebug() << "Creating Sample: " << getTitle();
}

RTSample::~RTSample()
{
  if (m_ui->btnCopySnippet->isEnabled())
    disconnect(m_ui->btnCopySnippet, SIGNAL(clicked()), this, SLOT(onBtnCopySnippetClicked()));

  while (!m_syntaxHighlighterVector.isEmpty())
  {
    SyntaxHighlighter* syntaxHighlighter = m_syntaxHighlighterVector.back();
    m_syntaxHighlighterVector.pop_back();
    delete syntaxHighlighter;
    syntaxHighlighter = nullptr;
  }
  qDebug() << "Deleting Sample: " << getTitle();

  delete m_ui;
  m_ui = nullptr;

  m_running = false;
}

const QString& RTSample::getPathSampleData()
{
//  if (s_pathSampleData.length() < 1)
//  {
//    QString path = Esri::ArcGISRuntime::ArcGISRuntime::installDirectory();
//    path.append("/sdk/samples/data");
//    s_pathSampleData = QDir::toNativeSeparators(path)+ QString(QDir::separator());
//  }

  return s_pathSampleData;
}

const QString& RTSample::getPathSource()
{
//  if (s_pathSource.length() < 1)
//  {
//    QString path = Esri::ArcGISRuntime::ArcGISRuntime::installDirectory();
//    path.append("/sdk/samples/QtSampleApp/samplecode");
//    QDir dataDir(path); // using QDir to convert to correct file separator
//    s_pathSource = dataDir.path() + QDir::separator();
//  }    
  return s_pathSource;
}

QGraphicsItem* RTSample::itemFromQGraphicsScene(QGraphicsScene* scene, const QPoint& pos)
{
  if (scene == nullptr)
    return nullptr;

#ifdef ESRIQTSAMPLE_QT5
  return scene->itemAt(pos, QTransform());
#else
  return scene->itemAt(pos);
#endif
}

void RTSample::setRunning()
{
  qDebug() << "Running Sample: " << getTitle();
  QMutexLocker lock(&s_sampleMutex);
//  if (EsriRuntimeQt::LocalServer::instance().isRunning())
//    EsriRuntimeQt::LocalServer::instance().shutdownAndWait();

  m_running = true;
}

void RTSample::displaySampleWidget(QWidget* widget)
{
  m_ui->tab_live_sample_layout->addWidget(widget);
  m_curWidget = widget;
}

void RTSample::addSourceTab(const QString& fileName)
{
  QString sourceFile(fileName);
  sourceFile.prepend(getPathSource());
  m_ui->Source_tabWidget->addTab(getSourceFile(sourceFile), fileName);
  m_ui->Source_tabWidget->setToolTip(sourceFile);
}

void RTSample::onSampleRemoved()
{
  qDebug() << "Removing Sample: " << getTitle();
  QMutexLocker lock(&s_sampleMutex);
  if (m_curWidget)
  {
    m_ui->tab_live_sample_layout->removeWidget(m_curWidget);
  }
  m_curWidget = nullptr;
}

void RTSample::onBtnCopySnippetClicked()
{
  QString strSnippet = m_ui->txtCodeSnippet->toPlainText();
  if (!strSnippet.isNull() && !strSnippet.isEmpty())
  {
//    QClipboard *clipboard = QApplication::clipboard();
//    clipboard->setText(strSnippet);
  }
}

QTextEdit* RTSample::getSourceFile(const QString& fileName)
{
  QTextEdit* editor = new QTextEdit;
  QFont font("Courier New", 9);
  editor->setFont(font);
  editor->setReadOnly(true);
  QFile file(fileName);
  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream in(&file);
    editor->setText(in.readAll());
  }
  m_syntaxHighlighterVector.push_back(new SyntaxHighlighter(editor->document()));
  return editor;
}

void RTSample::setSourceNameCpp(const QString& fileName)
{
  // list the sample's main cpp file in the gui
  QString fileCpp(fileName);
  fileCpp.prepend(getPathSource());
  m_ui->cpp_file_tab_layout->addWidget(getSourceFile(fileCpp));
  qDebug() << "widget added";
}

void RTSample::setSourceNameHeader(const QString& fileName)
{
  // list the sample's main header file in the gui
  QString fileH(fileName);
  fileH.prepend(getPathSource());
  m_ui->header_file_tab_layout->addWidget(getSourceFile(fileH));
}

void RTSample::setSourceNameHelpers(const QStringList& fileNames)
{
  m_sourceHelpers = fileNames;

  foreach(QString file, fileNames)
  {
    addSourceTab(file);
  }
}

void RTSample::setCodeSnippet(const QString& text)
{
  if (text.isNull() || text.isEmpty())
  {
    m_ui->txtCodeSnippet->setText("");
    m_ui->btnCopySnippet->setEnabled(false);
  }
  else
  {
    m_ui->btnCopySnippet->setEnabled(true);
    QString str(text);
    m_ui->txtCodeSnippet->setText(str.replace("&lt;", "<").replace("&gt;", ">"));
    m_syntaxHighlighterVector.push_back(new SyntaxHighlighter(m_ui->txtCodeSnippet->document()));
    connect(m_ui->btnCopySnippet, SIGNAL(clicked()), this, SLOT(onBtnCopySnippetClicked()));
  }
}


bool RTSample::removeDir(const QString& dirName)
{
  bool res = true;
  QDir dir(dirName);

  if (dir.exists(dirName)) {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
      if (info.isDir()) {
        res = removeDir(info.absoluteFilePath());
      }
      else {
        res = QFile::remove(info.absoluteFilePath());
      }
      if (!res) {
        return res;
      }
    }
    res = dir.rmdir(dirName);
  }

  return res;
}
