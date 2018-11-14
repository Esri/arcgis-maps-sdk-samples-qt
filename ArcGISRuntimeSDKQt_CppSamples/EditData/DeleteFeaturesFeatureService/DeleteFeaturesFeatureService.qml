// [WriteFile Name=DeleteFeaturesFeatureService, Category=EditData]
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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

DeleteFeaturesFeatureServiceSample {
    id: deleteFeaturesSample
    width: 800
    height: 600

    

    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // map callout window
        Callout {
            id: callout
            calloutData: deleteFeaturesSample.calloutData
            borderColor: "lightgrey"
            borderWidth : 1
            accessoryButtonType: "Custom"
            customImageUrl: "qrc:/Samples/EditData/DeleteFeaturesFeatureService/ic_menu_trash_light.png"
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                if (callout.visible)
                    callout.dismiss();
                deleteFeaturesSample.deleteSelectedFeature();
            }
        }
    }

    onFeatureSelected: {
        // show the callout
        callout.showCallout();
    }

    onHideWindow: {
        // hide the callout
        if (callout.visible)
            callout.dismiss();
    }
}
