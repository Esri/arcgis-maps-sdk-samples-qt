// [WriteFile Name=ApplyMapAlgebra, Category=Layers]
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

#ifndef APPLYMAPALGEBRA_H
#define APPLYMAPALGEBRA_H

// Qt headers
#include <QFutureWatcher>
#include <QQuickItem>

namespace Esri::ArcGISRuntime
{
  class ColormapRenderer;
  class ContinuousField;
  class DiscreteField;
  class Map;
  class MapQuickView;
  class RasterLayer;
} // namespace Esri::ArcGISRuntime

class ApplyMapAlgebra : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(bool dataAvailable READ dataAvailable NOTIFY dataAvailableChanged)
  Q_PROPERTY(bool resultsAvailable READ resultsAvailable NOTIFY resultsAvailableChanged)
  Q_PROPERTY(bool showGeomorphicResults READ showGeomorphicResults WRITE setShowGeomorphicResults NOTIFY showGeomorphicResultsChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit ApplyMapAlgebra(QQuickItem* parent = nullptr);
  ~ApplyMapAlgebra() override;

  static void init();
  void componentComplete() override;

  Q_INVOKABLE void categorize();

  bool busy() const;
  bool dataAvailable() const;
  bool resultsAvailable() const;
  bool showGeomorphicResults() const;
  QString errorMessage() const;

signals:
  void busyChanged();
  void dataAvailableChanged();
  void resultsAvailableChanged();
  void showGeomorphicResultsChanged();
  void errorMessageChanged();

private:
  // Sample workflow
  void setupMap();
  void createElevationField();
  void createGeomorphicField();
  void exportGeomorphicField();

  // Display helpers
  void displayElevationRaster();
  void displayGeomorphicRaster(const QString& rasterFilePath);
  void updateVisibleRasterLayer();

  // Internal state
  void setBusy(bool busy);
  void setDataAvailable(bool dataAvailable);
  void setResultsAvailable(bool resultsAvailable);
  void setShowGeomorphicResults(bool showGeomorphicResults);
  void setErrorMessage(const QString& errorMessage);

  // File system helpers
  QString elevationRasterPath() const;
  QString tempOutputDirectory() const;
  void clearPreviousExportedFiles(const QString& outputDirectory) const;

  // Map and raster state
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_elevationRasterLayer = nullptr;
  Esri::ArcGISRuntime::RasterLayer* m_geomorphicRasterLayer = nullptr;
  Esri::ArcGISRuntime::ContinuousField* m_elevationField = nullptr;
  Esri::ArcGISRuntime::DiscreteField* m_geomorphicField = nullptr;

  // Async workflow
  QFutureWatcher<Esri::ArcGISRuntime::ContinuousField*> m_createElevationFieldWatcher;
  QFutureWatcher<Esri::ArcGISRuntime::DiscreteField*> m_createGeomorphicFieldWatcher;
  QFutureWatcher<QStringList> m_exportGeomorphicFieldWatcher;

  // Local paths
  QString m_dataPath;

  // UI state
  bool m_busy = false;
  bool m_dataAvailable = false;
  bool m_resultsAvailable = false;
  bool m_showGeomorphicResults = true;
  QString m_errorMessage;
};

#endif // APPLYMAPALGEBRA_H
