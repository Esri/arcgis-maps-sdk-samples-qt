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
import QtQuick.Layouts
import Esri.Samples

AnalyzeHotspotsSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property date fromThisDate : Date.fromLocaleDateString(Qt.locale(), "98/01/01", "yy/MM/dd")
    property date toThisDate : Date.fromLocaleDateString(Qt.locale(), "98/01/31", "yy/MM/dd")

    // Declare the MapView
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    onDisplayErrorDialog: {
        messageDialog.title = "Error";
        messageDialog.text = titleText;
        messageDialog.detailedText = detailedText;
        messageDialog.open();
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
                executeTaskWithDates(fromString, toString);
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
        // check if each date is within the period
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

