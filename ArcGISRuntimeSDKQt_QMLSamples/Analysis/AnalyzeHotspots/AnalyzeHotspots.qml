// [WriteFile Name=AnalyzeHotspots, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls
// import Qt.labs.calendar // Calendar is not supported in Qt 6.2
import Qt5Compat.GraphicalEffects
import Esri.ArcGISRuntime
import QtQuick.Layouts

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var job
    property bool jobInProgress : false
    property string statusText: ""

    property date fromThisDate : Date.fromLocaleDateString(Qt.locale(), "98/01/01", "yy/MM/dd");
    property date toThisDate :   Date.fromLocaleDateString(Qt.locale(), "98/01/31", "yy/MM/dd")

    // Declare the MapView with a Map and Basemap
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
        }
    }

    // Declare the GeoprocessingTask and set the URL
    GeoprocessingTask {
        id: hotspotTask
        url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/911CallsHotspot/GPServer/911%20Calls%20Hotspot"

        onErrorChanged: {
            if (error)
                showErrorDialog(error);
        }

        function runTask() {
            // Create job that handles the communication between the application and the geoprocessing task
            job = hotspotTask.createJob(hotspotParameters);

            // Connect to the job's status changed signal
            job.statusChanged.connect(()=> {
                if (job.jobStatus === Enums.JobStatusFailed) {
                    showErrorDialog(job.error);
                    jobInProgress = false;
                    statusText = "Job failed.";
                } else if (job.jobStatus === Enums.JobStatusStarted) {
                    jobInProgress = true;
                    statusText = "Job in progress...";
                } else if (job.jobStatus === Enums.JobStatusPaused) {
                    jobInProgress = false;
                    statusText = "Job paused...";
                } else if (job.jobStatus === Enums.JobStatusSucceeded) {
                    // handle the results
                    processResults(job.result);
                    jobInProgress = false;
                    statusText = "Job succeeded.";
                }
            });

            // Execute analysis and wait for the results
            job.start();
        }

        // Extract the layer from the result and add to the map
        function processResults(result) {
            mapView.map.operationalLayers.clear();
            const layer = result.mapImageLayer;
            layer.loadStatusChanged.connect(()=> {
                if (layer.loadStatus === Enums.LoadStatusLoaded)
                    mapView.setViewpointGeometry(layer.fullExtent);
            });
            mapView.map.operationalLayers.append(layer);
        }

        function showErrorDialog(error) {
            messageDialog.title = "Error";
            messageDialog.text = "Executing geoprocessing failed.";
            messageDialog.detailedText = error ? error.additionalMessage : "Unknown error";
            messageDialog.open();
        }
    }

    // Declare the GeoprocessingParameters and set the execution type
    GeoprocessingParameters {
        id: hotspotParameters
        executionType: Enums.GeoprocessingExecutionTypeAsynchronousSubmit

        function addParameter(fromDateString, toDateString) {
            // create the query string
            let queryString = "(\"DATE\" > date '%1 00:00:00' AND \"DATE\" < date '%2 00:00:00')"
            queryString = queryString.arg(fromDateString).arg(toDateString);

            // Add query that contains the date range and the days of the week that are used in analysis
            const inputs = {};
            inputs["Query"] = ArcGISRuntimeEnvironment.createObject("GeoprocessingString", { value: queryString });
            hotspotParameters.inputs = inputs;

            // run the task
            hotspotTask.runTask();
        }
    }

    // Create the settings rectangle to set dates and execute the task
    Rectangle {
        anchors {
            margins: -10
            fill: settingsColumn
        }
        color: "lightgrey"
        radius: 5
        border.color: "black"
        opacity: 0.75
    }

    Column {
        id: settingsColumn
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }
        spacing: 5

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Select date range for analysis\n(d MMM yyyy)"
            font.pixelSize: 14
        }

        Text {
            text: "From"
            font.pixelSize: 12
        }

        TextField {
            id: fromDate
            width: parent.width
            text: "1 Jan 1998"
            onTextChanged: {
                fromThisDate = Date.fromLocaleString(Qt.locale(), text, "d MMM yyyy")
                validateDates();
            }
            selectByMouse: true
        }

        Text {
            text: "To"
            font.pixelSize: 12
        }

        TextField {
            id: toDate
            width: parent.width
            text: "31 Jan 1998"
            onTextChanged: {
                toThisDate = Date.fromLocaleString(Qt.locale(), text, "d MMM yyyy")
                validateDates();
            }

            selectByMouse: true
        }

        Button {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Run analysis"
            enabled: !jobInProgress && validateDates(fromThisDate, toThisDate)

            onClicked: {
                const fromString = fromThisDate.toLocaleString(Qt.locale(), "yyyy-MM-dd");
                const toString = toThisDate.toLocaleString(Qt.locale(), "yyyy-MM-dd");
                // Run the task
                hotspotParameters.addParameter(fromString, toString);
            }
        }

        Row {
            spacing: 15
            visible: jobInProgress

            BusyIndicator {
                anchors.verticalCenter: parent.verticalCenter
                width: 24
                height: width
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: statusText
                font.pixelSize: 14
            }
        }
    }

    Dialog {
        id: messageDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Error"
        property alias text : textLabel.text
        property alias detailedText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
            }
            Text {
                id: detailsLabel
            }
        }
    }

    // function to validate the date ranges provided
    function validateDates() {
        // check that the to date is after the from date
        if (fromThisDate > toThisDate) {
            return false;
        }

        // check that there is at least one day in between the from and to date
        const oneDay = 86400000;
        if ((toThisDate - fromThisDate) < oneDay) {
            return false;
        }

        return true;
    }
}

