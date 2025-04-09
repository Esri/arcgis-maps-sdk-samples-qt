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

// sample headers
#include "SnapGeometryEditsWithRules.h"
#include "SnapSourceListModel.h"

// ArcGIS Maps SDK headers
#include "ArcGISFeature.h"
#include "ArcGISFeatureLayerInfo.h"
#include "DrawingInfo.h"
#include "Error.h"
#include "ErrorException.h"
#include "FeatureLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "GeometryEditor.h"
#include "GeometryEditorStyle.h"
#include "GeometryEditorTool.h"
#include "GeometryEditorTypes.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyLayerResult.h"
#include "LayerListModel.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "ReticleVertexTool.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "SnapRules.h"
#include "SnapSettings.h"
#include "SnapSourceSettings.h"
#include "SpatialReference.h"
#include "SubtypeFeatureLayer.h"
#include "SubtypeSublayer.h"
#include "SubtypeSublayerListModel.h"
#include "SymbolTypes.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityElement.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkListModel.h"
#include "Viewpoint.h"

// Qt headers
#include <QFile>
#include <QFuture>
#include <QStandardPaths>
#include <QTimer>
#include <QtGlobal>

using namespace Esri::ArcGISRuntime;

namespace
{
  QString defaultDataPath()
  {
    QString dataPath;

#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    dataPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    return dataPath;
  }

  // Helper load function that converts a Loadable::doneLoading signal to a QFuture representation.
  template <typename T>
  QFuture<void> load(T* loadable)
  {
    static_assert(std::is_base_of<Esri::ArcGISRuntime::Loadable, T>::value, "T must derive from Loadable");

    QPromise<void> promise;
    QFuture<void> future = promise.future();

    QObject::connect(loadable, &T::doneLoading,
    [promise = std::move(promise)](const Esri::ArcGISRuntime::Error& error) mutable {
      if (!error.isEmpty())
      {
        promise.setException(ErrorException{error});
      }
      else
      {
        promise.finish();
      }
    });

    loadable->load();
    return future;
  }

  // Helper function to create a QFuture with an error.
  template <typename T>
  QFuture<T> makeExceptionalFutureHelper(const QString& error, const QString& additionalInfo = "")
  {
    // Create an error object with the provided error message and additional info.
    Error e{error, additionalInfo};

    // Return a future that is already completed with the error.
    return QtFuture::makeExceptionalFuture<T>(ErrorException{e});
  }
}

SnapGeometryEditsWithRules::SnapGeometryEditsWithRules(QObject* parent) :
  QObject(parent),
  m_map(new Map(BasemapStyle::ArcGISStreetsNight, this)),
  m_snapSourceListModel(new SnapSourceListModel(this)),
  m_defaultGraphicsOverlayRenderer(new SimpleRenderer(new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(Qt::gray), 3,this), this)),
  m_rulesLimitSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("orange"), 3, this)),
  m_rulesPreventSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 3, this)),
  m_noneSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor("green"), 3, this))
{
  initializeMap();
}

SnapGeometryEditsWithRules::~SnapGeometryEditsWithRules() = default;

void SnapGeometryEditsWithRules::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<SnapGeometryEditsWithRules>("Esri.Samples", 1, 0, "SnapGeometryEditsWithRulesSample");
}

void SnapGeometryEditsWithRules::initializeMap()
{
  m_map->setInitialViewpoint(Viewpoint(Point(-9811055.156028448, 5131792.19502501, SpatialReference::webMercator()), 10000));
\
  // Load the geodatabase
  loadGeodatabase().then(
  [this]()
  {
    loadOperationalLayers().then(
    [this](const std::pair<SubtypeFeatureLayer*, SubtypeFeatureLayer*>& layers)
    {
      // Pass the returned pipeLayer and deviceLayer from the future to modify their visibility
      modifyOperationalLayersVisibility(layers.first, layers.second);
    });

    loadUtilityNetwork();
  });
}

QFuture<void> SnapGeometryEditsWithRules::loadGeodatabase()
{
  // Get the path to the local geodatabase for this platform (temp directory, for example).
  const QString geodatabasePath = defaultDataPath() + "/ArcGIS/Runtime/Data/geodatabase/NapervilleGasUtilities.geodatabase";

  if (!QFile::exists(geodatabasePath))
  {
    return makeExceptionalFutureHelper<void>("Geodatabase file does not exist");
  }

  m_geodatabase = new Geodatabase(geodatabasePath, this);
  return load(m_geodatabase);
}

QFuture<std::pair<SubtypeFeatureLayer*, SubtypeFeatureLayer*>> SnapGeometryEditsWithRules::loadOperationalLayers()
{
  // Get and load feature tables from the geodatabase.
  // Create subtype feature layers from the feature tables and add them to the map.
  QList<QFuture<void>> layerLoadingFutures;

  auto* pipeTable = m_geodatabase->geodatabaseFeatureTable("PipelineLine");
  if (!pipeTable)
  {
    return makeExceptionalFutureHelper<std::pair<SubtypeFeatureLayer*, SubtypeFeatureLayer*>>("Unable to create PipelineLine feature table");
  }
  auto pipeLayer = new SubtypeFeatureLayer(pipeTable, this);
  m_map->operationalLayers()->append(pipeLayer);
  layerLoadingFutures.emplace_back(load(pipeLayer));

  auto* deviceTable = m_geodatabase->geodatabaseFeatureTable("PipelineDevice");
  if (!deviceTable)
  {
    return makeExceptionalFutureHelper<std::pair<SubtypeFeatureLayer*, SubtypeFeatureLayer*>>("Unable to create PipelineDevice feature table");
  }
  auto deviceLayer = new SubtypeFeatureLayer(deviceTable, this);
  m_map->operationalLayers()->append(deviceLayer);
  layerLoadingFutures.emplace_back(load(deviceLayer));

  auto* junctionTable = m_geodatabase->geodatabaseFeatureTable("PipelineJunction");
  if (!junctionTable)
  {
    return makeExceptionalFutureHelper<std::pair<SubtypeFeatureLayer*, SubtypeFeatureLayer*>>("Unable to create PipelineJunction feature table");
  }
  auto junctionLayer = new SubtypeFeatureLayer(junctionTable, this);
  m_map->operationalLayers()->append(junctionLayer);
  layerLoadingFutures.emplace_back(load(junctionLayer));

  return QtFuture::whenAll(layerLoadingFutures.begin(), layerLoadingFutures.end()).then(
  [pipeLayer, deviceLayer](const QList<QFuture<void>>&)
  {
    return std::make_pair(pipeLayer, deviceLayer);
  });
}

QFuture<void> SnapGeometryEditsWithRules::loadUtilityNetwork()
{
  // Add the utility network to the map and load it.
  auto* utilityNetwork = m_geodatabase->utilityNetworks().constFirst();
  if (!utilityNetwork)
  {
    return makeExceptionalFutureHelper<void>("Unable to create UtilityNetwork");
  }

  m_map->utilityNetworks()->append(utilityNetwork);

  return load(utilityNetwork);
}

Esri::ArcGISRuntime::ArcGISFeature* SnapGeometryEditsWithRules::getFeatureFromResult(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResult)
{
  if (identifyResult.isEmpty())
  {
    return nullptr;
  }

  QList<IdentifyLayerResult*> sublayerResults = identifyResult.first()->sublayerResults();
  if (sublayerResults.isEmpty())
  {
    return nullptr;
  }

  QList<GeoElement*> geoElements = sublayerResults.first()->geoElements();
  if (geoElements.isEmpty())
  {
    return nullptr;
  }

  auto* feature = dynamic_cast<ArcGISFeature*>(geoElements.first());
  if (!feature)
  {
    return nullptr;
  }

  return feature;
}

void SnapGeometryEditsWithRules::onMapViewClicked(const QMouseEvent& mouseEvent)
{
  if (m_mapView->geometryEditor()->isStarted())
  {
    return;
  }

  // Identify the feature at the tapped location.
  m_mapView->identifyLayersAsync(mouseEvent.position(), 5, false).then(
  [this](const QList<IdentifyLayerResult*>& identifyResult)
  {
    ArcGISFeature* selectedFeature = getFeatureFromResult(identifyResult);

    // In this sample we only allow selection of point features.
    // If the identified feature is null or the feature is not a point feature then reset the selection and return.
    if (!selectedFeature || !selectedFeature->featureTable() || selectedFeature->featureTable()->geometryType() != GeometryType::Point)
    {
      resetSelections();
      return;
    }

    if (m_selectedFeature != nullptr && selectedFeature != m_selectedFeature)
    {
      // If a feature is already selected and the tapped feature is not the selected feature then clear the previous selection.
      dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer())->clearSelection();
    }

    // Update the selected feature.
    m_selectedFeature = selectedFeature;

    // Select the feature on the layer.
    if (auto* layer = dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer()))
    {
      layer->selectFeature(m_selectedFeature);
    }

    // Create a utility element for the selected feature using the utility network.
    std::unique_ptr<UtilityElement> utilityEle{m_mapView->map()->utilityNetworks()->first()->createElementWithArcGISFeature(m_selectedFeature)};

    // Update the UI visbility with the selected feature information.
    emit assetTypeChanged(utilityEle->assetType()->name());
    emit assetGroupChanged(utilityEle->assetGroup()->name());
    emit isElementSelectedChanged();

    setSnapSettings(utilityEle->assetType());
  });
}

void SnapGeometryEditsWithRules::setMapView(MapQuickView* mapView)
{
  if (m_mapView == mapView)
  {
      return;
  }

  m_mapView = mapView;
  m_mapView->setMap(m_map);
  m_mapView->setGeometryEditor(new GeometryEditor(this));
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
  m_mapView->geometryEditor()->setTool(new ReticleVertexTool(this));
#endif

  auto* snapSettings = new SnapSettings(this);
  snapSettings->setEnabled(true);
  snapSettings->setFeatureSnappingEnabled(true);
  m_mapView->geometryEditor()->setSnapSettings(snapSettings);

  auto* graphicsOverlay = new GraphicsOverlay(this);
  graphicsOverlay->setRenderer(m_defaultGraphicsOverlayRenderer);

  const QString graphicJson = R"({"paths":[[[-9811826.6810284462,5132074.7700250093],[-9811786.4643617794,5132440.9533583419],[-9811384.2976951133,5132354.1700250087],[-9810372.5310284477,5132360.5200250093],[-9810353.4810284469,5132066.3033583425]]],"spatialReference":{"wkid":102100,"latestWkid":3857}})";
  graphicsOverlay->graphics()->append(new Graphic(Geometry::fromJson(graphicJson), this));

  m_mapView->graphicsOverlays()->append(graphicsOverlay);

  connect(m_mapView, &MapQuickView::mouseClicked, this, &SnapGeometryEditsWithRules::onMapViewClicked);

  emit mapViewChanged();
}

MapQuickView* SnapGeometryEditsWithRules::mapView() const
{
  return m_mapView;
}

bool SnapGeometryEditsWithRules::isElementSelected() const
{
  return (m_selectedFeature != nullptr);
}

SnapSourceListModel* SnapGeometryEditsWithRules::snapSourceListModel() const
{
  return m_snapSourceListModel;
}

bool SnapGeometryEditsWithRules::geometryEditorStarted() const
{
  return (m_mapView && m_mapView->geometryEditor() && m_mapView->geometryEditor()->isStarted());
}

void SnapGeometryEditsWithRules::startEditor()
{
  if (!m_selectedFeature || !m_selectedFeature->featureTable() || !dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer()))
  {
    return;
  }

  // Get the symbol for the selected feature.
  Symbol* selectedFeatureSymbol = dynamic_cast<GeodatabaseFeatureTable*>(m_selectedFeature->featureTable())->layerInfo().drawingInfo().renderer()->symbol(m_selectedFeature);

  // Set the vertex symbol for the geometry editor tool.
  auto* geometryEditorStyle = m_mapView->geometryEditor()->tool()->style();
  geometryEditorStyle->setVertexSymbol(selectedFeatureSymbol);
  geometryEditorStyle->setFeedbackVertexSymbol(selectedFeatureSymbol);
  geometryEditorStyle->setSelectedVertexSymbol(selectedFeatureSymbol);

  // Hide the selected feature.
  dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer())->setFeatureVisible(m_selectedFeature, false);

  // Start the geometry editor.
  m_mapView->geometryEditor()->start(m_selectedFeature->geometry());

  emit geometryEditorStartedChanged();
}

void SnapGeometryEditsWithRules::stopEditing()
{
  // Stop the geometry editor and get the updated geometry.
  auto geometry = m_mapView->geometryEditor()->stop();
  emit geometryEditorStartedChanged();

  // Update the feature with the new geometry.
  if (m_selectedFeature)
  {
    m_selectedFeature->setGeometry(geometry);
    m_selectedFeature->featureTable()->updateFeatureAsync(m_selectedFeature).then(
    [this]()
    {
      // Reset the selection.
      resetSelections();
    });
  }
}

void SnapGeometryEditsWithRules::discardEdits()
{
  // Discard the current edit.
  m_mapView->geometryEditor()->stop();

  emit geometryEditorStartedChanged();

  // Reset the selection.
  resetSelections();
}

void SnapGeometryEditsWithRules::modifyOperationalLayersVisibility(SubtypeFeatureLayer* pipeLayer, SubtypeFeatureLayer* deviceLayer)
{
  // Set the visibility of the sublayers and store the default renderer for the distribution and service pipe layers.
  // In this sample we will only set a small subset of sublayers to be visible.
  auto pipelineSublayers = pipeLayer->subtypeSublayers();
  std::for_each(pipelineSublayers->begin(), pipelineSublayers->end(),
  [this](SubtypeSublayer* sublayer)
  {
    if (sublayer && sublayer->name() == "Distribution Pipe")
    {
      // Store the default renderer for the distribution pipe layer.
      m_distributionPipeLayer = sublayer;
      m_defaultDistributionRenderer = sublayer->renderer();
    }
    else if (sublayer && sublayer->name() == "Service Pipe")
    {
      // Store the default renderer for the service pipe layer.
      m_servicePipeLayer = sublayer;
      m_defaultServiceRenderer = sublayer->renderer();
    }
    else
    {
      sublayer->setVisible(false);
    }
  });

  auto deviceSublayers = deviceLayer->subtypeSublayers();
  std::for_each(deviceSublayers->begin(), deviceSublayers->end(),
  [](SubtypeSublayer* sublayer)
  {
    if (sublayer && (sublayer->name() == "Excess Flow Valve" || sublayer->name() == "Controllable Tee"))
    {
      sublayer->setVisible(true);
    }
    else
    {
      sublayer->setVisible(false);
    }
  });
}

void SnapGeometryEditsWithRules::setSnapSettings(UtilityAssetType* assetType)
{
  // Get the snap rules associated with the asset type.
  SnapRules::createAsync(m_mapView->map()->utilityNetworks()->first(), assetType).then(
  [this](SnapRules* createdRules)
  {
    // Synchronize the snap source collection with the map's operational layers using the snap rules.
    // Setting SnapSourceEnablingBehavior.SetFromRules will enable snapping for the layers and sublayers specified in the snap rules.
    m_mapView->geometryEditor()->snapSettings()->syncSourceSettings(createdRules, SnapSourceEnablingBehavior::SetFromRules);

    QList<SnapSourceSettings*> sourceSettingsForListModel;
    const auto currentSourceSettings = m_mapView->geometryEditor()->snapSettings()->sourceSettings();
    std::for_each(currentSourceSettings.begin(), currentSourceSettings.end(),
    [&sourceSettingsForListModel](SnapSourceSettings* sourceSetting)
    {
      // Enable snapping for the graphics overlay.
      if (auto graphicsOverlay = dynamic_cast<GraphicsOverlay*>(sourceSetting->source()); graphicsOverlay)
      {
        sourceSetting->setEnabled(true);
        sourceSettingsForListModel.append(sourceSetting);
      }
      else if (auto subtypeFeatureLayer = dynamic_cast<SubtypeFeatureLayer*>(sourceSetting->source()); subtypeFeatureLayer && subtypeFeatureLayer->name() == "PipelineLine")
      {
        QList<SnapSourceSettings*> childSourceSettings = sourceSetting->childSourceSettings();
        std::for_each(childSourceSettings.begin(), childSourceSettings.end(),
        [&sourceSettingsForListModel](SnapSourceSettings* sourceSetting)
        {
          if (auto* subtypeSublayer = dynamic_cast<SubtypeSublayer*>(sourceSetting->source()); subtypeSublayer &&
             (subtypeSublayer->name() == "Service Pipe" || subtypeSublayer->name() == "Distribution Pipe"))
          {
            sourceSettingsForListModel.append(sourceSetting);
          }
        });
      }
    });

    m_snapSourceListModel->setSnapSourceSettings(sourceSettingsForListModel);
    updateSnapSourceRenderers(sourceSettingsForListModel);

    emit snapSourceModelChanged();
  });
}

void SnapGeometryEditsWithRules::updateSnapSourceRenderers(const QList<Esri::ArcGISRuntime::SnapSourceSettings*>& snapSourceSettings)
{
  // Update the renderer for each snap source based on their snap rule behavior.
  std::for_each(snapSourceSettings.begin(), snapSourceSettings.end(),
  [this](const SnapSourceSettings* settings)
  {
    Esri::ArcGISRuntime::Symbol* symbol = nullptr;
    switch (settings->ruleBehavior())
    {
      case Esri::ArcGISRuntime::SnapRuleBehavior::None:
        symbol = m_noneSymbol;
        break;
      case Esri::ArcGISRuntime::SnapRuleBehavior::RulesLimitSnapping:
        symbol = m_rulesLimitSymbol;
        break;
      case Esri::ArcGISRuntime::SnapRuleBehavior::RulesPreventSnapping:
        symbol = m_rulesPreventSymbol;
        break;
    }

    if (auto graphicsOverlay = dynamic_cast<GraphicsOverlay*>(settings->source()))
    {
      graphicsOverlay->setRenderer(std::make_unique<SimpleRenderer>(symbol).get());
    }
    else if (auto subtypeSublayer = dynamic_cast<SubtypeSublayer*>(settings->source()))
    {
      subtypeSublayer->setRenderer(std::make_unique<SimpleRenderer>(symbol).get());
    }
  });
}

void SnapGeometryEditsWithRules::resetSelections()
{
  if (m_selectedFeature && m_selectedFeature->featureTable() && dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer()))
  {
    // Clear the existing selection and show the selected feature;
    dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer())->clearSelection();
    dynamic_cast<FeatureLayer*>(m_selectedFeature->featureTable()->layer())->setFeatureVisible(m_selectedFeature, true);
  }

  // Reset the selected feature.
  m_selectedFeature = nullptr;

  // Revert back to the default renderer for the distribution and service pipe layers and graphics overlay.
  m_distributionPipeLayer->setRenderer(m_defaultDistributionRenderer);
  m_servicePipeLayer->setRenderer(m_defaultServiceRenderer);
  m_mapView->graphicsOverlays()->at(0)->setRenderer(m_defaultGraphicsOverlayRenderer);

  // Clear the snap sources list.
  m_snapSourceListModel->clear();

  // Update the UI visibility.
  emit isElementSelectedChanged();
  emit snapSourceModelChanged();
}
