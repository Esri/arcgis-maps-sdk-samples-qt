// [WriteFile Name=FindPlace, Category=Search]
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
import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtPositioning 5.6
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Controls 100.5
import Esri.Samples 1.0

FindPlaceSample {
    id: findPlaceSample
    clip: true
    width: 800
    height: 600

    
    property bool isSearchingLocation: false
    property bool searchByExtent: false

    onHideSuggestionView: suggestionView.visible = false
    onHideCallout: callout.dismiss();
    onShowCallout: callout.showCallout();
    onShowExtentButton: searchExtentButton.visible = true;

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        // declare a Callout
        Callout {
            id: callout
            leaderPosition: leaderPositionEnum.Automatic
            calloutData: findPlaceSample.calloutData
            accessoryButtonHidden: true
            screenOffsetY: -19
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 10
        }
        spacing: 3

        // create a text field for the POI search
        SearchBox {
            id: poiTextField
            imageUrl: "qrc:/Samples/Search/FindPlace/find.png"
            placeholderText: "Point of interest (e.g. Movie Theater)"
            onTextChanged: {
                if (text.length > 0 && suggestionView)
                    suggestionView.visible = true;
                findPlaceSample.setSuggestionsText(text);
            }
            onFocusChanged: if (focus) { poiTextHasFocus = true; }
            onAccepted: {
                geocodePOIs(poiTextField.text, locationTextField.text);
                suggestionView.visible = false;
                callout.dismiss();
            }
            onImageClicked: {
                geocodePOIs(poiTextField.text, locationTextField.text);
                suggestionView.visible = false;
                callout.dismiss();
            }
        }

        // create a text field for the location search
        SearchBox {
            id: locationTextField
            imageUrl: "qrc:/Samples/Search/FindPlace/location.png"
            placeholderText: "In proximity of"
            text: "Current Location"
            onTextChanged: {
                if (text.length > 0 && suggestionView)
                    suggestionView.visible = true;
                findPlaceSample.setSuggestionsText(text);
            }
            onFocusChanged: if (focus) { poiTextHasFocus = false; }
            onAccepted: {
                geocodePOIs(poiTextField.text, locationTextField.text);
                suggestionView.visible = false;
            }
            onImageClicked: {
                locationTextField.text = "Current Location";
                geocodePOIs(poiTextField.text, FindPlaceSample.CurrentLocation);
                suggestionView.visible = false;
            }
        }

        // create a list view for the suggestion results
        SuggestionView {
            id: suggestionView
            width: parent.width
            height: 20 * suggestionView.count
            onSuggestionClicked: {
                // change the text label
                poiTextField.focus ? poiTextField.text = label : locationTextField.text = label;

                // geocode
                geocodePOIs(poiTextField.text, locationTextField.text);

                // dismiss suggestions
                suggestionView.visible = false;
            }
        }
    }

    // create a button that allows the user to re-search the current map extent
    SearchButton {
        id: searchExtentButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }
        visible: searchByExtent
        onButtonClicked: {
            geocodePOIs(poiTextField.text, FindPlaceSample.MapExtent);
            callout.dismiss();
        }
    }
}
