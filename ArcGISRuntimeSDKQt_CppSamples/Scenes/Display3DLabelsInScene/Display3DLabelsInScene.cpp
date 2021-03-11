// [WriteFile Name=Display3DLabelsInScene, Category=Scenes]
// [Legal]
// Copyright 2021 Esri.

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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Display3DLabelsInScene.h"

#include "ArcadeLabelExpression.h"
#include "ArcGISTiledElevationSource.h"
#include "FeatureLayer.h"
#include "GroupLayer.h"
#include "LabelDefinition.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "TextSymbol.h"

using namespace Esri::ArcGISRuntime;

Display3DLabelsInScene::Display3DLabelsInScene(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(QUrl("https://www.arcgis.com/home/item.html?id=850dfee7d30f4d9da0ebca34a533c169"), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  connect(m_scene, &Scene::doneLoading, this, [this]()
  {
    for (Layer* layer : *m_scene->operationalLayers())
    {
      if (layer->name() == "Gas")
      {
        // The gas layer is a GroupLayer type consisting of Layer types.
        // Labels can only be displayed on FeatureLayer types, so we must first convert it to a FeatureLayer class.
        GroupLayer* gasGroupLayer = dynamic_cast<GroupLayer*>(layer);
        if (!gasGroupLayer)
          continue;

        FeatureLayer* gasFeatureLayer = dynamic_cast<FeatureLayer*>(gasGroupLayer->layers()->first());
        if (gasFeatureLayer)
          display3DLabelsOnFeatureLayer(gasFeatureLayer);

        break;
      }
    }
  });
}

void Display3DLabelsInScene::display3DLabelsOnFeatureLayer(FeatureLayer* featureLayer)
{
  TextSymbol* textSymbol = new TextSymbol(this);
  textSymbol->setColor(QColor("#ffa500"));
  textSymbol->setHaloColor(QColor(Qt::white));
  textSymbol->setHaloWidth(2.0);
  textSymbol->setSize(14.0);

  LabelDefinition* labelDefinition = new LabelDefinition(this);
  labelDefinition->setExpression(new ArcadeLabelExpression("Text($feature.INSTALLATIONDATE, 'D MMM Y')", this));
  labelDefinition->setPlacement(LabelingPlacement::LineAboveAlong);
  labelDefinition->setUseCodedValues(true);
  labelDefinition->setTextSymbol(textSymbol);

  featureLayer->labelDefinitions()->clear();
  featureLayer->labelDefinitions()->append(labelDefinition);
  featureLayer->setLabelsEnabled(true);
}

Display3DLabelsInScene::~Display3DLabelsInScene() = default;

void Display3DLabelsInScene::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<Display3DLabelsInScene>("Esri.Samples", 1, 0, "Display3DLabelsInSceneSample");
}

SceneQuickView* Display3DLabelsInScene::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void Display3DLabelsInScene::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

