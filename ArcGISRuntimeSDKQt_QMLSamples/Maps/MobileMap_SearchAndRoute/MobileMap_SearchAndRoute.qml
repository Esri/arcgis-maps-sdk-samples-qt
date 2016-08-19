// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
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
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISRuntime.Toolkit.Controls 2.0

Rectangle {
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mmpk/"
    property var mobileMapList: []
    property var mobilePathsList: []
    property LocatorTask currentLocatorTask: null
    property RouteTask currentRouteTask: null
    property var currentRouteParams
    property int mapPackageLoadIndex: 0
    property int selectedMmpkIndex
    property int selectedMapInBundleIndex
    property Point clickedPoint: null
    property var routeStops: []

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        calloutData {
            title : "Address";
        }

        // create a callout to display information
        Callout {
            id: callout
            calloutData: parent.calloutData
        }

        GraphicsOverlay {
            id: stopsGraphicsOverlay

            SimpleMarkerSymbol {
                id: simpleSymbol
                style: Enums.SimpleFillSymbolStyleCross
                color: "blue"
                size: 12
            }
        }

        GraphicsOverlay {
            id: routeGraphicsOverlay

            SimpleRenderer {
                SimpleLineSymbol {
                    color: "cyan"
                    style: Enums.SimpleLineSymbolStyleSolid
                    width: 4
                }
            }
        }

        onMouseClicked: {
            if (currentLocatorTask !== null) {
                clickedPoint = mouse.mapPoint;
                currentLocatorTask.reverseGeocodeWithParameters(mouse.mapPoint, reverseGeocodeParams);
            }
        }

        onMapChanged: {
            stopsGraphicsOverlay.graphics.clear();
            routeGraphicsOverlay.graphics.clear();
            callout.dismiss();
            currentLocatorTask = mobileMapList[selectedMmpkIndex].locatorTask;

            if (mobileMapList[selectedMmpkIndex].maps[selectedMapInBundleIndex].transportationNetworks.length > 0) {
                currentRouteTask = ArcGISRuntimeEnvironment.createObject("RouteTask", {transportationNetworkDataset: mobileMapList[selectedMmpkIndex].maps[selectedMapInBundleIndex].transportationNetworks[0]});
                currentRouteTask.load();
            }
            else {
                currentRouteTask = null;
            }
        }
    }

    Connections {
        target: currentLocatorTask

        onGeocodeStatusChanged: {
            if (currentLocatorTask.geocodeStatus === Enums.TaskStatusCompleted) {
                busyIndicator.visible = false;
                if(currentLocatorTask.geocodeResults.length > 0) {
                    var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: simpleSymbol});
                    stopsGraphicsOverlay.graphics.append(graphic);

                    mapView.calloutData.geoElement = graphic;
                    mapView.calloutData.detail = currentLocatorTask.geocodeResults[0].label;
                    callout.showCallout();

                    if (currentRouteTask !== null) {
                        var stop = ArcGISRuntimeEnvironment.createObject("Stop", {name: "stop", geometry: clickedPoint});
                        routeStops.push(stop);

                        if (routeStops.length > 0) {
                            clearButton.visible = true;
                            if (routeStops.length > 1) {
                                routeButton.visible = true;
                            }
                        }
                    }
                }
            }

            else
                busyIndicator.visible = true;
        }
    }

    Connections {
        target: currentRouteTask

        onLoadStatusChanged: {
            if (currentRouteTask.loadStatus === Enums.LoadStatusLoaded) {
                currentRouteTask.generateDefaultParameters();
            }
        }

        onGenerateDefaultParametersStatusChanged: {
            if (currentRouteTask.generateDefaultParametersStatus === Enums.TaskStatusCompleted)
                currentRouteParams = currentRouteTask.generateDefaultParametersResult;
        }

        onSolveRouteStatusChanged: {
            if(currentRouteTask.solveRouteStatus === Enums.TaskStatusCompleted) {
                var generatedRoute = currentRouteTask.solveRouteResult.routes[0];
                var routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: generatedRoute.routeGeometry});
                routeGraphicsOverlay.graphics.append(routeGraphic);

            }

            else if (currentRouteTask.solveRouteStatus === Enums.TaskStatusErrored)
                console.log(currentRouteTask.error.message);

        }
    }

    // create reverse geocoding parameters
    ReverseGeocodeParameters {
        id: reverseGeocodeParams
        maxResults: 1
    }

    // window for selecting mobile map package and desired map
    Rectangle {
        id: mapSelectionWindow
        anchors.fill: parent
        color: "transparent"

        RadialGradient {
            anchors.fill: parent
            opacity: 0.45
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.5; color: "black" }
            }
        }

        Column {
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                margins: 10 * scaleFactor
            }

            spacing: 100 * scaleFactor

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 300 * scaleFactor
                height: 65 * scaleFactor
                color: "#f7f8fa"
                border.color: "black"
                radius: 2

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: 18 * scaleFactor
                    text: mapListView.state === "choosePackage" ? "Choose a Mobile Map Package" : "Choose a Map"
                    renderType: Text.NativeRendering
                }
            }

            // Mmpk ListModel
            ListModel {
                id: mobileMapPackages
            }

            // maps contained in Mmpk ListModel
            ListModel {
                id: mapsInBundle
            }

            ListView {
                id: mapListView
                anchors.horizontalCenter: parent.horizontalCenter
                height: 400 * scaleFactor
                width: 200 * scaleFactor
                spacing: 10 * scaleFactor
                model: mobileMapPackages
                state: "choosePackage"

                states: [
                    State {
                        name: "choosePackage"
                        PropertyChanges { target: mapListView; model: mobileMapPackages }

                    },

                    State {
                        name: "chooseMap"
                        PropertyChanges { target: mapListView; model: mapsInBundle }
                    }
                ]

                delegate: Component {

                    // idea: make this an image instead and then have separate delegates for each mode.
                    Rectangle {
                        width: 200 * scaleFactor
                        height: 50 * scaleFactor
                        color: "#f7f8fa"
                        border.color: "black"
                        radius: 2

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            renderType: Text.NativeRendering
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (mapListView.state === "choosePackage") {

                                    // reset map list
                                    mapsInBundle.clear();

                                    // create the list of maps within a package
                                    for(var i = 0; i < mobileMapList[index].maps.length; i++) {
                                        var mapTitle = mobileMapList[index].maps[i].item.title;

                                        // if no locatorTask or transportationNetworks, just display title
                                        if (mobileMapList[index].locatorTask !== null)
                                            mapTitle += " (Geocoding)";
                                        if (mobileMapList[index].maps[i].transportationNetworks.length !== 0)
                                            mapTitle += " (Routing)";

                                        // add to ListModel
                                        mapsInBundle.append({"name": mapTitle});
                                    }

                                    selectedMmpkIndex = index;
                                    mapListView.state = "chooseMap";
                                }

                                // if mobile map package has been selected, display map selection options
                                else if (mapListView.state === "chooseMap") {
                                    selectedMapInBundleIndex = index;
                                    mapSelectionWindow.visible = false;
                                    mapView.visible = true;
                                    mapView.map = mobileMapList[selectedMmpkIndex].maps[index];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    FileFolder {
        id: mmpkFolder
        url: dataPath

        // recursively load the created maps
        function loadMmpks() {
            if (mapPackageLoadIndex < mobilePathsList.length) {
                var index = mapPackageLoadIndex;
                var mobileMap = ArcGISRuntimeEnvironment.createObject("MobileMapPackage", { path: mobilePathsList[index] });
                mobileMap.load();

                mobileMap.loadStatusChanged.connect(function() {
                    if (mobileMap.loadStatus === Enums.LoadStatusLoaded) {
                        var title = mobileMap.item.title;
                        mobileMapList.push(mobileMap);
                        mobileMapPackages.append({"name": title});
                    }
                });

                mapPackageLoadIndex++;
                loadMmpks();
            }
        }

        Component.onCompleted: {
            // search through every file in the folder
            for(var i = 0; i < mmpkFolder.fileNames().length; i++) {

                // if it is an mmpk file, store its path
                if (mmpkFolder.fileInfo(mmpkFolder.fileNames()[i]).suffix === "mmpk") {
                    mobilePathsList.push(mmpkFolder.url + "/" + mmpkFolder.fileInfo(mmpkFolder.fileNames()[i]).fileName);
                }
            }

            // then create a MobileMapPackage with the stored paths
            loadMmpks();
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
    }
    Column {
        id: routeControls
        anchors {
            top: parent.top
            right: parent.right
            margins: 10 * scaleFactor
        }

        visible: currentRouteTask !== null

        spacing: 10 * scaleFactor

        // solve route button
        Rectangle {
            id: routeButton
            color: "#f7f8fa"
            height: 50 * scaleFactor
            width: height
            border.color: "black"
            radius: 2
            opacity: 0.90
            visible: false

            Text {
                anchors.centerIn: parent
                text: "ROUTE"
                renderType: Text.NativeRendering
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // only start routing if there are at least 2 stops
                    if (routeStops.length >= 2) {

                        // clear any previous routing displays
                        routeGraphicsOverlay.graphics.clear();

                        // set stops
                        currentRouteParams.setStops(routeStops);

                        currentRouteTask.solveRoute(currentRouteParams);
                    }
                }
            }
        }

        Rectangle {
            id: clearButton
            color: "#f7f8fa"
            height: 50 * scaleFactor
            width: height
            border.color: "black"
            radius: 2
            opacity: 0.90
            visible: false

            Text {
                anchors.centerIn: parent
                text: "CLEAR"
                renderType: Text.NativeRendering
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // reset graphic overlays
                    routeGraphicsOverlay.graphics.clear();
                    stopsGraphicsOverlay.graphics.clear();

                    // reset stops
                    routeStops = [];
                    currentRouteParams.clearStops();

                    // dismiss callout
                    callout.dismiss();

                    // make route controls invisible
                    clearButton.visible = false;
                    routeButton.visible = false;
                }
            }
        }
    }

    //backbutton
    Rectangle {
        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: 10 * scaleFactor
        }

        color: "#f7f8fa"
        height: 50 * scaleFactor
        width: height
        border.color: "black"
        radius: 2
        opacity: 0.90
        visible: mapListView.state !== "choosePackage"

        Text {
            anchors.centerIn: parent
            text: "BACK"
            renderType: Text.NativeRendering
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (mapSelectionWindow.visible === true) {
                    // go from "choose map" to "choose mobile map package" display
                    if (mapListView.state === "chooseMap") {
                        // go back to MobileMapPackage selection
                        mapListView.state = "choosePackage";
                    }
                }

                // if MapView is currently in foreground, pop up map selection window
                else {
                    mapSelectionWindow.visible = true;
                    mapListView.state = "chooseMap";
                }
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
