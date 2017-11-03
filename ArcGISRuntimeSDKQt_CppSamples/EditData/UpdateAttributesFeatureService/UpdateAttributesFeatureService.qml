// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.2

UpdateAttributesFeatureServiceSample {
    id: updateFeaturesSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
    }

    onFeatureSelected: {
        // show the callout
        callout.x = updateFeaturesSample.screenX;
        callout.y = updateFeaturesSample.screenY;
        callout.visible = true;

        // set the combo box's default value
        damageComboBox.currentIndex = featAttributes.indexOf(updateFeaturesSample.featureType);
    }

    onHideWindow: {
        // hide the callout
        callout.visible = false;
        // hide the update window
        updateWindow.visible = false;
    }

    // map callout window
       Rectangle {
           id: callout
           width: row.width + (10 * scaleFactor) // add 10 for padding
           height: 40 * scaleFactor
           radius: 5
           border {
               color: "lightgrey"
               width: .5
           }
           visible: false

           MouseArea {
               anchors.fill: parent
               onClicked: mouse.accepted = true
           }

           Row {
               id: row
               anchors {
                   verticalCenter: parent.verticalCenter
                   left: parent.left
                   margins: 5 * scaleFactor
               }
               spacing: 10

               Text {
                   text: updateFeaturesSample.featureType
                   font.pixelSize: 18 * scaleFactor
               }

               Rectangle {
                   radius: 100
                   width: 22 * scaleFactor
                   height: width
                   color: "transparent"
                   border.color: "blue"
                   antialiasing: true

                   Text {
                       anchors.centerIn: parent
                       text: "i"
                       font.pixelSize: 18 * scaleFactor
                       color: "blue"
                   }

                   // create a mouse area over the (i) text to open the update window
                   MouseArea {
                       anchors.fill: parent
                       onClicked: {
                           updateWindow.visible = true;
                       }
                   }
               }
           }
       }

       // Update Window
       Rectangle {
           id: updateWindow
           anchors.centerIn: parent
           width: 200 * scaleFactor
           height: 110 * scaleFactor
           radius: 10
           visible: false

           GaussianBlur {
               anchors.fill: updateWindow
               source: mapView
               radius: 40
               samples: 20
           }

           MouseArea {
               anchors.fill: parent
               onClicked: mouse.accepted = true
               onWheel: wheel.accepted = true
           }

           Column {
               anchors {
                   fill: parent
                   margins: 10 * scaleFactor
               }
               spacing: 10
               Row {
                   anchors.horizontalCenter: parent.horizontalCenter
                   Text {
                       text: "Update Attribute"
                       font.pixelSize: 16 * scaleFactor
                   }
               }

               ComboBox {
                   id: damageComboBox
                   width: updateWindow.width - (20 * scaleFactor)
                   model: featAttributes
               }

               Row {
                   anchors.horizontalCenter: parent.horizontalCenter
                   spacing: 10

                   Button {
                       width: (updateWindow.width / 2) - (20 * scaleFactor)
                       text: "Update"
                       // once the update button is clicked, hide the windows, and fetch the currently selected features
                       onClicked: {
                           callout.visible = false;
                           updateWindow.visible = false;
                           updateFeaturesSample.updateSelectedFeature(damageComboBox.currentText)
                       }
                   }

                   Button {
                       width: (updateWindow.width / 2) - (20 * scaleFactor)
                       text: "Cancel"
                       // once the cancel button is clicked, hide the window
                       onClicked: updateWindow.visible = false;
                   }
               }
           }
       }
}
