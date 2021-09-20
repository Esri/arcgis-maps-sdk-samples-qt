
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

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    QueryOGCAPICQLFiltersSample {
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
                id: whereClauseText
                anchors.verticalCenter: parent.verticalCenter
                text: "Where Clause"
            }

            ComboBox {
                id: whereClauseMenu
                width: 200
                model: ["F_CODE = 'AP010'", "{ \"eq\" : [ { \"property\" : \"F_CODE\" }, \"AP010\" ] }", "F_CODE LIKE 'AQ%'", "{\"and\":[{\"eq\":[{ \"property\" : \"F_CODE\" }, \"AP010\"]},{ \"before\":"
                    + "[{ \"property\" : \"ZI001_SDV\"},\"2013-01-01\"]}]}"]
            }
        }

        Row {
            spacing: 8
            Text {
                id: maxFeatureText
                anchors.verticalCenter: parent.verticalCenter
                text: "Max Features"
            }

            TextField {
                id: maxFeatureField
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: "1000"
                selectByMouse: true
                validator: IntValidator {}
            }
        }

        Row {
            spacing: 8
            Text {
                id: fromField
                anchors.verticalCenter: parent.verticalCenter
                text: "From"
                rightPadding: 40
            }

            TextField {
                id: fromDate
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: ""
                selectByMouse: true
                validator: RegExpValidator {
                    regExp: /(0[1-9]|1[012])[-](0[1-9]|[12][0-9]|3[01])[-](19|20)\d\d/
                }
                placeholderText: "MM-DD-YYYY"
            }
        }

        Row {
            spacing: 8
            Text {
                id: toField
                anchors.verticalCenter: parent.verticalCenter
                text: "To"
                rightPadding: 53
            }

            TextField {
                id: toDate
                anchors.verticalCenter: parent.verticalCenter
                width: 200
                text: ""
                selectByMouse: true
                validator: RegExpValidator {
                    regExp: /(0[1-9]|1[012])[-](0[1-9]|[12][0-9]|3[01])[-](19|20)\d\d/
                }
                placeholderText: "MM-DD-YYYY"
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Query"
            onClicked: {
                model.query(whereClauseMenu.currentText, maxFeatureField.text,
                            fromDate.text, toDate.text)
            }
        }
    }
}
