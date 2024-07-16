// [WriteFile Name=ValidateUtilityNetworkTopology, Category=UtilityNetwork]
// [Legal]
// Copyright 2024 Esri.
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

#endif // PCH_BUILD
#ifdef PCH_BUILD
#include "pch.hpp"

// sample headers
#include "ValidateUtilityNetworkTopology.h"

// C++ API headers
#include "ArcGISFeature.h"
#include "ArcGISFeatureListModel.h"
#include "AttributeListModel.h"
#include "Credential.h"
#include "Envelope.h"
#include "Error.h"
#include "FeatureEditResult.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "FeatureTableEditResult.h"
#include "Field.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "LabelDefinition.h"
#include "LabelDefinitionListModel.h"
#include "LayerListModel.h"
#include "LoadSettings.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "Point.h"
#include "Portal.h"
#include "PortalItem.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "ServiceVersionInfo.h"
#include "ServiceVersionParameters.h"
#include "SimpleLabelExpression.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TaskTypes.h"
#include "TextSymbol.h"
#include "UtilityAssetGroup.h"
#include "UtilityAssetType.h"
#include "UtilityDomainNetwork.h"
#include "UtilityElement.h"
#include "UtilityElementTraceResult.h"
#include "UtilityNetwork.h"
#include "UtilityNetworkCapabilities.h"
#include "UtilityNetworkDefinition.h"
#include "UtilityNetworkListModel.h"
#include "UtilityNetworkSource.h"
#include "UtilityNetworkState.h"
#include "UtilityNetworkTypes.h"
#include "UtilityNetworkValidationJob.h"
#include "UtilityTerminal.h"
#include "UtilityTerminalConfiguration.h"
#include "UtilityTier.h"
#include "UtilityTraceParameters.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityValidationResult.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

ValidateUtilityNetworkTopology::ValidateUtilityNetworkTopology(QObject* parent /* = nullptr */) :
  QObject(parent)
{
  Credential* cred = new Credential("editor01", "S7#i2LWmYH75", this);

  Portal* portal = new Portal(QUrl("https://sampleserver7.arcgisonline.com/portal/sharing/rest"), cred, this);

  PortalItem* portalItem = new PortalItem(portal, "6e3fc6db3d0b4e6589eb4097eb3e5b9b", this);

  m_map = new Map(portalItem, this);

  connect(m_map, &Map::doneLoading, this, [this]()
  {
    onMapLoaded();
  });
}

ValidateUtilityNetworkTopology::~ValidateUtilityNetworkTopology() = default;

void ValidateUtilityNetworkTopology::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ValidateUtilityNetworkTopology>("Esri.Samples", 1, 0, "ValidateUtilityNetworkTopologySample");
}

MapQuickView* ValidateUtilityNetworkTopology::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ValidateUtilityNetworkTopology::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_progressBarVisibility = true;
  emit progressBarVisibilityChanged();
  updateMessage("Loading a webmap...");

  const Envelope envelope(
      -9815489.0660101417,
      5128463.4221229386,
      -9814625.2768726498,
      5128968.4911854975,
      SpatialReference::webMercator()
      );

  Viewpoint initialViewpoint(envelope);

  m_map->setInitialViewpoint(initialViewpoint);

  // Load in persistent session mode (workaround for server caching issue)
  // https://support.esri.com/en-us/bug/asynchronous-validate-request-for-utility-network-servi-bug-000160443
  m_map->loadSettings()->setFeatureServiceSessionType(FeatureServiceSessionType::Persistent);

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);

  emit mapViewChanged();

  connectSignals();
}

void ValidateUtilityNetworkTopology::onMapLoaded()
{
  // Load and switch utility network version
  updateMessage("Loading the utility network...");

  m_utilityNetwork = m_map->utilityNetworks()->first();

  const QString randomVersionUuid = QUuid().createUuid().toString(QUuid::WithoutBraces);

  // Restrict editing and tracing on a random branch
  ServiceVersionParameters* params = new ServiceVersionParameters(this);
  params->setName("ValidateNetworkTopology_" + randomVersionUuid);
  params->setAccess(VersionAccess::Private);
  params->setDescription("Validate network topology with ArcGIS Runtime");

  connect(m_utilityNetwork, &UtilityNetwork::doneLoading, this, [this]()
  {
    setupTraceParameters();
  });

  m_utilityNetwork->load();
  m_serviceGeodatabase = m_utilityNetwork->serviceGeodatabase();

  connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, [this, params](const Esri::ArcGISRuntime::Error &loadError)
  {
    onServiceGeodatabaseLoaded(params, loadError);
  });

  m_serviceGeodatabase->load();
}

void ValidateUtilityNetworkTopology::onServiceGeodatabaseLoaded(ServiceVersionParameters* params, const Error& loadError)
{
  if (!loadError.isEmpty())
    return;

  m_serviceGeodatabase->createVersionAsync(params).then(this, [this](ServiceVersionInfo* serviceVersionInfo)
  {
    m_serviceGeodatabase->switchVersionAsync(serviceVersionInfo->name()).then(this, [this]()
    {
      displayLabelDefinitions();

      // Visualize dirty area by adding to the map
      ArcGISFeatureTable* dirtyAreaTable = m_utilityNetwork->dirtyAreaTable();
      m_map->operationalLayers()->append(new FeatureLayer(dynamic_cast<FeatureTable*>(dirtyAreaTable), this));
    });
    serviceVersionInfo->deleteLater();
  });
}

void ValidateUtilityNetworkTopology::connectSignals()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    if (m_deviceFeatureLayer && m_lineFeatureLayer)
    {
      // first clear the selection
      m_deviceFeatureLayer->clearSelection();
      m_lineFeatureLayer->clearSelection();

      m_progressBarVisibility = true;
      emit progressBarVisibilityChanged();

      updateMessage("Identifying feature to edit...");

      if (m_map->loadStatus() != LoadStatus::Loaded)
        return;

      // Perform an identify to determine if a user tapped on a feature.
      m_mapView->identifyLayersAsync(mouseEvent.position(), 5.0, false).then(this, [this](const QList<IdentifyLayerResult*>& results)
      {
        onIdentifyLayersAsyncCompleted(results);
      });
    }
  });
}

void ValidateUtilityNetworkTopology::onIdentifyLayersAsyncCompleted(const QList<IdentifyLayerResult*>& results)
{
  if (!results.isEmpty())
  {
    for (IdentifyLayerResult* result : results)
    {
      if (result && (result->layerContent()->name() == "Electric Distribution Device" || result->layerContent()->name() == "Electric Distribution Line"))
      {
        m_feature = static_cast<ArcGISFeature*>(std::as_const(result)->geoElements().first());
        break;
      }
    }
  }
  else
  {
    updateMessage("No feature identified. Tap on a feature to edit.");

    m_progressBarVisibility = false;
    emit progressBarVisibilityChanged();

    return;
  }
  m_fieldName = m_feature->featureTable()->tableName() == "Electric Distribution Device" ? "devicestatus" : "nominalvoltage";
  Field field = m_feature->featureTable()->field(m_fieldName);
  CodedValueDomain codedValueDomain = static_cast<CodedValueDomain>(field.domain());
  m_codedValues = codedValueDomain.codedValues();
  if (field.isEmpty() || m_codedValues.empty())
  {
    return;
  }

  if (m_feature->loadStatus() != LoadStatus::Loaded)
    m_feature->load();

  m_featureToEdit = m_feature;

  // Select the feature.
  if (m_fieldName == "nominalvoltage")
    m_lineFeatureLayer->selectFeature(m_featureToEdit);
  else if (m_fieldName == "devicestatus")
    m_deviceFeatureLayer->selectFeature(m_featureToEdit);

  m_choices.clear();

  m_choices.append("");
  for (const CodedValue& codedValue : m_codedValues)
  {
    if (codedValue.code() == m_featureToEdit->attributes()->attributeValue(m_fieldName))
      m_choices.replace(0, codedValue.name());
    else
      m_choices.append(codedValue.name());
  }
  emit choicesChanged();

  updateMessage("Select a new " + m_fieldName);

  m_clearButtonAvailability = true;
  emit clearButtonAvailabilityChanged();

  m_updateWindowVisibility = true;
  emit updateWindowVisibilityChanged();

  m_progressBarVisibility = false;
  emit progressBarVisibilityChanged();

  emit fieldNameChanged();
}

void ValidateUtilityNetworkTopology::onApplyEdits(const QString& choice)
{
  CodedValue choiceCodeValue;

  m_serviceGeodatabase = static_cast<ServiceFeatureTable*>(m_featureToEdit->featureTable())->serviceGeodatabase();

  m_progressBarVisibility = true;
  emit progressBarVisibilityChanged();

  for (const CodedValue &codedValue : m_codedValues)
  {
    if (codedValue.name() == choice)
      choiceCodeValue = codedValue;
  }

  m_featureToEdit->attributes()->replaceAttribute(m_fieldName, choiceCodeValue.code());

  updateMessage("Updating feature...");

  m_featureToEdit->featureTable()->updateFeatureAsync(m_featureToEdit).then(this, [this]()
  {
    updateMessage("Applying edits...");
    m_serviceGeodatabase->applyEditsAsync().then(this, [this](const QList<FeatureTableEditResult*>& featureTableEditResults)
    {
      for (FeatureTableEditResult* featureTableEditResult : featureTableEditResults)
      {
        const auto results = featureTableEditResult->editResults();
        for (FeatureEditResult* featureEditResult : results)
        {
          if (!featureEditResult->error().isEmpty())
          {
            updateMessage("Apply edits completed with error.");
            return;
          }
          else
          {
            updateMessage("Apply edits completed successfully.\n"
                "Click 'Get State' to check the updated network state.");
          }
        }
      }

      m_progressBarVisibility = false;
      emit progressBarVisibilityChanged();
    }).onFailed(this, [this]()
    {
      updateMessage("Apply edits failed.");

      m_progressBarVisibility = false;
      emit progressBarVisibilityChanged();
    });
  });

  m_deviceFeatureLayer->clearSelection();
  m_lineFeatureLayer->clearSelection();

  m_updateWindowVisibility = false;
  emit updateWindowVisibilityChanged();

  m_validateButtonAvailability = true;
  emit validateButtonAvailabilityChanged();

  m_clearButtonAvailability = false;
  emit clearButtonAvailabilityChanged();
}

void ValidateUtilityNetworkTopology::onClear()
{
  // Clear the selection.
  m_deviceFeatureLayer->clearSelection();
  m_lineFeatureLayer->clearSelection();

  m_updateWindowVisibility = false;
  emit updateWindowVisibilityChanged();

  m_featureToEdit = nullptr;

  updateMessage("Selection cleared.");

  m_clearButtonAvailability = false;
  emit clearButtonAvailabilityChanged();
}

void ValidateUtilityNetworkTopology::onGetState()
{
  m_utilityNetwork = m_mapView ? (m_mapView->map() ? m_mapView->map()->utilityNetworks()->first() : nullptr) : nullptr;

  if (m_utilityNetwork && m_utilityNetwork->definition()->capabilities()->isSupportsNetworkState())
  {
    m_progressBarVisibility = true;
    emit progressBarVisibilityChanged();

    updateMessage("Getting utility network state...");

    m_utilityNetwork->stateAsync().then([this](const QFuture<Esri::ArcGISRuntime::UtilityNetworkState*>& state)
    {
      m_utilityNetworkstate = state.result();

      // Validate if dirty areas or errors exist
      m_validateButtonAvailability = m_utilityNetworkstate->hasDirtyAreas();
      emit validateButtonAvailabilityChanged();

      // Trace if network topology is enabled
      m_traceButtonAvailability = m_utilityNetworkstate->isNetworkTopologyEnabled();
      emit traceButtonAvailabilityChanged();

      m_message = QString("Utility Network State:\n"
          "    Has Dirty Areas: " + QString(m_utilityNetworkstate && m_utilityNetworkstate->hasDirtyAreas() ? "true" : "false") + "\n"
          "    Has Errors: " + QString(m_utilityNetworkstate && m_utilityNetworkstate->hasErrors() ? "true" : "false") + "\n"
          "    Is Network Topology Enabled: " + QString(m_utilityNetworkstate && m_utilityNetworkstate->isNetworkTopologyEnabled() ? "true" : "false") + "\n");

      if (m_utilityNetworkstate->hasDirtyAreas() || m_utilityNetworkstate->hasErrors())
      {
        m_message.append("Click 'Validate' before trace or expect a trace error.");
      }
      else
      {
        m_message.append("Tap on a feature to edit or click 'Trace' to run a trace.");
      }

      updateMessage(m_message);

      m_utilityNetworkstate->deleteLater();

      m_progressBarVisibility = false;
      emit progressBarVisibilityChanged();
    });
  }
}

void ValidateUtilityNetworkTopology::onValidate()
{  
  m_utilityNetwork = m_mapView ? (m_mapView->map() ? m_mapView->map()->utilityNetworks()->first() : nullptr) : nullptr;

  if (m_utilityNetwork)
  {
    // Validate using the current extent
    const Envelope extent = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent();

    m_progressBarVisibility = true;
    emit progressBarVisibilityChanged();

    updateMessage("Validating utility network topology...");

    // Get the validation result
    UtilityNetworkValidationJob* job = m_utilityNetwork->validateNetworkTopology(extent);

    connect(job, &UtilityNetworkValidationJob::statusChanged, this, [this, job](JobStatus status)
    {
      if (status == JobStatus::Succeeded)
      {
        UtilityValidationResult* result = job->result();
        result->hasDirtyAreas();
        result->hasErrors();

        updateMessage("Utility Validation Result:\n"
            "    Has Dirty Areas: " + QString(result->hasDirtyAreas() ? "true" : "false") + "\n"
            "    Has Errors: " + QString(result->hasErrors() ? "true" : "false") + "\n"
            "    Click 'Get State' to check the updated network state.");

        m_validateButtonAvailability = result->hasDirtyAreas();
        emit validateButtonAvailabilityChanged();

        m_progressBarVisibility = false;
        emit progressBarVisibilityChanged();

        job->deleteLater();
      }
      else if (status == JobStatus::Failed)
      {
        updateMessage("Validate network topology failed.");

        m_progressBarVisibility = false;
        emit progressBarVisibilityChanged();

        job->deleteLater();
      }
      else if (status == JobStatus::Canceling)
      {
        updateMessage("Validate network topology cancelled.");

        m_progressBarVisibility = false;
        emit progressBarVisibilityChanged();

        job->deleteLater();
      }
    });
    job->start();
  }
}

void ValidateUtilityNetworkTopology::onTrace()
{  
  m_utilityNetwork = m_mapView ? (m_mapView->map() ? m_mapView->map()->utilityNetworks()->first() : nullptr) : nullptr;

  if (m_utilityNetwork)
  {
    updateMessage("Running a downstream trace...");

    m_progressBarVisibility = true;
    emit progressBarVisibilityChanged();

    // Clear previous selection from the layers.
    m_deviceFeatureLayer->clearSelection();
    m_lineFeatureLayer->clearSelection();

    //  Get the trace result from the utility network.
    m_utilityNetwork->traceAsync(m_traceParameters).then(this, [this](QList<UtilityTraceResult*>)
    {
      m_progressBarVisibility = false;
      emit progressBarVisibilityChanged();

      UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);

      const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);
      updateMessage(QString("Trace completed: %1 elements found.").arg(elements.length()));

      QueryParameters deviceParams;
      QueryParameters lineParams;
      QList<qint64> deviceObjIds;
      QList<qint64> lineObjIds;

      for (UtilityElement* item : elements)
      {
        if (item->networkSource()->name() == "Electric Distribution Device")
          deviceObjIds.append(item->objectId());
        else if (item->networkSource()->name() == "Electric Distribution Line")
          lineObjIds.append(item->objectId());
      }

      deviceParams.setObjectIds(deviceObjIds);
      lineParams.setObjectIds(lineObjIds);

      m_deviceFeatureLayer->selectFeaturesAsync(deviceParams, SelectionMode::Add).then(this, [](FeatureQueryResult* rawResult)
      {
        std::unique_ptr<FeatureQueryResult> {rawResult};
      });
      m_lineFeatureLayer->selectFeaturesAsync(lineParams, SelectionMode::Add).then(this, [this](FeatureQueryResult* rawResult)
      {
        std::unique_ptr<FeatureQueryResult> {rawResult};

        m_clearButtonAvailability = true;
        emit clearButtonAvailabilityChanged();
      });
      result->deleteLater();
    }).onFailed(this, [this]()
    {
      updateMessage("Trace failed.\n"
          "Click 'Get State' to check the updated network state.");

      m_progressBarVisibility = false;
      emit progressBarVisibilityChanged();
    });
  }
}

void ValidateUtilityNetworkTopology::updateMessage(QString message)
{
  m_message = message;
  emit messageChanged();
}

void ValidateUtilityNetworkTopology::displayLabelDefinitions()
{
  FeatureLayer* featureLayer;
  // Visualize attribute editing using labels
  LabelDefinition* m_deviceLabelDefinition = createDeviceLabelDefinition();
  LabelDefinition* m_lineLabelDefinition = createLineLabelDefinition();
  LayerListModel* layers = m_mapView->map()->operationalLayers();

  for (Layer* layer : *layers)
  {
    if (layer->layerType() == LayerType::FeatureLayer)
    {
      featureLayer = qobject_cast<FeatureLayer*>(layer);

      if (featureLayer->name() == "Electric Distribution Device")
      {
        m_deviceFeatureLayer = featureLayer;
        m_deviceFeatureLayer->labelDefinitions()->append(m_deviceLabelDefinition);
        m_deviceFeatureLayer->setLabelsEnabled(true);
      }
      else if (featureLayer->name() == "Electric Distribution Line")
      {
        m_lineFeatureLayer = featureLayer;
        m_lineFeatureLayer->labelDefinitions()->append(m_lineLabelDefinition);
        m_lineFeatureLayer->setLabelsEnabled(true);
      }
    }
  }
}

void ValidateUtilityNetworkTopology::setupTraceParameters()
{
  // Trace with a subnetwork controller as default starting location
  m_utilityAssetType = m_utilityNetwork ? m_utilityNetwork->definition()->
                            networkSource("Electric Distribution Device")->
                            assetGroup("Circuit Breaker")->
                            assetType("Three Phase") :
                            nullptr;

  if (m_utilityAssetType)
  {
    const QList<UtilityTerminal*> utilityTerminals = m_utilityAssetType->terminalConfiguration()->terminals();

    UtilityTerminal* loadTerminal = nullptr;

    for (UtilityTerminal* utilityTerminal : utilityTerminals)
    {
      // Set the terminal for the location. (For our case, use the "Load" terminal.)
      if (utilityTerminal->name() == "Load")
      {
        loadTerminal = utilityTerminal;
        break;
      }
    }
    m_startingLocation = m_utilityNetwork->createElementWithAssetType(m_utilityAssetType, QUuid("{1CAF7740-0BF4-4113-8DB2-654E18800028}"), loadTerminal, this);
  }
  else
    return;

  // Display starting location as graphic
  m_utilityNetwork->featuresForElementsAsync(QList<UtilityElement*> {m_startingLocation}).then(this, [this](QList<ArcGISFeature*>)
  {
    ArcGISFeatureListModel* elementFeaturesList = m_utilityNetwork->featuresForElementsResult();
    const Point startingLocationGeometry = geometry_cast<Point>(elementFeaturesList->first()->geometry());

    GraphicsOverlay* overlay = new GraphicsOverlay(this);
    SimpleMarkerSymbol* startingPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::green, 25, this);
    Graphic* graphic = new Graphic(startingLocationGeometry, startingPointSymbol, this);
    overlay->graphics()->append(graphic);

    m_mapView->graphicsOverlays()->append(overlay);
  });

  UtilityNetworkDefinition* networkDefinition = m_utilityNetwork->definition();
  UtilityDomainNetwork* domainNetwork = networkDefinition->domainNetwork("ElectricDistribution");
  UtilityTier* tier = domainNetwork->tier("Medium Voltage Radial");

  m_traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, {m_startingLocation}, this);

  UtilityTraceConfiguration* m_traceConfiguration = tier->defaultTraceConfiguration();

  m_traceParameters->setTraceConfiguration(m_traceConfiguration);

  m_validateButtonAvailability = m_utilityNetwork->definition()->capabilities()->isSupportsValidateNetworkTopology();
  emit validateButtonAvailabilityChanged();
  m_traceButtonAvailability = m_utilityNetwork->definition()->capabilities()->isSupportsTrace();
  emit traceButtonAvailabilityChanged();
  m_stateButtonAvailability = m_utilityNetwork->definition()->capabilities()->isSupportsNetworkState();
  emit stateButtonAvailabilityChanged();
  m_clearButtonAvailability = false;
  emit clearButtonAvailabilityChanged();

  updateMessage("Utility Network Loaded\n"
      "Tap on a feature to edit.\n"
      "Click 'Get State' to check if validating is\n"
      "necessary or if tracing is available.\n"
      "Click 'Trace' to run a trace.");

  m_progressBarVisibility = false;
  emit progressBarVisibilityChanged();
}

LabelDefinition* ValidateUtilityNetworkTopology::createDeviceLabelDefinition()
{
  SimpleLabelExpression* labelExpression = new SimpleLabelExpression("[devicestatus]", this);

  TextSymbol* textSymbol = new TextSymbol(this);
  textSymbol->setSize(12);
  textSymbol->setColor(Qt::blue);
  textSymbol->setHaloColor(Qt::white);
  textSymbol->setHaloWidth(2);

  LabelDefinition* deviceLabelDefinition = new LabelDefinition(labelExpression, textSymbol, this);
  deviceLabelDefinition->setUseCodedValues(true);
  return deviceLabelDefinition;
}

LabelDefinition* ValidateUtilityNetworkTopology::createLineLabelDefinition()
{
  SimpleLabelExpression* labelExpression = new SimpleLabelExpression("[nominalvoltage]", this);

  TextSymbol* textSymbol = new TextSymbol(this);
  textSymbol->setSize(12);
  textSymbol->setColor(Qt::red);
  textSymbol->setHaloColor(Qt::white);
  textSymbol->setHaloWidth(2);

  LabelDefinition* lineLabelDefinition = new LabelDefinition(labelExpression, textSymbol, this);
  lineLabelDefinition->setUseCodedValues(true);
  return lineLabelDefinition;
}
