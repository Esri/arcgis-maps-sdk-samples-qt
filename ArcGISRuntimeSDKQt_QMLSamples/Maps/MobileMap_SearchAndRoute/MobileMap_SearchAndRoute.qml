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
    color: "#E0E0E0"

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mmpk/"

    property LocatorTask currentLocatorTask: null
    property RouteTask currentRouteTask: null
    property Point clickedPoint: null

    property var currentRouteParams
    property int mapPackageLoadIndex
    property int selectedMmpkIndex
    property int selectedMapInBundleIndex

    property var mobileMapList: []
    property var mobilePathsList: []
    property var routeStops: []

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        calloutData {
            title : "Address";
        }

        // animation for showing the map selection window
        transform: Translate {
            id: translate
            x: 275
            Behavior on x { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
        }

        // create a callout to display information
        Callout {
            id: callout
            calloutData: parent.calloutData
            screenOffsety: -19 * scaleFactor
        }

        // graphics overlay to visually display geocoding results
        GraphicsOverlay {
            id: stopsGraphicsOverlay

            PictureMarkerSymbol {
                id: bluePinSymbol
                height: 36 * scaleFactor
                width: 36 * scaleFactor
                url: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/bluePinSymbol.png"
                offsetY: height / 2
            }
        }

        // graphics overlay to display any routing results
        GraphicsOverlay {
            id: routeGraphicsOverlay

            SimpleRenderer {
                SimpleLineSymbol {
                    color: "#2196F3"
                    style: Enums.SimpleLineSymbolStyleSolid
                    width: 4
                }
            }
        }

        Column {
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
                color: "#E0E0E0"
                height: 50 * scaleFactor
                width: height
                border.color: "black"
                radius: 2
                opacity: 0.90
                visible: false

                Image {
                    anchors {
                        centerIn: parent
                        margins: 5 * scaleFactor
                    }
                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/routingSymbol.png"
                    height: 44 * scaleFactor
                    width: height
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        // only start routing if there are at least 2 stops
                        if (currentRouteTask.solveRouteStatus !== Enums.TaskStatusInProgress && routeStops.length >= 2) {

                            // clear any previous routing displays
                            routeGraphicsOverlay.graphics.clear();

                            // set stops
                            currentRouteParams.setStops(routeStops);

                            // solve route using generated default parameters
                            currentRouteTask.solveRoute(currentRouteParams);
                        }
                    }
                }
            }

            // clear graphics button
            Rectangle {
                id: clearButton
                color: "#E0E0E0"
                height: 50 * scaleFactor
                width: height
                border.color: "black"
                radius: 2
                opacity: 0.90
                visible: false

                Image {
                    anchors {
                        centerIn: parent
                        margins: 5 * scaleFactor
                    }
                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/discardSymbol.png"
                    height: 44 * scaleFactor
                    width: height
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

        onMouseClicked: {
            // if map selection is currently visible, dismiss
            if (mapSelectionWindow.visible === true) {
                mapSelectionWindow.visible = !mapSelectionWindow.visible
                translate.x = 0;
            }

            else if (mapSelectionWindow.visible === false && currentLocatorTask !== null) {
                clickedPoint = mouse.mapPoint;
                identifyGraphicsOverlayWithMaxResults(stopsGraphicsOverlay, mouse.x, mouse.y, 5, 2);
            }
        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus === Enums.TaskStatusCompleted){

                // if clicked on the pin graphic, display callout.
                if (identifyGraphicsOverlayResults.length > 0) {

                    // set callout's geoelement
                    mapView.calloutData.geoElement = identifyGraphicsOverlayResults[0].symbol.symbolType === Enums.SymbolTypePictureMarkerSymbol ? identifyGraphicsOverlayResults[0] : identifyGraphicsOverlayResults[1];
                    mapView.calloutData.detail = mapView.calloutData.geoElement.attributes.attributeValue("Match_addr");
                    callout.showCallout();
                }

                // otherwise, reverse geocode
                else if (currentLocatorTask.geocodeStatus !== Enums.TaskStatusInProgress){
                    currentLocatorTask.reverseGeocodeWithParameters(clickedPoint, reverseGeocodeParams);
                }
            }
        }

        onMapChanged: {
            // clear any previous graphics overlays
            stopsGraphicsOverlay.graphics.clear();
            routeGraphicsOverlay.graphics.clear();

            callout.dismiss();

            // change the locatorTask
            currentLocatorTask = mobileMapList[selectedMmpkIndex].locatorTask;

            // clear any stored stops
            routeStops = [];

            // determine if map supports routing
            if (mobileMapList[selectedMmpkIndex].maps[selectedMapInBundleIndex].transportationNetworks.length > 0) {
                currentRouteTask = ArcGISRuntimeEnvironment.createObject("RouteTask", {transportationNetworkDataset: mobileMapList[selectedMmpkIndex].maps[selectedMapInBundleIndex].transportationNetworks[0]});
                currentRouteTask.load();
            }

            else {
                currentRouteTask = null;
            }
        }
    }

    // connect signals from LocatorTask
    Connections {
        target: currentLocatorTask

        onGeocodeStatusChanged: {
            if (currentLocatorTask.geocodeStatus === Enums.TaskStatusCompleted) {
                busyIndicator.visible = false;
                if(currentLocatorTask.geocodeResults.length > 0) {

                    // create a pin graphic to display location
                    var pinGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: bluePinSymbol});
                    stopsGraphicsOverlay.graphics.append(pinGraphic);

                    // extract address from GeocodeResult and add as the an attribute of the graphic
                    pinGraphic.attributes.insertAttribute("Match_addr", currentLocatorTask.geocodeResults[0].label);

                    // add geocoded point as a stop if routing is available for current map
                    if (currentRouteTask !== null) {
                        var stop = ArcGISRuntimeEnvironment.createObject("Stop", {name: "stop", geometry: clickedPoint});
                        routeStops.push(stop);

                        if (routeStops.length > 0) {
                            clearButton.visible = true;
                            if (routeStops.length > 1) {
                                routeButton.visible = true;
                            }
                        }

                        // create a Text symbol to display stop number
                        var textSymbol = ArcGISRuntimeEnvironment.createObject("TextSymbol", {
                                                                                   color: "white",
                                                                                   text: routeStops.length,
                                                                                   size: 18 * scaleFactor,
                                                                                   offsetY: 19 * scaleFactor
                                                                               });

                        // create graphic using the text symbol
                        var labelGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: textSymbol});
                        stopsGraphicsOverlay.graphics.append(labelGraphic);
                    }
                }
            }

            else
                busyIndicator.visible = true;
        }
    }

    // connect signals from RouteTask
    Connections {
        target: currentRouteTask

        // if RouteTask loads properly, generate the default parameters
        onLoadStatusChanged: {
            if (currentRouteTask.loadStatus === Enums.LoadStatusLoaded) {
                currentRouteTask.generateDefaultParameters();
            }
        }

        // obtain default parameters
        onGenerateDefaultParametersStatusChanged: {
            if (currentRouteTask.generateDefaultParametersStatus === Enums.TaskStatusCompleted)
                currentRouteParams = currentRouteTask.generateDefaultParametersResult;
        }

        onSolveRouteStatusChanged: {
            // if route solve is successful, add a route graphic
            if(currentRouteTask.solveRouteStatus === Enums.TaskStatusCompleted) {
                var generatedRoute = currentRouteTask.solveRouteResult.routes[0];
                var routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: generatedRoute.routeGeometry});
                routeGraphicsOverlay.graphics.append(routeGraphic);
            }

            // otherwise, display error message
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

        // create window for displaying available mmpk/maps
        Rectangle {
            id: selectionMenu
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                margins: 5 * scaleFactor
            }
            visible: true
            height: parent.height
            width: 275 * scaleFactor
            color: "#E0E0E0"
            radius: 1

            Column {
                anchors {
                    top: parent.top
                    left: parent.left
                }
                width: parent.width

                Rectangle {
                    width: parent.width
                    height: 100 * scaleFactor
                    color: "#283593"

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        height: 40 * scaleFactor
                        font.pixelSize: 18 * scaleFactor
                        text: mapListView.state === "choosePackage" ? "Choose a Mobile Map Package" : "Choose a Map"
                        renderType: Text.NativeRendering
                    }
                }

                spacing: 100 * scaleFactor

                // ListModel to store names of Mobile Map Packages in data folder
                ListModel {
                    id: mobileMapPackages
                }

                // maps contained in a MobileMapPackage
                ListModel {
                    id: mapsInBundle
                    dynamicRoles: true
                }

                // displays either the avaialble MMPKs or maps
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
                        Rectangle {

                            width: 200 * scaleFactor
                            height: 50 * scaleFactor
                            color: "transparent"
                            radius: 2
                            border.color: "#283593"

                            Text {
                                anchors.centerIn: parent
                                text: name
                                renderType: Text.NativeRendering
                            }

                            // geocoding available indicator
                            Image {
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    margins: 5 * scaleFactor
                                }
                                source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/pinOutlineSymbol.png"
                                height: 20 * scaleFactor
                                width: height
                                visible: mapListView.state === "chooseMap" && mobileMapList[selectedMmpkIndex].locatorTask !== null
                            }

                            // routing available indicator
                            Image {
                                anchors {
                                    right: parent.right
                                    top: parent.top
                                    margins: 5 * scaleFactor
                                }
                                source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/routingSymbol.png"
                                height: 20 * scaleFactor
                                width: height
                                visible: mapListView.state === "chooseMap" && routing
                            }

                            MouseArea {
                                anchors.fill: parent
                                propagateComposedEvents: false
                                onClicked: {
                                    if (mapListView.state === "choosePackage") {

                                        // reset map list
                                        mapsInBundle.clear();

                                        // create the list of maps within a package
                                        for(var i = 0; i < mobileMapList[index].maps.length; i++) {
                                            var mapTitle = mobileMapList[index].maps[i].item.title;

                                            mapTitle += " " + (i + 1);

                                            // add to ListModel
                                            mapsInBundle.append({"name": mapTitle, "routing": mobileMapList[index].maps[i].transportationNetworks.length > 0});
                                        }

                                        selectedMmpkIndex = index;
                                        mapListView.state = "chooseMap";
                                    }

                                    // if mobile map package has been selected, display map selection options
                                    else if (mapListView.state === "chooseMap") {
                                        selectedMapInBundleIndex = index;
                                        mapSelectionWindow.visible = false;
                                        mapView.visible = true;

                                        // set map
                                        mapView.map = mobileMapList[selectedMmpkIndex].maps[index];

                                        // animate map back to original position
                                        translate.x = 0;

                                    }
                                }
                            }
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

                color: "#E0E0E0"
                height: 50 * scaleFactor
                width: height
                border.color: "black"
                radius: 2
                opacity: 0.90
                visible: mapListView.state !== "choosePackage"

                Image {
                    anchors {
                        centerIn: parent
                        margins: 5 * scaleFactor
                    }
                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/back.png"
                    height: 44 * scaleFactor
                    width: height
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
                    // after mmpk is loaded, add it to the list of mobile map packages
                    if (mobileMap.loadStatus === Enums.LoadStatusLoaded) {
                        var title = mobileMap.item.title;
                        mobileMapList.push(mobileMap);
                        mobileMapPackages.append({"name": title, "routing": false});
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

    // runs when app is geocoding
    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
    }

    // button to open map selection
    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 5 * scaleFactor
        }

        color: "#283593"
        height: 35 * scaleFactor
        width: height
        radius: 2
        opacity: 0.85
        visible: !mapSelectionWindow.visible

        Image {
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                margins: 1 * scaleFactor
            }
            source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/drawerSymbol.png"
            height: 33 * scaleFactor
            width: height
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // animate map to the right to display side bar
                translate.x = selectionMenu.width;
                mapSelectionWindow.visible = true;
                mapListView.state = "chooseMap";
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
