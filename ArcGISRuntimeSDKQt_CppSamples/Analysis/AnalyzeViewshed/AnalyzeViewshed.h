// [WriteFile Name=AnalyzeViewshed, Category=Analysis]
// [Legal]
// Copyright 2016 Esri.

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

#ifndef ANALYZEVIEWSHED_H
#define ANALYZEVIEWSHED_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GraphicsOverlay;
    class GeoprocessingTask;
    class GeoprocessingResult;
    class Graphic;
  }
}

#include <QQuickItem>

class AnalyzeViewshed : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool viewshedInProgress READ viewshedInProgress NOTIFY viewshedInProgressChanged)
  Q_PROPERTY(QString statusText READ jobStatus NOTIFY jobStatusChanged)

public:
  explicit AnalyzeViewshed(QQuickItem* parent = nullptr);
  ~AnalyzeViewshed() override;

  void componentComplete() override;
  static void init();

signals:
  void viewshedInProgressChanged();
  void displayErrorDialog(const QString& titleText, const QString& detailedText);
  void jobStatusChanged();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_inputOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_resultsOverlay = nullptr;
  Esri::ArcGISRuntime::GeoprocessingTask* m_viewshedTask = nullptr;
  Esri::ArcGISRuntime::Graphic* m_inputGraphic = nullptr;
  bool m_viewshedInProgress = false;
  QString m_jobStatus;
  QObject* m_graphicParent = nullptr;

private:
  void connectSignals();
  void createOverlays();
  void calculateViewshed();
  void processResults(Esri::ArcGISRuntime::GeoprocessingResult* results);
  bool viewshedInProgress() const { return m_viewshedInProgress; }
  QString jobStatus() const { return m_jobStatus; }
};

#endif // ANALYZEVIEWSHED_H
