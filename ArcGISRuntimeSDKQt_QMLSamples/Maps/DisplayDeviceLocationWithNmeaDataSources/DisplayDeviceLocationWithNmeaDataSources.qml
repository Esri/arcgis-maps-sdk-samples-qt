// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=Maps]
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

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISNavigationNight
            }
            ViewpointCenter {
                Point {
                    x: -117.191
                    y: 34.0306
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 100000
            }
        }

        locationDisplay.dataSource: NmeaLocationDataSource {
            id: nmeaLocationDataSource
            receiverSpatialReference: SpatialReference { wkid: 4326 }
        }
        locationDisplay.autoPanMode: Enums.LocationDisplayAutoPanModeRecenter;
    }

    Button {
        id: button
        anchors.horizontalCenter: parent.horizontalCenter
        y: 5
        width: 200
        property bool sampleStarted: false
        text: sampleStarted ? "STOP" : "START"
        onClicked: {
            sampleStarted = !sampleStarted;
            if (sampleStarted) {
                timer.start();
                nmeaLocationDataSource.start();
                mapView.locationDisplay.start();
            }
            else {
                timer.stop();
                timer.iterator = 0;
                nmeaLocationDataSource.stop();
                mapView.locationDisplay.stop();
            }
        }
    }

    property var mockNmeaData: []

    FileFolder {
        id: mockNmeaDataFile
        path: ":/Samples/Maps/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea"
        Component.onCompleted: {
            readFile(path).toString().split("\n").forEach((line) => {
                                                              mockNmeaData.push(line + "\n");
                                                          });
        }
    }

    Timer {
        id: timer
        interval: 1000
        repeat: true
        property var iterator: 0
        onTriggered: {

            if (!mockNmeaData) {
                console.log("File not found sorry :(");
                stop();
                return;
            }

            while (true) {
                nmeaLocationDataSource.pushData(mockNmeaData[iterator]);
                iterator++;
                if (iterator >= mockNmeaData.length)
                    iterator = 0;
                if (mockNmeaData[iterator].startsWith("$GPGGA"))
                    break;
            }
        }
    }
}
