// [WriteFile Name=FeatureLayerSelection, Category=Features]
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

#include "FeatureLayerSelection.h"

#include "Map.h"
#include "MapQuickView.h"
#include "FeatureLayer.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "ServiceFeatureTable.h"
#include "Viewpoint.h"
#include "Envelope.h"
#include "Feature.h"
#include "GeoElement.h"
#include <QList>
#include <QString>
#include <QUrl>
#include <QMouseEvent>

using namespace Esri::ArcGISRuntime;

FeatureLayerSelection::FeatureLayerSelection(QQuickItem* parent) :
    QQuickItem(parent),
    m_map(nullptr),
    m_mapView(nullptr),
    m_featureLayer(nullptr),
    m_featureTable(nullptr),
    m_selectedFeatureText("Click or tap to select features.")
{
}

FeatureLayerSelection::~FeatureLayerSelection()
{
}

void FeatureLayerSelection::componentComplete()
{
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");
    m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

    // Create a map using the streets basemap
    m_map = new Map(Basemap::streets(this), this);
    m_map->setInitialViewpoint(Viewpoint(Envelope(-14400186.784644607, 2328358.2989760893, -7312972.106830405, 7377998.756918708, SpatialReference::webMercator())));

    // Set map to map view
    m_mapView->setMap(m_map);

    // create the feature table
    m_featureTable = new ServiceFeatureTable(QUrl("http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"), this);
    // create the feature layer using the feature table
    m_featureLayer = new FeatureLayer(m_featureTable, this);

    // set a selection width on the feature layer
    m_featureLayer->setSelectionWidth(3);

    // add the feature layer to the map
    m_map->operationalLayers()->append(m_featureLayer);

    connectSignals();
}

void FeatureLayerSelection::connectSignals()
{
    // lambda expression for the mouse press event on the mapview... do an identify operation
    connect(m_mapView, &MapQuickView::mouseClick, [this](QMouseEvent& mouseEvent)
    {
        m_mapView->identifyLayer(m_featureLayer, mouseEvent.x(), mouseEvent.y(), 22, 1000);
    });

    // once the identify is done
    connect(m_mapView, &MapQuickView::identifyLayerCompleted, [this](QUuid, Esri::ArcGISRuntime::IdentifyLayerResult* identifyResult)
    {
        if (!identifyResult)
          return;

        // clear any existing selection
        m_featureLayer->clearSelection();

        // create a list to store the identified elements
        QList<Feature*> identifiedFeatures;
        for (int i = 0; i < identifyResult->geoElements().size(); i++)
        {
            auto element = identifyResult->geoElements().at(i);
            if (static_cast<Feature*>(element))
                // add the element to the list
                identifiedFeatures.append(static_cast<Feature*>(element));
        }

        // select the identified features
        m_featureLayer->selectFeatures(identifiedFeatures);
        // update the member with the number of selected features
        auto count = identifiedFeatures.length();
        m_selectedFeatureText = count > 1 ? QString::number(count) + " features selected." : QString::number(count) + " feature selected.";
        emit selectedFeatureTextChanged();
    });
}

QString FeatureLayerSelection::selectedFeatureText() const
{
    return m_selectedFeatureText;
}
