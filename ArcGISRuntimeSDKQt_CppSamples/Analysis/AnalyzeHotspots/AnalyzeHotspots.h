// [WriteFile Name=AnalyzeHotspots, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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

#ifndef ANALYZEHOTSPOTS_H
#define ANALYZEHOTSPOTS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GeoprocessingTask;
    class GeoprocessingResult;
    class ArcGISMapImageLayer;
  }
}

#include "GeoprocessingParameters.h"
#include <QQuickItem>

class AnalyzeHotspots : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool jobInProgress MEMBER m_jobInProgress NOTIFY jobInProgressChanged)
  Q_PROPERTY(QString statusText MEMBER m_jobStatus NOTIFY jobStatusChanged)

public:
  explicit AnalyzeHotspots(QQuickItem* parent = nullptr);
  ~AnalyzeHotspots() override;

  void componentComplete() override;
  static void init();

  Q_INVOKABLE void executeTaskWithDates(const QString& fromDate, const QString& toDate);

signals:
  void jobInProgressChanged();
  void jobStatusChanged();
  void displayErrorDialog(const QString& titleText, const QString& detailedText);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GeoprocessingTask* m_hotspotTask = nullptr;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_layer = nullptr;
  bool m_jobInProgress = false;
  QString m_jobStatus = QStringLiteral("Not started.");

private:
  Esri::ArcGISRuntime::GeoprocessingParameters createParameters(const QString& fromDate, const QString& toDate);
  void processResults(Esri::ArcGISRuntime::GeoprocessingResult* result);
};

#endif // ANALYZEHOTSPOTS_H
