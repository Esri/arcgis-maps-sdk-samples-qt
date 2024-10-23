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

#include <QObject>

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
  Q_PROPERTY(bool gridVisible MEMBER m_gridVisible NOTIFY propertiesChanged)
  Q_PROPERTY(bool labelsVisible MEMBER m_labelsVisible NOTIFY propertiesChanged)
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
  bool m_gridVisible = true;
  bool m_labelsVisible = true;
  QString m_currentLineColor;
  QString m_currentLabelColor;
  QString m_currentLabelPosition;
  QString m_currentLabelFormat;

  // Constants
  const QString s_mapView = QStringLiteral("MapView");
  const QString s_sceneView = QStringLiteral("SceneView");
  const QStringList m_viewTypes = {s_mapView, s_sceneView};

  const QString s_latLong = QStringLiteral("LatLong");
  const QString s_mgrs = QStringLiteral("MGRS");
  const QString s_utm = QStringLiteral("UTM");
  const QString s_usng = QStringLiteral("USNG");
  const QStringList m_gridTypes = {s_latLong, s_mgrs, s_utm, s_usng};

  const QString s_white = QStringLiteral("White");
  const QString s_red = QStringLiteral("Red");
  const QString s_blue = QStringLiteral("Blue");
  const QString s_black = QStringLiteral("Black");
  const QStringList m_lineColors = {s_white, s_red, s_blue};
  const QStringList m_labelColors = {s_black, s_red, s_blue};

  const QString s_geographic = QStringLiteral("Geographic");
  const QString s_topLeft = QStringLiteral("Top Left");
  const QString s_topRight = QStringLiteral("Top Right");
  const QString s_bottomLeft = QStringLiteral("Bottom Left");
  const QString s_bottomRight = QStringLiteral("Bottom Right");
  const QString s_center = QStringLiteral("Center");
  const QString s_allSides = QStringLiteral("All sides");
  const QStringList m_labelPositions = {s_geographic, s_topLeft, s_topRight, s_bottomLeft, s_bottomRight, s_center, s_allSides};

  const QString s_decimalDegrees = QStringLiteral("Decimal Degrees");
  const QString s_degreesMinutesSeconds = QStringLiteral("Degrees Minutes Seconds");
  const QStringList m_labelFormats = {s_decimalDegrees, s_degreesMinutesSeconds};
};

#endif // SHOWGRID_H
