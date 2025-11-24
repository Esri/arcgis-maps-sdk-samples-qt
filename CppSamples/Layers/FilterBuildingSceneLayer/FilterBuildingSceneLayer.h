// [WriteFile Name=FilterBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

#ifndef FILTERBUILDINGSCENELAYER_H
#define FILTERBUILDINGSCENELAYER_H

// Qt headers
#include <QFuture>
#include <QMouseEvent>
#include <QObject>

// STL headers
#include <Popup.h>

namespace Esri::ArcGISRuntime
{
  class BuildingComponentSublayer;
  class BuildingFilter;
  class BuildingFilterBlock;
  class BuildingGroupSublayer;
  class BuildingSceneLayer;
  class BuildingSceneLayerAttributeStatistics;
  class BuildingSublayerListModel;
  class FeatureLayer;
  class IdentifyLayerResult;
  class LayerListModel;
  class LocalSceneQuickView;
  class Scene;
} // namespace Esri::ArcGISRuntime

Q_MOC_INCLUDE("BuildingSublayerListModel.h")
Q_MOC_INCLUDE("LocalSceneQuickView.h");

class FilterBuildingSceneLayer : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QStringList floorList READ floorList NOTIFY floorListChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView READ localSceneView WRITE setLocalSceneView NOTIFY localSceneViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::Popup* popup MEMBER m_popup NOTIFY popupChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::BuildingSublayerListModel* sublayerListModel MEMBER m_sublayerListModel NOTIFY sublayerListModelChanged)

public:
  explicit FilterBuildingSceneLayer(QObject* parent = nullptr);
  ~FilterBuildingSceneLayer() override;

  static void init();
  QStringList floorList() const;
  void getCategoriesList();
  void getFloorList();
  Q_INVOKABLE void clearSelection();
  Q_INVOKABLE Esri::ArcGISRuntime::BuildingSublayerListModel* getComponentSubLayerListModel(int layerId);
  Q_INVOKABLE void updateFloorFilter(const QString& selectedFloor);

signals:
  void localSceneViewChanged();
  void floorListChanged();
  void sublayerListModelChanged();
  void popupChanged();

private slots:
  void onMouseClicked(QMouseEvent& mouseEvent);

private:
  Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView() const;
  void setLocalSceneView(Esri::ArcGISRuntime::LocalSceneQuickView* localSceneView);

  Esri::ArcGISRuntime::BuildingComponentSublayer* m_selectedSublayer = nullptr;
  Esri::ArcGISRuntime::BuildingFilter* m_buildingFilter = nullptr;
  Esri::ArcGISRuntime::BuildingGroupSublayer* m_fullModelSublayer = nullptr;
  Esri::ArcGISRuntime::BuildingSceneLayer* m_buildingSceneLayer = nullptr;
  Esri::ArcGISRuntime::BuildingSublayerListModel* m_sublayerListModel = nullptr;
  Esri::ArcGISRuntime::LocalSceneQuickView* m_localSceneView = nullptr;
  Esri::ArcGISRuntime::Popup* m_popup = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;

  QFuture<Esri::ArcGISRuntime::IdentifyLayerResult*> m_future;
  QList<Esri::ArcGISRuntime::BuildingFilterBlock*> m_blocks;
  QStringList m_floorList;
};

#endif // FILTERBUILDINGSCENELAYER_H
