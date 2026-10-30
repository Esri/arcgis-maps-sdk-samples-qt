// [WriteFile Name=AddFeaturesWithSharedTemplates, Category=EditData]
// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifndef ADDFEATURESWITHSHAREDTEMPLATES_H
#define ADDFEATURESWITHSHAREDTEMPLATES_H

// Qt headers
#include <QList>
#include <QObject>
#include <QString>
#include <QVariantList>

namespace Esri::ArcGISRuntime
{
  class GeometryEditor;
  class Map;
  class MapQuickView;
  class ServiceGeodatabase;
  class SharedTemplate;
} // namespace Esri::ArcGISRuntime

class QTimer;

Q_MOC_INCLUDE("MapQuickView.h")

class AddFeaturesWithSharedTemplates : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QVariantList templates READ templates NOTIFY templatesChanged)
  Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
  Q_PROPERTY(bool drawing READ drawing NOTIFY drawingChanged)
  Q_PROPERTY(bool hasPendingEdits READ hasPendingEdits NOTIFY hasPendingEditsChanged)
  Q_PROPERTY(bool canComplete READ canComplete NOTIFY canCompleteChanged)

public:
  explicit AddFeaturesWithSharedTemplates(QObject* parent = nullptr);
  ~AddFeaturesWithSharedTemplates() override;

  static void init();

  Q_INVOKABLE void startDrawing(int index);
  Q_INVOKABLE void cancelDrawing();
  Q_INVOKABLE void completeDrawing();
  Q_INVOKABLE void saveEdits();
  Q_INVOKABLE void undoEdits();

  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  QVariantList templates() const;
  QString statusText() const;
  bool busy() const;
  bool ready() const;
  bool drawing() const;
  bool hasPendingEdits() const;
  bool canComplete() const;

signals:
  void mapViewChanged();
  void templatesChanged();
  void statusTextChanged();
  void busyChanged();
  void readyChanged();
  void drawingChanged();
  void hasPendingEditsChanged();
  void canCompleteChanged();
  void errorOccurred(const QString& message);

private:
  Q_DISABLE_COPY_MOVE(AddFeaturesWithSharedTemplates)

  struct TemplateItem
  {
    Esri::ArcGISRuntime::SharedTemplate* sharedTemplate = nullptr;
    qint64 layerId = 0;
  };

  void loadTemplates_();
  void createSwatch_(qsizetype index);
  void updateCanComplete_();
  void setStatusText_(const QString& status);
  void setBusy_(bool busy);
  void setHasPendingEdits_(bool hasPendingEdits);
  void finishOperation_(const QString& status);
  void reportError_(const QString& status, const QString& message);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GeometryEditor* m_geometryEditor = nullptr;
  Esri::ArcGISRuntime::ServiceGeodatabase* m_serviceGeodatabase = nullptr;
  QList<TemplateItem> m_templateItems;
  QVariantList m_templates;
  QTimer* m_statusResetTimer = nullptr;
  QString m_statusText;
  int m_activeTemplateIndex = -1;
  bool m_busy = true;
  bool m_ready = false;
  bool m_hasPendingEdits = false;
  bool m_canComplete = false;
};

#endif // ADDFEATURESWITHSHAREDTEMPLATES_H
