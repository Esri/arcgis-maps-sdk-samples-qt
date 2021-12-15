// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=Maps]
// [Legal]
// Copyright 2021 Esri.

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
import Esri.ArcGISRuntime 100.14
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool nmeaSimulationActive: false
    property var mockNmeaData: []

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISNavigation
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

        // Create the NemaLocationDataSource to push NMEA sentences to
        locationDisplay.dataSource: NmeaLocationDataSource {
            id: nmeaLocationDataSource
            receiverSpatialReference: SpatialReference { wkid: 4326 }
        }
        locationDisplay.autoPanMode: Enums.LocationDisplayAutoPanModeRecenter
    }

    Button {
        id: button
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 25
        }
        width: 200
        text: nmeaSimulationActive ? "RESET" : "START"
        onClicked: {
            nmeaSimulationActive = !nmeaSimulationActive;
            if (nmeaSimulationActive)
                startSimulation();
            else
                endSimulation();
        }
    }

    FileFolder {
        id: mockNmeaDataFile
        path: ":/Samples/Maps/DisplayDeviceLocationWithNmeaDataSources/redlands.nmea"
        Component.onCompleted: {
            // Load simulated NMEA sentences to display for sample
            readFile(path)
            .toString()
            .split("\n")
            .forEach(
                 (line) => {
                     // In this simulated data stream, blocks of NMEA sentences start with $GPGGA (which provides the device's position)
                     if (line.startsWith("$GPGGA"))
                         mockNmeaData.push(line + "\n");

                     // Additional sentences that provide information such as direction and velocity follow and are separated by line breaks
                     else
                         mockNmeaData[mockNmeaData.length-1] += (line + "\n");
                 });
        }
    }

    // This timer is used to simulate NMEA sentences being received at regular intervals
    Timer {
        id: timer
        interval: 1000
        repeat: true
        property var count: 0
        onTriggered: {
            try {
                // In a non-simulated scenario, incoming NMEA sentences from a serial port or bluetooth device would be pushed to the location data source in real time
                // NMEA sentences can be pushed individually or in multiple lines separated by line breaks
                // Sentences pass information such as direction, velocity, and location and are grouped together to provide detailed information about a user's position
                nmeaLocationDataSource.pushData(mockNmeaData[count]);
            } catch(err) {
                console.log("Unable to push: " + mockNmeaData[count]);
                console.log(err);
            }

            count++;
            if (count >= mockNmeaData.length)
                count = 0;
        }
    }

    function startSimulation() {
        // Enable receiving NMEA location data from external device
        nmeaLocationDataSource.start();

        // Display the user's location
        mapView.locationDisplay.start();

        // Begin simulated data stream
        timer.count = 0;
        timer.start();
    }

    function endSimulation() {
        // End simulated data stream
        timer.stop();

        // Stop displaying the user's location
        mapView.locationDisplay.stop();

        // Stop receiving location data
        nmeaLocationDataSource.stop();
    }
}
