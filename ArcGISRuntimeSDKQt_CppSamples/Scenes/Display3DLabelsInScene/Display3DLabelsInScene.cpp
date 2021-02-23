
// Copyright 2021 ESRI
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

#include "Display3DLabelsInScene.h"

#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "PortalItem.h"
#include "FeatureLayer.h"
#include "GroupLayer.h"
#include "Layer.h"
#include "TextSymbol.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

Display3DLabelsInScene::Display3DLabelsInScene(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(QUrl("https://www.arcgis.com/home/item.html?id=850dfee7d30f4d9da0ebca34a533c169"), this))
{
  // create a new elevation source from Terrain3D rest service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  connect(m_scene, &Scene::doneLoading, this, [this]()
  {
    for (Layer* layer : *m_scene->operationalLayers())
    {
      if(layer->name() == "Gas")
      {
        // The gas layer is a GroupLayer type consisting of Layer types.
        // Labels can only be displayed on FeatureLayer types, so we must first convert it to a FeatureLayer class.
        GroupLayer* gasGroupLayer = dynamic_cast<GroupLayer*>(layer);
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
  textSymbol->setAngle(0);
  textSymbol->setBackgroundColor(QColor(Qt::transparent));
  textSymbol->setOutlineColor(QColor(Qt::white));
  textSymbol->setColor(QColor("#ffa500"));
  textSymbol->setHaloColor(QColor(Qt::white));
  textSymbol->setHaloWidth(2.0);
  textSymbol->setHorizontalAlignment(HorizontalAlignment::Center);
  textSymbol->setVerticalAlignment(VerticalAlignment::Middle);
  textSymbol->setKerningEnabled(false);
  textSymbol->setOffsetX(0);
  textSymbol->setOffsetY(0);
  textSymbol->setFontDecoration(FontDecoration::None);
  textSymbol->setSize(14.0);
  textSymbol->setFontStyle(FontStyle::Normal);
  textSymbol->setFontWeight(FontWeight::Normal);

  QString labelJson = "{"
                      "\"labelExpressionInfo\":{\"expression\": \"$feature.INSTALLATIONDATE\"},"
                      "\"labelPlacement\": \"esriServerLinePlacementAboveAlong\","
                      "\"useCodedValues\": true,"
                      "\"symbol\":"+ textSymbol->toJson() + "}";

  featureLayer->labelDefinitions()->clear();
  featureLayer->labelDefinitions()->append(LabelDefinition::fromJson(labelJson));
  featureLayer->setLabelsEnabled(true);
}

Display3DLabelsInScene::~Display3DLabelsInScene()
{
}

SceneQuickView* Display3DLabelsInScene::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void Display3DLabelsInScene::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
  {
    return;
  }

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}
