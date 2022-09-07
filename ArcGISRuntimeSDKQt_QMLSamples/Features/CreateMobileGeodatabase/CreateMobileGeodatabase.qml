// [WriteFile Name=CreateMobileGeodatabase, Category=Features]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import Esri.ArcGISRuntime
import Esri.ArcGISExtras
import QtQuick.Controls

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Geodatabase mobileGeodatabase: null
    property FeatureTable featureTable: null

    property bool gdbOpen: false
    property int numberOfFeatures: 0
    property var featureListModel: []

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic
            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -77.7332
                    y: 39.3238
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 10000
            }
        }

        onMouseClicked: mouse => {
            // Add a feature to the current feature table at the given mouse click or screen tap
            if (!featureTable)
                return;

            const addFeatureToFeatureTable = () => {
                if (featureTable.addFeatureStatus !== Enums.TaskStatusCompleted)
                    return;

                featureListModel.push({"oid": feature.attributes.attributeValue("oid"), "collection_timestamp": feature.attributes.attributeValue("collection_timestamp")})
                tableView.model = featureListModel;
                featureTable.addFeatureStatusChanged.disconnect(addFeatureToFeatureTable);
                numberOfFeatures = featureTable.numberOfFeatures
            }

            const feature = featureTable.createFeatureWithAttributes({"collection_timestamp": new Date()}, mouse.mapPoint);
            featureTable.addFeatureStatusChanged.connect(addFeatureToFeatureTable);
            featureTable.addFeature(feature);
        }
    }

    Rectangle {
        id: buttonListRectangle
        width: 250
        height: buttonColumn.height + 20
        color: "#ffffff"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 10

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        Column {
            id: buttonColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 10
            anchors.leftMargin: 10
            anchors.topMargin: 10
            height: children.height
            spacing: 5

            onHeightChanged: {
                buttonListRectangle.height = height + 20
            }

            Button {
                id: createGdbButton
                text: qsTr("Create new .geodatabase")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: !gdbOpen
                onClicked: {
                    createGeodatabase();
                }
            }

            Button {
                id: viewGdbTableButton
                text: qsTr("View feature table")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: gdbOpen
                onClicked: {
                    featureTableDisplay.visible = true;
                    buttonListRectangle.visible = false;
                }
            }
            Button {
                id: clearFeaturesButton
                text: qsTr("Clear features")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: gdbOpen
                onClicked: {
                    clearTable();
                }
            }

            Button {
                id: closeGdbButton
                text: qsTr("Close .geodatabase")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: gdbOpen
                onClicked: {
                    closeGeodatabase();
                    gdbClosedNoticeRectangle.visible = true;
                    buttonListRectangle.visible = false;
                }
            }

            Text {
                text: mobileGeodatabase ? "Created new geodatabase:\n" + mobileGeodatabase.path.toString().split("/").pop() : "Geodatabase path not found"
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 12
                wrapMode: Text.WrapAnywhere
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                visible: gdbOpen
            }

            Text {
                text: "Number of features: " + numberOfFeatures + (numberOfFeatures === 0 ? "\n(Click or tap the map to add new features)" : "")
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                visible: gdbOpen
            }
        }
    }

    Rectangle {
        id: gdbClosedNoticeRectangle
        anchors.centerIn: parent
        width: parent.width * 0.75
        height: gdbInfoColumn.height + 20
        color: "white"
        border.color: "black"
        clip: true
        visible: false

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        Column {
            id: gdbInfoColumn
            anchors.centerIn: parent
            anchors.margins: 10
            spacing: 10
            width: parent.width - 20
            height: children.height

            Text {
                id: gdbNameText
                width: parent.width
                text: "Closed and saved geodatabase to the temporary path:"
                wrapMode: Text.WordWrap
            }
            TextEdit {
                id: gdbPathText
                width: parent.width
                readOnly: true
                selectByMouse: true
                text: mobileGeodatabase ? mobileGeodatabase.path : "The geodatabase returned no path property"
                wrapMode: Text.WrapAnywhere
            }
            Button {
                id: gdbInfoClose
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Ok"
                onClicked: {
                    gdbClosedNoticeRectangle.visible = false;
                    buttonListRectangle.visible = true;
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    Rectangle {
        id: featureTableDisplay
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - parent.width * 0.2
        height: parent.height - parent.height * 0.3
        color: "#80808080"
        visible: false

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        ListView {
            id: tableView
            anchors.fill: parent
            anchors.margins: 10
            ScrollBar.vertical: ScrollBar {
                active: true
            }
            clip: true

            header: Row {
                height: 40
                width: tableView.width
                Rectangle {
                    color: "grey"
                    border.color: "black"
                    width: parent.width * 0.2
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "Object ID"
                        color: "white"
                        font.bold: true
                        wrapMode: Text.WordWrap

                    }
                }
                Rectangle {
                    color: "grey"
                    border.color: "black"
                    width: parent.width * 0.8
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "Timestamp"
                        color: "white"
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }
                }
            }

            model: featureListModel
            delegate: Row {
                height: 40
                width: tableView.width
                Rectangle {
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.2
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: modelData.oid
                        wrapMode: Text.WordWrap
                    }
                }
                Rectangle {
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.8
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: modelData.collection_timestamp
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    Rectangle {
        id: buttonRectangle
        width: closeTableButton.width + 10
        height: closeTableButton.height + 10
        color: "#ffffff"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 10
        visible: featureTableDisplay.visible

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        Button {
            id: closeTableButton
            anchors.centerIn: parent

            text: "Close table view"

            onClicked: {
                featureTableDisplay.visible = false;
                buttonListRectangle.visible = true;
            }
        }
    }

    // Create a table description with which to create a feature table from the mobile geodatabase
    TableDescription {
        id: tableDescription
        tableName: "LocationHistory"
        spatialReference: SpatialReference { wkid: 4326 }
        geometryType: Enums.GeometryTypePoint
        hasAttachments: false
        hasM: false
        hasZ: false

        Component.onCompleted: {
            fieldDescriptions.append(ArcGISRuntimeEnvironment.createObject("FieldDescription", {name: "oid", fieldType: Enums.FieldTypeOID}));
            fieldDescriptions.append(ArcGISRuntimeEnvironment.createObject("FieldDescription", {name: "collection_timestamp", fieldType: Enums.FieldTypeDate}));
        }
    }

    // This ArcGISExtras component is used to ensure the given file path does not already exist
    FileFolder {
        id: fileFolder
    }

    function createGeodatabase() {
        const path = System.temporaryFolder.url + "/LocationHistory_%1.geodatabase".arg((new Date().getTime() % 1000).toString());

        // We cannot call create() with a path that already exists
        if (fileFolder.fileExists(path))
            return;

        const createGdbStatusChanged = () => {
            if (GeodatabaseUtility.createStatus === Enums.TaskStatusCompleted) {
                mobileGeodatabase = GeodatabaseUtility.createResult;

                // Create a feature table from the geodatabase once it has been created
                createTable();
                GeodatabaseUtility.createStatusChanged.disconnect(createGdbStatusChanged);
            }
        }

        GeodatabaseUtility.createStatusChanged.connect(createGdbStatusChanged);
        GeodatabaseUtility.create(path);
    }

    function createTable() {
        gdbOpen = true;
        const tableCreationStatusChanged = () => {
            if (mobileGeodatabase.createTableStatus !== Enums.TaskStatusCompleted)
                return;

            featureTable = mobileGeodatabase.createTableResult;
            const layer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: featureTable});
            map.operationalLayers.append(layer);

        }
        mobileGeodatabase.onCreateTableStatusChanged.connect(tableCreationStatusChanged);
        mobileGeodatabase.createTable(tableDescription);
    }

    function closeGeodatabase() {
        // Call Geodatabase.close() to safely share the ".geodatabase" file
        mobileGeodatabase.close();

        // Clear any information in the UI about the previous geodatabase
        gdbOpen = false;
        map.operationalLayers.clear();
        featureListModel = [];
        tableView.model = [];
        numberOfFeatures = 0;
    }

    function clearTable() {
        if (featureTable.numberOfFeatures > 0)
            deleteAllFeatures();

        featureListModel = [];
        tableView.model = [];
    }

    QueryParameters {
        id: params
        whereClause: "1=1"
    }

    function deleteAllFeatures() {
        const queryFeaturesCompleted = () => {
            if (featureTable.queryFeaturesStatus !== Enums.TaskStatusCompleted)
                return;

            const results = featureTable.queryFeaturesResult;
            const features = results.iterator.features;
            featureTable.deleteFeatures(features);
            numberOfFeatures = 0;
            featureTable.queryFeaturesStatusChanged.disconnect(queryFeaturesCompleted);
        }

        featureTable.queryFeaturesStatusChanged.connect(queryFeaturesCompleted);
        featureTable.queryFeatures(params);
    }
}
