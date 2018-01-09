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
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property var job
    property bool jobInProgress
    property string statusText: ""    

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
        url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/911CallsHotspot/GPServer/911%20Calls%20Hotspot"

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
            margins: -10 * scaleFactor
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
            margins: 20 * scaleFactor
        }
        spacing: 5 * scaleFactor

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Select date range for analysis"
            font.pixelSize: 14 * scaleFactor
        }

        Text {
            text: "From"
            font.pixelSize: 12 * scaleFactor
        }

        TextField {
            id: fromDate
            width: parent.width
            text: "01/01/1998"

            Image {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 5 * scaleFactor
                }
                source: "qrc:/Samples/Analysis/AnalyzeHotspots/calendar.png"
                width: 22 * scaleFactor
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        calendar.selectedDate = getFormattedDateFromString(fromDate.text);
                        calendarOverlay.toOrFromDate = "from";
                        calendarOverlay.visible = true;
                    }
                }
            }
        }

        Text {
            text: "To"
            font.pixelSize: 12 * scaleFactor
        }

        TextField {
            id: toDate
            width: parent.width
            text: "01/31/1998"

            Image {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 5 * scaleFactor
                }
                source: "qrc:/Samples/Analysis/AnalyzeHotspots/calendar.png"
                width: 22 * scaleFactor
                height: width

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        calendar.selectedDate = getFormattedDateFromString(toDate.text);
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
            enabled: !jobInProgress

            onClicked: {
                // convert the strings to date objects
                var _fromDate = getFormattedDateFromString(fromDate.text);
                var _toDate = getFormattedDateFromString(toDate.text);

                // validate dates
                var success = validateDates(_fromDate, _toDate);
                if (!success) {
                    messageDialog.open();
                    return;
                }

                // format the date strings for the GP parameters
                var fromDateString = _fromDate.yyyymmdd();
                var toDateString = _toDate.yyyymmdd();

                // set up the GP Parameters
                hotspotParameters.addParameter(fromDateString, toDateString);
            }
        }

        Row {
            spacing: 15 * scaleFactor
            visible: jobInProgress

            BusyIndicator {
                anchors.verticalCenter: parent.verticalCenter
                width: 24 * scaleFactor
                height: width
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: statusText
                font.pixelSize: 14 * scaleFactor
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

        Calendar {
            id: calendar
            anchors.centerIn: parent
            minimumDate: new Date(1998,0,1)
            maximumDate: new Date(1998,4,31)

            onClicked: {
                var formattedDate = selectedDate.mmddyyyy();

                if (calendarOverlay.toOrFromDate === "from") {
                    fromDate.text = formattedDate;
                    if (!validateDates(getFormattedDateFromString(fromDate.text), getFormattedDateFromString(toDate.text)))
                        toDate.text = new Date(selectedDate.setDate(selectedDate.getDate() + 2)).mmddyyyy();
                } else if (calendarOverlay.toOrFromDate === "to") {
                    toDate.text = formattedDate;
                    if (!validateDates(getFormattedDateFromString(fromDate.text), getFormattedDateFromString(toDate.text)))
                        fromDate.text = new Date(selectedDate.setDate(selectedDate.getDate() - 2)).mmddyyyy();
                }
                calendarOverlay.visible = false;
            }
        }
    }

    MessageDialog {
        id: messageDialog
        title: "Error"
    }

    Component.onCompleted: {
        // function to convert date to formatted string MM/DD/YYYY
        Date.prototype.mmddyyyy = function() {
            var mm = this.getMonth() + 1;
            var dd = this.getDate();

            return [(mm > 9 ? "" : "0") + mm,
                    (dd > 9 ? "" : "0") + dd,
                    this.getFullYear()
                    ].join("/");
        };

        // function to convert date to formatted string YYYY-MM-DD
        Date.prototype.yyyymmdd = function() {
            var mm = this.getMonth() + 1;
            var dd = this.getDate();

            return [this.getFullYear(),
                    (mm > 9 ? "" : "0") + mm,
                    (dd > 9 ? "" : "0") + dd,
                    ].join("-");
        };
    }

    // function to get date from a string ex: 01/01/1998
    function getFormattedDateFromString(dateString) {
        var p = dateString.split("/");
        return new Date(p[2], p[0]-1, p[1]);
    }   

    // function to validate the date ranges provided
    function validateDates(_fromDate, _toDate) {
        var _minDate = calendar.minimumDate;
        var _maxDate = calendar.maximumDate;

        // check if invalid dates have been provided
        if (String(_toDate) === "Invalid Date" || String(_fromDate) === "Invalid Date") {
            messageDialog.text = "Invalid date range.";
            messageDialog.detailedText = "Please ensure that each date field contains valid dates."
            return false;
        }

        // check if each date is within the period
        if (!(_fromDate >= _minDate && _fromDate <= _maxDate) || !(_fromDate >= _minDate && _fromDate <= _maxDate)) {
            messageDialog.text = "Invalid date range.";
            messageDialog.detailedText = "Please specify dates between\nJan 01, 1998 and May 31, 1998."
            return false;
        }

        // check that the to date is after the from date
        if (_fromDate > _toDate) {
            messageDialog.text = "Invalid date range.";
            messageDialog.detailedText = "Please ensure the 'From' date precedes the 'To' date."
            return false;
        }

        // check that there is at least one day in between the from and to date
        var oneDay = (24*60*60*1000) * 2;
        if ((_toDate - _fromDate) < oneDay) {
            messageDialog.text = "Invalid date range.";
            messageDialog.detailedText = "Please ensure there is at least day in between the 'To' and 'From' dates."
            return false;
        }

        return true;
    }
}
