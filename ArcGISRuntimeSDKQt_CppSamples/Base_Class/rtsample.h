
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

#ifndef RTSAMPLE_H
#define RTSAMPLE_H

#include "Map.h"
#include "MapView.h"

class SyntaxHighlighter;

namespace Ui {
class RTSample;
}

class RTSample : public QWidget
{
  Q_OBJECT

public:
  explicit RTSample(const QString &title, const QString &categoryList, const QString &shortDesc, const QString &longDesc, const QString &thumbnail);
  virtual ~RTSample();

  static const QString& getPathSampleData();
  static const QString& getPathSource();
  static QGraphicsItem* itemFromQGraphicsScene(QGraphicsScene* scene, const QPoint &pos);

  void setRunning();
  virtual void run() = 0;
  void displaySampleWidget(QWidget* widget);

  const QString& getTitle() {  return m_title;}
  const QString& getCategoryList() { return m_categoryList; }
  const QString& getShortDesc() { return m_shortDesc; }
  const QString& getLongDesc() { return m_longDesc; }
  const QString& getThumbnail() { return m_thumbnail; }

  bool running(){return m_running;}   
  Esri::ArcGISRuntime::Map* map() { return m_map; }

  // Setters
  void setSourceNameCpp(const QString& fileName);
  void setSourceNameHeader(const QString& fileName);
  void setSourceNameHelpers(const QStringList& fileNames);
  void setCodeSnippet(const QString& text);

  // utility
  bool removeDir(const QString& dirName);

protected slots:
  void onSampleRemoved();

protected:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapView* m_mapView;

private slots:
  void onBtnCopySnippetClicked();

private:
  QTextEdit* getSourceFile(const QString& fileName);
  void addSourceTab(const QString& fileName);

  Ui::RTSample *m_ui;
  QString m_title;
  QString m_categoryList;
  QString m_shortDesc;
  QString m_longDesc;
  QString m_thumbnail;
  QStringList m_sourceHelpers;

  QWidget* m_curWidget;
  QVector<SyntaxHighlighter*> m_syntaxHighlighterVector;
  static QMutex s_sampleMutex;
  static QString s_pathSampleData;
  static QString s_pathSource;

  bool m_running;
};

/* Helper class to hold thumbnail info */
class ThumbnailEntry
{
public:
  ThumbnailEntry(QString name, QString cats, QString descShort, QString descLong, QString thumb) :
    m_name(name),
    m_categories(cats),
    m_shortDesc(descShort),
    m_longDesc(descLong),
    m_thumbNail(thumb)
  {
  }
  QString m_name;
  QString m_categories;
  QString m_shortDesc;
  QString m_longDesc;
  QString m_thumbNail;
};


#endif // RTSAMPLE_H
