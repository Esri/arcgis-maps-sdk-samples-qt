// [WriteFile Name=OfflineGeocode, Category=Search]
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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISRuntime.Toolkit.Controls 2.0
// todo: implement suggestions. implement real time geocoding. adjust callout/pin position to make it look nicer.
Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    property Point calloutLocation
    property int highlightIndex: 0

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        calloutData {
            title: "Address"
            imageUrl: "qrc:/Samples/Search/OfflineGeocode/RedShinyPin.png"
            location: calloutLocation
        }

        Map {

            Basemap {
                ArcGISTiledLayer {
                    TileCache {
                        path: dataPath + "/tpk/streetmap_SD.tpk"
                    }
                }
            }
        }

        GraphicsOverlay {
            id: graphicsOverlay

            // pin graphic that will visually display geocoding results
            Graphic {
                id: pinGraphic
                geometry: calloutLocation
                visible: true

                PictureMarkerSymbol {
                    id: pictureMarker
                    url: "qrc:/Samples/Search/OfflineGeocode/blue_symbol.png"
                }

            }
        }

        Callout {
            id: callout
            calloutData: parent.calloutData
        }

        onMouseClicked: {
            suggestionRect.visible = false;
            if(locatorTask.geocodeStatus !== Enums.TaskStatusInProgress)
                locatorTask.reverseGeocodeWithParameters(mouse.mapPoint, reverseGeocodeParams);
        }

        // These two signal will be used for real time reverse geocoding.. maybe.
        onMousePressAndHold: {

        }

        onMouseReleased: {

        }
    }

    LocatorTask {
        id: locatorTask
        url: dataPath + "/Locators/SanDiego_StreetAddress.loc"

        suggestions {
            searchText: textField.text
            suggestTimerThreshold: 250
            suggestParameters: SuggestParameters {
                maxResults: 5
            }
        }

        GeocodeParameters {
            id: geocodeParams
            maxResults: 1
        }

        ReverseGeocodeParameters {
            id: reverseGeocodeParams
            maxDistance: 1000
            maxResults: 1
        }

        onSuggestionsChanged: {
            suggestionRect.visible = true;
        }

        onGeocodeStatusChanged: {
            if (geocodeStatus === Enums.TaskStatusInProgress)
            {
                busyIndicator.visible = true;
            }
            else if (geocodeStatus === Enums.TaskStatusCompleted)
            {
                busyIndicator.visible = false;
                callout.dismiss();

                if(locatorTask.geocodeResults.length > 0)
                {
                    // reset textfield
                    textField.text = "";

                    // zoom to geocode location
                    mapView.setViewpointGeometry(geocodeResults[0].extent)

                    // show callout
                    mapView.calloutData.detail = geocodeResults[0].label;
                    calloutLocation = geocodeResults[0].displayLocation;
                    callout.showCallout();

                }
            }
        }
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }

        bottomPadding: 10 * scaleFactor

        TextField {
            id: textField
            width: 300 * scaleFactor
            placeholderText: "Enter Address"

            onAccepted: {
                highlightIndex = 0
                suggestionRect.visible = false;
                if(locatorTask.geocodeStatus !== Enums.TaskStatusInProgress)
                    locatorTask.geocodeWithParameters(text, geocodeParams);
            }

            // select suggestions with key presses
            Keys.onDownPressed: {
                if ((locatorTask.suggestions.count !== 0) && (highlightIndex < locatorTask.suggestions.count))
                {
                    highlightIndex += 1;

                    if(highlightIndex === 1)
                        highlightIndex = 0;

                    text = locatorTask.suggestions.get(highlightIndex).label;
                }
            }

            Keys.onUpPressed: {
                if ((locatorTask.suggestions.count !== 0) && (highlightIndex < locatorTask.suggestions.count))
                {
                    highlightIndex -= 1;

                    if(highlightIndex < 0)
                        highlightIndex = 0;

                    text = locatorTask.suggestions.get(highlightIndex).label;
                }
            }
        }

        Rectangle {
            id: suggestionRect
            width: textField.width
            height: 4 * textField.height
            color: "lightgrey"
            visible: false

            ListView {
                id: suggestView
                model: locatorTask.suggestions
                height: parent.height
                highlightFollowsCurrentItem: true

                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                delegate: Component {

                    Rectangle {
                        width: textField.width
                        height: 20 * scaleFactor
                        color: "lightgrey"
                        border.color: "black"

                        Text {
                            font.pixelSize: 12 * scaleFactor
                            text: modelData
                            renderType: Text.NativeRendering
                            color: "black"
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                suggestView.currentIndex = index;
                                suggestionRect.visible = false;
                                if (locatorTask.geocodeStatus !== Enums.TaskStatusInProgress) {
                                    locatorTask.geocodeWithSuggestResult(locatorTask.suggestions.get(suggestView.currentIndex));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
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
