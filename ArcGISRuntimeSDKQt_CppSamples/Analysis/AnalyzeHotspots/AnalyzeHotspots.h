// [WriteFile Name=AnalyzeHotspots, Category=Analysis]
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

#ifndef ANALYZEHOTSPOTS_H
#define ANALYZEHOTSPOTS_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class GeoprocessingTask;
    class GeoprocessingJob;
    class GeoprocessingResult;
    class ArcGISMapImageLayer;
  }
}

#include "GeoprocessingParameters.h"
#include <QQuickItem>

class AnalyzeHotspots : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool jobInProgress READ jobInProgress NOTIFY jobInProgressChanged)
  Q_PROPERTY(QString statusText READ jobStatus NOTIFY jobStatusChanged)

public:
  AnalyzeHotspots(QQuickItem* parent = nullptr);
  ~AnalyzeHotspots();

  void componentComplete() Q_DECL_OVERRIDE;

  Q_INVOKABLE void executeTaskWithDates(const QString& fromDate, const QString& toDate);

signals:
  void jobInProgressChanged();
  void jobStatusChanged();
  void displayErrorDialog(const QString& titleText, const QString& detailedText);

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::MapQuickView* m_mapView;
  Esri::ArcGISRuntime::GeoprocessingTask* m_hotspotTask;
  Esri::ArcGISRuntime::GeoprocessingJob* m_job;
  Esri::ArcGISRuntime::ArcGISMapImageLayer* m_layer;
  bool m_jobInProgress;
  QString m_jobStatus;

private:
  bool jobInProgress() const { return m_jobInProgress; }
  QString jobStatus() const { return m_jobStatus; }
  Esri::ArcGISRuntime::GeoprocessingParameters createParameters(const QString& fromDate, const QString& toDate);
  void processResults(Esri::ArcGISRuntime::GeoprocessingResult* result);
};

#endif // ANALYZEHOTSPOTS_H
