// [WriteFile Name=AnalyzeViewshed, Category=Analysis]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "AnalyzeViewshed.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "Viewpoint.h"
#include "SpatialReference.h"
#include "GraphicsOverlay.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "Graphic.h"
#include "GeoprocessingTask.h"
#include "GeoprocessingJob.h"
#include "GeoprocessingFeatures.h"
#include "GeoprocessingParameter.h"
#include "GeoprocessingParameters.h"
#include "GeoprocessingResult.h"
#include "FeatureCollectionTable.h"
#include "Feature.h"
#include <QMouseEvent>

using namespace Esri::ArcGISRuntime;

AnalyzeViewshed::AnalyzeViewshed(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

AnalyzeViewshed::~AnalyzeViewshed() = default;

void AnalyzeViewshed::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<AnalyzeViewshed>("Esri.Samples", 1, 0, "AnalyzeViewshedSample");
}

void AnalyzeViewshed::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);
  m_map->setInitialViewpoint(Viewpoint(Point(6.84905317262762, 45.3790902612337, SpatialReference(4326)), 100000));

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create the GeoprocessingTask
  m_viewshedTask = new GeoprocessingTask(QUrl("https://sampleserver6.arcgisonline.com/arcgis/rest/services/Elevation/ESRI_Elevation_World/GPServer/Viewshed"), this);

  // Create the Graphics Overlays
  createOverlays();

  // Connect signals
  connectSignals();
}

void AnalyzeViewshed::createOverlays()
{
  // Create the graphics overlays for the input and output
  m_inputOverlay = new GraphicsOverlay(this);
  m_inputGraphic = new Graphic(this);
  m_inputOverlay->graphics()->append(m_inputGraphic);
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 12.0, this);
  SimpleRenderer* inputRenderer = new SimpleRenderer(sms, this);
  m_inputOverlay->setRenderer(inputRenderer);
  m_mapView->graphicsOverlays()->append(m_inputOverlay);

  m_resultsOverlay = new GraphicsOverlay(this);
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor(226, 119, 40, 100), this);
  SimpleRenderer* outputRenderer = new SimpleRenderer(sfs, this);
  m_resultsOverlay->setRenderer(outputRenderer);
  m_mapView->graphicsOverlays()->append(m_resultsOverlay);
}

void AnalyzeViewshed::connectSignals()
{
  // Set up signal handler for the mouse clicked signal
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouse)
  {
    // The geoprocessing task is still executing, don't do anything else (i.e. respond to
    // more user taps) until the processing is complete.
    if (m_viewshedInProgress)
      return;

    // Indicate that the geoprocessing is running
    m_viewshedInProgress = true;

    // Clear previous viewshed geoprocessing task results
    m_resultsOverlay->graphics()->clear();
    if (m_graphicParent)
    {
      delete m_graphicParent;
      m_graphicParent = nullptr;
    }

    // Create a marker graphic where the user clicked on the map and add it to the existing graphics overlay
    Point mapPoint = m_mapView->screenToLocation(mouse.x(), mouse.y());
    if (m_inputGraphic)
      m_inputGraphic->setGeometry(mapPoint);

    // Setup the geoprocessing task
    calculateViewshed();
  });

  // Connect to the GP Task's errorOccurred signal
  connect(m_viewshedTask, &GeoprocessingTask::errorOccurred, this, [this](Error error)
  {
    emit displayErrorDialog("Geoprocessing Task failed", error.message());
  });
}

void AnalyzeViewshed::calculateViewshed()
{
  // Create a new feature collection table based upon point geometries using the current map view spatial reference
  FeatureCollectionTable* inputFeatures = new FeatureCollectionTable(QList<Field>(),
                                                                     GeometryType::Point,
                                                                     SpatialReference::webMercator(),
                                                                     this);

  // Create a new feature from the feature collection table. It will not have a coordinate location (x,y) yet
  Feature* inputFeature = inputFeatures->createFeature(this);

  // Assign a physical location to the new point feature based upon where the user clicked on the map view
  inputFeature->setGeometry(m_inputOverlay->graphics()->at(0)->geometry());

  // connect to addFeature status changed signal
  connect(inputFeatures, &FeatureCollectionTable::addFeatureCompleted, this, [this, inputFeatures](QUuid, bool success)
  {
    if (!success)
      return;

    // Create the parameters that are passed to the used geoprocessing task
    GeoprocessingParameters viewshedParameters = GeoprocessingParameters(GeoprocessingExecutionType::SynchronousExecute);

    // Request the output features to use the same SpatialReference as the map view
    viewshedParameters.setOutputSpatialReference(SpatialReference::webMercator());

    // Add an input location to the geoprocessing parameters
    QMap<QString, GeoprocessingParameter*> inputs;
    inputs["Input_Observation_Point"] = new GeoprocessingFeatures(inputFeatures, this);
    viewshedParameters.setInputs(inputs);

    // Create the job that handles the communication between the application and the geoprocessing task
    GeoprocessingJob* viewshedJob = m_viewshedTask->createJob(viewshedParameters);

    // Create signal handler for the job
    connect(viewshedJob, &GeoprocessingJob::jobStatusChanged, this, [this, viewshedJob]()
    {
      switch (viewshedJob->jobStatus())
      {
      case JobStatus::Failed:
        emit displayErrorDialog("Geoprocessing Task failed", !viewshedJob->error().isEmpty() ? viewshedJob->error().message() : "Unknown error.");
        m_viewshedInProgress = false;
        m_jobStatus = "Job failed";
        break;
      case JobStatus::Started:
        m_viewshedInProgress = true;
        m_jobStatus = "Job in progress...";
        break;
      case JobStatus::Paused:
        m_viewshedInProgress = false;
        m_jobStatus = "Job paused...";
        break;
      case JobStatus::Succeeded:
        m_viewshedInProgress = false;
        m_jobStatus = "Job succeeded";
        // handle the results
        processResults(viewshedJob->result());
        break;
      default:
        break;
      }

      // emit signals
      emit viewshedInProgressChanged();
      emit jobStatusChanged();
    });

    // start the job
    viewshedJob->start();
  });

  // Add the new feature with (x,y) location to the feature collection table
  inputFeatures->addFeature(inputFeature);
}

void AnalyzeViewshed::processResults(GeoprocessingResult *results)
{
  // Get the results from the outputs as GeoprocessingFeatures
  GeoprocessingFeatures* viewshedResultFeatures = static_cast<GeoprocessingFeatures*>(results->outputs()["Viewshed_Result"]);

  // Create the parent for the graphic
  if (!m_graphicParent)
    m_graphicParent = new QObject(this);

  // Add all the features from the result feature set as a graphics to the map
  FeatureIterator features = viewshedResultFeatures->features()->iterator();
  while (features.hasNext())
  {
    Feature* feat = features.next(this);
    Graphic* graphic = new Graphic(feat->geometry(), m_graphicParent);
    m_resultsOverlay->graphics()->append(graphic);
  }
}
