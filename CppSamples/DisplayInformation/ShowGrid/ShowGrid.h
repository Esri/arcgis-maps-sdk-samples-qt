// [WriteFile Name=ShowGrid, Category=DisplayInformation]
// [Legal]
// Copyright 2024 Esri.

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

#ifndef SHOWGRID_H
#define SHOWGRID_H

namespace Esri::ArcGISRuntime
{
class Grid;
class Map;
class MapQuickView;
class Scene;
class SceneQuickView;
class SimpleLineSymbol;
class TextSymbol;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h");
Q_MOC_INCLUDE("SceneQuickView.h");

class ShowGrid : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY viewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY viewChanged)
  Q_PROPERTY(QString currentViewType MEMBER m_currentViewType NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList viewTypes MEMBER m_viewTypes CONSTANT)
  Q_PROPERTY(QString currentGridType MEMBER m_currentGridType NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList gridTypes MEMBER m_gridTypes CONSTANT)
  Q_PROPERTY(QString currentLineColor MEMBER m_currentLineColor NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList lineColors MEMBER m_lineColors CONSTANT)
  Q_PROPERTY(QString currentLabelColor MEMBER m_currentLabelColor NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList labelColors MEMBER m_labelColors CONSTANT)
  Q_PROPERTY(QString currentLabelPosition MEMBER m_currentLabelPosition NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList labelFormats MEMBER m_labelFormats CONSTANT)
  Q_PROPERTY(QString currentLabelFormat MEMBER m_currentLabelFormat NOTIFY propertiesChanged)
  Q_PROPERTY(QStringList labelPositions MEMBER m_labelPositions CONSTANT)

public:
  explicit ShowGrid(QObject* parent = nullptr);
  ~ShowGrid() override;

  static void init();

  Q_INVOKABLE void setGridVisible(bool visible);
  Q_INVOKABLE void setLabelsVisible(bool visible);
  Q_INVOKABLE void setViewType(const QString& viewType);
  Q_INVOKABLE void setGridType(const QString& gridType);
  Q_INVOKABLE void setLineColor(const QString& lineColor);
  Q_INVOKABLE void setLabelColor(const QString& labelColor);
  Q_INVOKABLE void setLabelPosition(const QString& labelPosition);
  Q_INVOKABLE void setLabelFormat(const QString& labelFormat);

signals:
  void viewChanged();
  void propertiesChanged();

private:
  // Declare private methods
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  Esri::ArcGISRuntime::SceneQuickView* sceneView() const;
  void setSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

  // Declare private members
  Esri::ArcGISRuntime::Grid* m_grid = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;

  // Properties
  QString m_currentViewType;
  QString m_currentGridType;
  QString m_currentLineColor;
  QString m_currentLabelColor;
  QString m_currentLabelPosition;
  QString m_currentLabelFormat;

  // Constants
  QStringList m_viewTypes = {"MapView", "SceneView"};
  QStringList m_gridTypes = {"LatLong", "MGRS", "UTM", "USNG"};
  QStringList m_lineColors = {"White", "Red", "Blue"};
  QStringList m_labelColors = {"Black", "Red", "Blue"};
  QStringList m_labelPositions = {"Geographic", "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center", "All sides"};
  QStringList m_labelFormats = {"Decimal Degrees", "Degrees Minutes Seconds"};
};

#endif // SHOWGRID_H
