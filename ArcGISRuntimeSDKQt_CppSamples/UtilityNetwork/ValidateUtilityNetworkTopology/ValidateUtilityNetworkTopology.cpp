// [WriteFile Name=ValidateUtilityNetworkTopology, Category=UtilityNetwork]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ArcGISFeature.h"
#include "ArcGISFeatureListModel.h"
#include "ArcGISFeatureTable.h"
#include "AttributeListModel.h"
#include "AuthenticationChallenge.h"
#include "AuthenticationManager.h"
#include "CodedValueDomain.h"
#include "Credential.h"
#include "Envelope.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureEditResult.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "FeatureTableEditResult.h"
#include "Field.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyLayerResult.h"
#include "LabelDefinition.h"
#include "LabelDefinitionListModel.h"
#include "LayerListModel.h"
#include "LoadSettings.h"
#include "Map.h"
#include "MapQuickView.h"
#include "MapTypes.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "QueryParameters.h"
#include "ServiceFeatureTable.h"
#include "ServiceGeodatabase.h"
#include "ServiceTypes.h"
#include "ServiceVersionInfo.h"
#include "ServiceVersionParameters.h"
#include "SimpleLabelExpression.h"
#include "SimpleMarkerSymbol.h"
#include "SpatialReference.h"
#include "SubtypeSublayer.h"
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
#include "UtilityTraceResult.h"
#include "UtilityTraceResultListModel.h"
#include "UtilityValidationResult.h"
#include "ValidateUtilityNetworkTopology.h"
#include "Viewpoint.h"


#include <QUuid>
#include <QFuture>

using namespace Esri::ArcGISRuntime;


ValidateUtilityNetworkTopology::ValidateUtilityNetworkTopology(QObject* parent /* = nullptr */) :
  QObject(parent),
  m_cred(new Credential("editor01", "S7#i2LWmYH75", this))
{
  connect(AuthenticationManager::instance(), &AuthenticationManager::authenticationChallenge, this, [this](AuthenticationChallenge* challenge)
          {
            challenge->continueWithCredential(m_cred);
          });

  m_map = new Map(QUrl("https://sampleserver7.arcgisonline.com/portal/home/item.html?id=6e3fc6db3d0b4e6589eb4097eb3e5b9b"), this);
  connect(m_map, &Map::doneLoading, this, [this]()
  {
    m_message = QString("Loading the utility network...");
    emit messageChanged();

    m_utilityNetwork = m_map->utilityNetworks()->first();
    m_utilityNetwork->load();

    m_serviceGeodatabase = m_utilityNetwork->serviceGeodatabase();
    const QString uuid = QUuid().createUuid().toString(QUuid::WithoutBraces);

    ServiceVersionParameters* params = new ServiceVersionParameters(this);
    params->setName("ValidateNetworkTopology_"+uuid);
    params->setAccess(VersionAccess::Private);
    params->setDescription("Validate network topology with ArcGIS Runtime");

    connect(m_serviceGeodatabase, &ServiceGeodatabase::doneLoading, this, [this, params]()
    {
      if (m_serviceGeodatabase->loadStatus() == LoadStatus::Loaded)
      {
        m_serviceGeodatabase->createVersionAsync(params).then(this, [this](ServiceVersionInfo* serviceVersionInfo)
        {
          m_serviceGeodatabase->switchVersionAsync(serviceVersionInfo->name()).then(this, [this]()
          {
            getLabelDefinitions();
            m_dirtyAreaTable = m_utilityNetwork->dirtyAreaTable();
            m_map->operationalLayers()->append(new FeatureLayer(dynamic_cast<FeatureTable*>(m_dirtyAreaTable)));
            });
          }).onFailed(this, []()
          {
          });
      }
    });

    connect(m_utilityNetwork, &UtilityNetwork::loadStatusChanged, this, [this]()
    {
      if (m_utilityNetwork->loadStatus() == LoadStatus::Loaded)
      {
        setupTraceParameters();
      }
    });
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
  m_busy = true;
  emit isBusy();
  m_message = QString("Loading a webmap...");
  emit messageChanged();

  Envelope envelope(
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
  m_map->loadSettings()->setFeatureServiceSessionType(FeatureServiceSessionType(FeatureServiceSessionType::Persistent));

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  m_overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_overlay);

  emit mapViewChanged();

  connectSignals();
}

void ValidateUtilityNetworkTopology::connectSignals()
{
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    // first clear the selection
    m_deviceFeatureLayer->clearSelection();
    m_lineFeatureLayer->clearSelection();

    m_busy = true;
    emit isBusy();

    m_message = QString("Identifying feature to edit...");
    emit messageChanged();

    if (m_map->loadStatus() != LoadStatus::Loaded)
      return;

    m_clickPoint = m_mapView->screenToLocation(mouseEvent.position().x(), mouseEvent.position().y());
    m_mapView->identifyLayersAsync(mouseEvent.position(), 5.0, false).then(this, [this](const QList<IdentifyLayerResult*>& results)
    { if(!results.isEmpty())
      {
        for(IdentifyLayerResult* result : results)
        {
          if(result && (result->layerContent()->name() == m_deviceTableName || result->layerContent()->name() == m_lineTableName))
          {
            m_feature = static_cast<ArcGISFeature*>(qAsConst(result)->geoElements()[0]);
            break;
          }
        }
      }
      else
      {
        m_message = QString("No feature identified. Tap on a feature to edit.");
        emit messageChanged();

        m_busy = false;
        emit isBusy();

        return;
      }
      m_updateFieldName = m_feature->featureTable()->tableName() == m_deviceTableName ? m_deviceStatusField : m_nominalVoltageField;
      m_field = m_feature->featureTable()->field(m_updateFieldName);
      m_codedValueDomain = static_cast<CodedValueDomain>(m_field.domain());
      m_codedValues = m_codedValueDomain.codedValues();
      if (m_field.isEmpty() || m_codedValues.empty())
      {
        return;
      }

      if (m_feature->loadStatus() != LoadStatus::Loaded)
        m_feature->load();

      m_featureToEdit = m_feature;
      // Select the feature.
      if (m_updateFieldName == m_nominalVoltageField)
        m_lineFeatureLayer->selectFeature(m_featureToEdit);
      else if (m_updateFieldName == m_deviceStatusField)
        m_deviceFeatureLayer->selectFeature(m_featureToEdit);

      if(!m_choices.isEmpty())
        m_choices.clear();

      m_choices.append("");
      for (const CodedValue& codedValue : m_codedValues)
      {
        if(codedValue.code() == m_featureToEdit->attributes()->attributeValue(m_updateFieldName))

        m_choices.replace(0, codedValue.name());
        else
            m_choices.append(codedValue.name());
      }
      emit attributeValue();
      emit addChoices();

      m_message = QString("Select a new ") + m_updateFieldName;

      m_clearBtn = true;
      emit clearBtn();

      m_isUpdateVisible = true;
      emit isUpdateVisible();

      m_busy = false;
      emit isBusy();

      emit messageChanged();
      emit updateFieldName();
    });
  });
}

void ValidateUtilityNetworkTopology::onApplyEdits(QString choice)
{
  m_serviceGeodatabase = dynamic_cast<ServiceFeatureTable*>(m_featureToEdit->featureTable())->serviceGeodatabase();

  m_busy = true;
  emit isBusy();

  for (const CodedValue &codedValue : m_codedValues)
  {
    if(codedValue.name() == choice)
      m_choiceCodeValue = codedValue;
  }

  m_featureToEdit->attributes()->replaceAttribute(m_updateFieldName, m_choiceCodeValue.code());

  m_message = QString("Updating feature...");
  emit messageChanged();

  m_featureToEdit->featureTable()->updateFeatureAsync(m_featureToEdit).then(this, [this]()
  {
    m_message = QString("Applying edits...");
    emit messageChanged();
    m_serviceGeodatabase->applyEditsAsync().then(this, [this](const QList<FeatureTableEditResult*>& featureTableEditResults)
    {
      for (FeatureTableEditResult* featureTableEditResult : featureTableEditResults)
      {
        const auto results = featureTableEditResult->editResults();
        for (FeatureEditResult* featureEditResult : results)
        {
          if (!featureEditResult->error().isEmpty())
          {
            m_message = QString("Apply edits completed with error.");
            emit messageChanged();
            return;
          }
          else
          {
            m_message = QString("Apply edits completed successfully.\n"
                "Click 'Get State' to check the updated network state.");
            emit messageChanged();
          }
        }
      }
    }).onFailed(this, [this]()
    {
      m_message = QString("Apply edits failed.");
      emit messageChanged();
    });
  });

  m_deviceFeatureLayer->clearSelection();
  m_lineFeatureLayer->clearSelection();

  m_isUpdateVisible = false;
  emit isUpdateVisible();

  m_busy = false;
  emit isBusy();

  m_validateBtn = true;
  emit validateBtn();

  m_clearBtn = false;
  emit clearBtn();
}

void ValidateUtilityNetworkTopology::onClear()
{
  m_deviceFeatureLayer->clearSelection();
  m_lineFeatureLayer->clearSelection();

  m_isUpdateVisible = false;
  emit isUpdateVisible();

  m_featureToEdit = nullptr;

  m_message = QString("Selection cleared.");
  emit messageChanged();

  m_clearBtn = false;
  emit clearBtn();
}

void ValidateUtilityNetworkTopology::onGetState()
{
  m_utilityNetwork = m_mapView->map()->utilityNetworks()->first();

  if(m_utilityNetwork->definition()->capabilities()->isSupportsNetworkState())
  {
    m_busy = true;
    emit isBusy();

    m_message = QString("Getting utility network state...");
    emit messageChanged();

    m_utilityNetwork->stateAsync().then([this](const QFuture<Esri::ArcGISRuntime::UtilityNetworkState*>& state)
    {
      m_getstate = state.result();

      m_validateBtn = m_getstate->hasDirtyAreas();
      emit validateBtn();
      m_traceBtn = m_getstate->isNetworkTopologyEnabled();
      emit traceBtn();

      m_message = QString("Utility Network State:\n"
          "    Has Dirty Areas: " + QString(m_getstate && m_getstate->hasDirtyAreas() ? "true" : "false") + "\n"
          "    Has Errors: " + QString(m_getstate && m_getstate->hasErrors() ? "true" : "false") + "\n"
          "    Is Network Topology Enabled: " + QString(m_getstate && m_getstate->isNetworkTopologyEnabled() ? "true" : "false") + "\n");

      if(m_getstate->hasDirtyAreas() || m_getstate->hasErrors())
      {
        m_message.append("Click 'Validate' before trace or expect a trace error.");
      }
      else
      {
        m_message.append("Tap on a feature to edit or click 'Trace' to run a trace.");
      }

      emit messageChanged();
    });
  }
  m_busy = false;
  emit isBusy();
}

void ValidateUtilityNetworkTopology::onValidate()
{
  m_utilityNetwork = m_mapView->map()->utilityNetworks()->first();
  // Validate using the current extent
  const Envelope m_extent = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry).targetGeometry().extent();

  m_busy = true;
  emit isBusy();

  m_message = QString("Validating utility network topology...");
  emit messageChanged();
  UtilityNetworkValidationJob* job = m_utilityNetwork->validateNetworkTopology(m_extent);
  job->start();

  QObject::connect(job, &UtilityNetworkValidationJob::statusChanged, [this, job](JobStatus status)
  {
    if (status == JobStatus::Succeeded)
    {
      UtilityValidationResult* result = job->result();
      result->hasDirtyAreas();
      result->hasErrors();

      m_message = QString("Utility Validation Result:\n"
          "    Has Dirty Areas: " + QString(result->hasDirtyAreas() ? "true" : "false") + "\n"
          "    Has Errors: " + QString(result->hasErrors() ? "true" : "false") + "\n"
          "    Click 'Get State' to check the updated network state.");
      emit messageChanged();

      m_validateBtn = result->hasDirtyAreas();
      emit validateBtn();
    }
    else
    {
      m_message = QString("Validate network topology failed.");
      emit messageChanged();
    }
  });

  m_busy = false;
  emit isBusy();
}

void ValidateUtilityNetworkTopology::onTrace()
{
  m_utilityNetwork = m_mapView->map()->utilityNetworks()->first();

  m_busy = true;
  emit isBusy();

  m_message = QString("Running a downstream trace...");
  emit messageChanged();

  m_deviceFeatureLayer->clearSelection();
  m_lineFeatureLayer->clearSelection();

  m_utilityNetwork->traceAsync(m_traceParameters).then(this, [this](QList<UtilityTraceResult*>)
  {
    UtilityTraceResult* result = m_utilityNetwork->traceResult()->at(0);

    const QList<UtilityElement*> elements = static_cast<UtilityElementTraceResult*>(result)->elements(this);
    m_message = QString("Trace completed: %1 elements found.").arg(elements.length());
    emit messageChanged();

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

    m_deviceFeatureLayer->selectFeaturesAsync(deviceParams, SelectionMode::Add).then(this, [](FeatureQueryResult*)
    {
    });

    m_lineFeatureLayer->selectFeaturesAsync(lineParams, SelectionMode::Add).then(this, [](FeatureQueryResult*)
    {
    });
  }).onFailed(this, [this]()
  {
    m_message = QString("Trace failed.\n"
        "Click 'Get State' to check the updated network state.");
    emit messageChanged();
  });

  m_clearBtn = true;
  emit clearBtn();
  m_busy = false;
  emit isBusy();
}


void ValidateUtilityNetworkTopology::getLabelDefinitions()
{
  m_deviceLabelDefinition = createDeviceLabelDefinition();
  m_lineLabelDefinition = createLineLabelDefinition();
  LayerListModel* layers = m_mapView->map()->operationalLayers();

  for (Layer* layer : *layers)
  {
    if (layer->layerType() == LayerType::FeatureLayer)
    {
      m_featureLayer = qobject_cast<FeatureLayer*>(layer);

      if(m_featureLayer->name() == "Electric Distribution Device")
      {
        m_deviceFeatureLayer = m_featureLayer;
        m_deviceFeatureLayer->labelDefinitions()->append(m_deviceLabelDefinition);
        m_deviceFeatureLayer->setLabelsEnabled(true);
      }
      else if(m_featureLayer->name() == "Electric Distribution Line")
      {
        m_lineFeatureLayer = m_featureLayer;
        m_lineFeatureLayer->labelDefinitions()->append(m_lineLabelDefinition);
        m_lineFeatureLayer->setLabelsEnabled(true);
      }
    }
  }
}

void ValidateUtilityNetworkTopology::setupTraceParameters()
{
  m_utilityAssetType = m_utilityNetwork
                           ->definition()
                           ->networkSource("Electric Distribution Device")
                           ->assetGroup("Circuit Breaker")
                           ->assetType("Three Phase");

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

  m_utilityNetwork->featuresForElementsAsync(QList<UtilityElement*> {m_startingLocation}).then(this, [this](QList<ArcGISFeature*>)
  {
    // display starting location
    ArcGISFeatureListModel* elementFeaturesList = m_utilityNetwork->featuresForElementsResult();
    const Point startingLocationGeometry = geometry_cast<Point>(elementFeaturesList->first()->geometry());

    GraphicsOverlay* overlay = new GraphicsOverlay(this);
    SimpleMarkerSymbol* startingPointSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Cross, Qt::green, 25, this);
    Graphic* graphic = new Graphic(startingLocationGeometry, startingPointSymbol);
    overlay->graphics()->append(graphic);

    m_mapView->graphicsOverlays()->append(overlay);
  });

  UtilityNetworkDefinition* networkDefinition = m_utilityNetwork->definition();
  UtilityDomainNetwork* domainNetwork = networkDefinition->domainNetwork("ElectricDistribution");
  UtilityTier* tier = domainNetwork->tier("Medium Voltage Radial");

  m_traceParameters = new UtilityTraceParameters(UtilityTraceType::Downstream, {m_startingLocation}, this);

  m_traceConfiguration = tier->defaultTraceConfiguration();

  m_traceParameters->setTraceConfiguration(m_traceConfiguration);

  m_validateBtn = m_utilityNetwork->definition()->capabilities()->isSupportsValidateNetworkTopology();
  emit validateBtn();
  m_traceBtn = m_utilityNetwork->definition()->capabilities()->isSupportsTrace();
  emit traceBtn();
  m_stateBtn = m_utilityNetwork->definition()->capabilities()->isSupportsNetworkState();
  emit stateBtn();
  m_clearBtn = false;
  emit clearBtn();

  m_message = QString("Utility Network Loaded\n"
      "Tap on a feature to edit.\n"
      "Click 'Get State' to check if validating is\n"
      "necessary or if tracing is available.\n"
      "Click 'Trace' to run a trace.");
  emit messageChanged();

  m_busy = false;
  emit isBusy();
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
