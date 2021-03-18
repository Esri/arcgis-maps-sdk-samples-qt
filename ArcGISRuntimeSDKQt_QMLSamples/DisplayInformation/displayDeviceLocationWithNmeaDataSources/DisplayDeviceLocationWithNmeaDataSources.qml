// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.

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
import Esri.ArcGISRuntime 100.11
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool sampleStarted: false
    property bool useSimulatedData: true

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {

        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISNavigationNight
            }

            NmeaLocationDataSource {
                receiverSpatialReference: SpatialReference { wkid: 4326 }
            }
        }
    }

    Button {
        id: button
        anchors.horizontalCenter: parent.horizontalCenter
        y: 5
        width: 200
        text: sampleStarted ? "Stop tracking" : "Start tracking"
        onClicked: {
            sampleStarted = !sampleStarted;
            console.log(sampleStarted);
            if (sampleStarted)
                start();
            else
                reset();
        }
    }

    Rectangle {
        height: dataSourceCol.height
        width: dataSourceCol.width
        x: parent.width - (width + 15)
        y: 5
        color: "white"
        opacity: .9
        border.color: "black"
        border.width: 1

        Column {
            id: dataSourceCol
            padding: 10
            Text {
                text: "Data source"
                font.bold: true
            }
            RadioButton {
                id: simData
                text: "Simulated data"
                checked: true
                onCheckedChanged: {
                    useSimulatedData = !useSimulatedData
                    reset();
                    sampleStarted = false;
                }
            }
            RadioButton {
                id: receiverData
                text: "Receiver"
            }
        }
    }

    FileFolder {
        id: mockNmeaDataFile
        path: ":/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea"
        property var mockNmeaData: []
        Component.onCompleted: {
            console.log(fileExists(path));
            readFile(path).forEach((line) => { mockNmeaData.push(line + "\n") });

        }
    }

    function loadNmeaData(fileUrl) {
        var xhr = new XMLHttpRequest;
        xhr.open("GET", fileUrl);
        xhr.onreadystatechange = function () {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                var response = xhr.responseText;
                response.split("\n").forEach((line) => { mockNmeaData.push(line + "\n") });
                start();
            }
        }
        xhr.send();
    }

    function start() {
        if (mockNmeaData.length === 0) {
            loadNmeaData("qrc:/Samples/DisplayInformation/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea");
            return;
        }
        console.log(mockNmeaData);
        return;
    }

    function reset() {
        return;
    }
}
