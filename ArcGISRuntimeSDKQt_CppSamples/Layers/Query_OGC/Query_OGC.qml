// [WriteFile Name=Query_OGC, Category=Layers]
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

import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.Samples 1.0
import QtQuick.Controls 1.4

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    Query_OGCSample {
        id: model
        mapView: view
    }

    Rectangle {
        anchors {
            fill: controlColumn
            margins: -5
        }
        color: "#efefef"
        radius: 5
        border {
            color: "darkgray"
            width: 1
        }
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Row {
            spacing: 5
            Text {
                id: fieldText_1
                anchors.verticalCenter: parent.verticalCenter
                text: "Where Clause"
            }

            ComboBox {
                width: 200
                model: [ "F_CODE = 'AP010'", "{ \"eq\" : [ { \"property\" : \"F_CODE\" }, \"AP010\" ] }", "F_CODE LIKE 'AQ%'", "{\"and\":[{\"eq\":[{ \"property\" : \"F_CODE\" }, \"AP010\"]},{ \"before\":" +
                    "[{ \"property\" : \"ZI001_SDV\"},\"2013-01-01\"]}]}" ]
            }
        }

        Row {
            spacing: 8
            Text {
                id: fieldText_3
                anchors.verticalCenter: parent.verticalCenter
                text: "Max Features"
            }

            TextField {
                id: fieldText_4
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: ""
                selectByMouse: true
                validator: IntValidator{}
            }
        }

        Row {
            spacing: 5
            Text {
                id: fieldText_5
                anchors.verticalCenter: parent.verticalCenter
                text: "Date Filter"
                rightPadding: 16
            }

            Switch {
                id: switch_button
            }
        }

        Row {
            spacing: 8
            Text {
                id: from_field
                anchors.verticalCenter: parent.verticalCenter
                text: "From"
                rightPadding: 40
            }

            TextField {
                id: from_date
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: ""
                selectByMouse: true
                validator: RegExpValidator { regExp: /^\d\d\d\d-(0?[1-9]|1[0-2])-(0?[1-9]|[12][0-9]|3[01])/ }
            }
        }

        Row {
            spacing: 8
            Text {
                id: to_field
                anchors.verticalCenter: parent.verticalCenter
                text: "To"
                rightPadding: 53
            }

            TextField {
                id: to_date
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: ""
                selectByMouse: true
                validator: RegExpValidator { regExp: /^\d\d\d\d-(0?[1-9]|1[0-2])-(0?[1-9]|[12][0-9]|3[01])/ }
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Query"
            onClicked: query(fieldText.text + populationText.text);
        }
    }
}
