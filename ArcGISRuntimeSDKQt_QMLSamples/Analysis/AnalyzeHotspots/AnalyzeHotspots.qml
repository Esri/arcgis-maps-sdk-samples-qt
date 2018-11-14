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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Qt.labs.calendar 1.0
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.5
import QtQuick.Layouts 1.3

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

        Map {
            BasemapTopographic {}
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
            job.jobStatusChanged.connect(function(){
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
            var layer = result.mapImageLayer;
            layer.loadStatusChanged.connect(function() {
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
            var queryString = "(\"DATE\" > date '%1 00:00:00' AND \"DATE\" < date '%2 00:00:00')"
            queryString = queryString.arg(fromDateString).arg(toDateString);

            // Add query that contains the date range and the days of the week that are used in analysis
            var inputs = {};
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
            text: "Select date range for analysis"
            font.pixelSize: 14
        }

        Text {
            text: "From"
            font.pixelSize: 12
        }

        TextField {
            id: fromDate
            width: parent.width
            text: fromThisDate.toLocaleString(Qt.locale(), "d MMM yyyy")

            Image {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 5
                }
                source: "qrc:/Samples/Analysis/AnalyzeHotspots/calendar.png"
                width: 22
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        calendar.selectedDate = new Date(fromThisDate);
                        calendarOverlay.toOrFromDate = "from";
                        calendarOverlay.visible = true;
                    }
                }
            }
        }

        Text {
            text: "To"
            font.pixelSize: 12
        }

        TextField {
            id: toDate
            width: parent.width
            text: toThisDate.toLocaleString(Qt.locale(), "d MMM yyyy")

            Image {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 5
                }
                source: "qrc:/Samples/Analysis/AnalyzeHotspots/calendar.png"
                width: 22
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        calendar.selectedDate = new Date(fromThisDate);
                        calendarOverlay.toOrFromDate = "to";
                        calendarOverlay.visible = true;
                    }
                }
            }
        }

        Button {
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Run analysis"
            enabled: !jobInProgress && validateDates(fromThisDate, toThisDate)

            onClicked: {
                var fromString = fromThisDate.toLocaleString(Qt.locale(), "yyyy-MM-dd");
                var toString = toThisDate.toLocaleString(Qt.locale(), "yyyy-MM-dd");
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

    // Calendar overlay for selecting the date ranges
    Rectangle {
        id: calendarOverlay
        anchors.fill: parent
        color: "transparent"
        visible: false

        property string toOrFromDate

        RadialGradient {
            anchors.fill: parent
            opacity: 0.7
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }


        GridLayout {
            id: calendarGrid
            anchors.centerIn: parent
            columns: 3
            Button {
                text: "<"
                onClicked: {
                    calendar.month -= 1;
                }
                enabled: calendar.month > calendar.minDate.getMonth()
                Layout.column: 0
                Layout.row: 0
            }

            Text {
                text: calendar.title
                horizontalAlignment: Text.AlignHCenter
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: true
            }

            Button {
                text: ">"
                onClicked: {
                    calendar.month += 1;
                }
                enabled: calendar.month < (calendar.maxDate.getMonth() - 1)
                Layout.column: 2
                Layout.row: 0
            }

            DayOfWeekRow {
                locale: calendar.locale

                Layout.column: 1
                Layout.row: 1
                Layout.fillWidth: true
            }

            WeekNumberColumn {
                month: calendar.month
                year: calendar.year
                locale: calendar.locale

                Layout.fillHeight: true
                Layout.column: 0
                Layout.row: 2
            }

            MonthGrid {
                id: calendar
                month: Calendar.January
                year: 1998
                Layout.fillWidth: true
                Layout.fillHeight: true

                property date minDate: new Date(1998,0,1)
                property date maxDate: new Date(1998,4,31)
                property date selectedDate: new Date(1998,4,31)

                delegate: Text {
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    opacity: model.month !== calendar.month ? 0 : 1
                    text: model.day
                    font.bold: calendar.selectedDate.getTime() === model.date.getTime()
                }

                onClicked: {
                    if (date.getMonth() !== calendar.month)
                        return;

                    if (calendarOverlay.toOrFromDate === "from")
                        fromThisDate = date;
                    else if (calendarOverlay.toOrFromDate === "to")
                        toThisDate = date;
                    calendarOverlay.visible = false;
                }
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
    function validateDates(_fromDate, _toDate) {
        // check if each date is within the period
        if (_fromDate > calendar.maxDate || _toDate < calendar.minDate) {
            return false;
        }

        if (_fromDate < calendar.minDate || _toDate > calendar.maxDate) {
            return false;
        }

        // check that the to date is after the from date
        if (_fromDate > _toDate) {
            return false;
        }

        // check that there is at least one day in between the from and to date

        var oneDay = 86400000;
        if ((_toDate - _fromDate) < oneDay) {
            return false;
        }

        return true;
    }
}

