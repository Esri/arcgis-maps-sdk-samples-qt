// [WriteFile Name=SearchDictionarySymbolStyle, Category=Search]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

SearchDictionarySymbolStyleSample {
    id: searchDictionarySymbolStyleSample
    width: 800
    height: 600

    readonly property double fontSize: 16
    readonly property var repeaterModel: ["Names", "Tags", "Symbol Classes", "Categories", "Keys"]
    readonly property var hintsModel: ["Fire", "Sustainment Points", "3", "Control Measure", "25212300_6"]
    property var searchParamList: [[],[],[],[],[]]

    ColumnLayout {
        id: topRectangle
        anchors {
            fill: parent
            margins: 9
        }

        Column {
            id: fieldColumn
            visible: !hideSearch.checked
            enabled: visible
            Layout.fillWidth: true
            Layout.margins: 8

            Repeater {
                id: repeater
                model: repeaterModel

                Rectangle {
                    width: parent.width
                    height: childrenRect.height
                    color: palette.base
                    border.color: "darkgrey"
                    radius: 2
                    clip: true

                    GridLayout {
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: 3
                        }
                        columns: 3
                        rowSpacing: 0
                        Label {
                            text: repeaterModel[index]
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            wrapMode: Text.WordWrap
                        }

                        TextField {
                            id: categoryEntry
                            Layout.fillWidth: true
                            placeholderText: repeaterModel[index] +" (e.g. "+ hintsModel[index] +")"
                            selectByMouse: true
                            validator: RegularExpressionValidator{ regularExpression: /^\s*[\da-zA-Z_][\da-zA-Z\s_]*$/ }
                            onAccepted: addCategoryButton.addCategory();
                        }

                        ToolButton {
                            id: addCategoryButton
                            height: 24
                            width: height
                            padding: 0
                            display: AbstractButton.IconOnly
                            enabled: categoryEntry.text.length > 0
                            opacity: enabled ? 1 : 0
                            icon {
                                source: "qrc:/Samples/Search/SearchDictionarySymbolStyle/plus-circle-24.svg"
                                width: 24
                                height: 24
                                color: hovered ? palette.buttonText : palette.text
                            }
                            onClicked: addCategory()

                            background: Rectangle {
                                color: parent.hovered ? palette.highlight : "transparent"
                                radius: 2
                            }

                            function addCategory() {
                                if (categoryEntry.text.length === 0)
                                    return;
                                const tmp = searchParamList;
                                tmp[index].push(categoryEntry.text);
                                searchParamList = tmp;
                                categoryEntry.text = "";
                            }
                        }

                        Label {
                            id: categoryList
                            Layout.fillWidth: true
                            Layout.column: 1
                            Layout.row: 1
                            text: qsTr(searchParamList[index].length > 0 ? searchParamList[index].join() : "")
                        }

                        ToolButton {
                            height: 24
                            width: height
                            padding: 0
                            display: AbstractButton.IconOnly
                            enabled: categoryList.text.length > 0
                            visible: enabled
                            icon {
                                source: "qrc:/Samples/Search/SearchDictionarySymbolStyle/x-24.svg"
                                width: 24
                                height: 24
                                color: hovered ? palette.buttonText : palette.text
                            }
                            onClicked: {
                                categoryEntry.text = "";
                                const tmp = searchParamList;
                                tmp[index] = [];
                                searchParamList = tmp;
                            }

                            background: Rectangle {
                                color: parent.hovered ? palette.highlight : "transparent"
                                radius: 2
                            }
                        }
                    }
                }
            }
        }

        Row {
            spacing: 10

            Button {
                id: searchBtn
                icon {
                    source: "qrc:/Samples/Search/SearchDictionarySymbolStyle/search-24.svg"
                    width: 24
                    height: 24
                    color: palette.buttonText
                }

                text: searchParamList[0].length === 0 &&
                      searchParamList[1].length === 0 &&
                      searchParamList[2].length === 0 &&
                      searchParamList[3].length === 0 &&
                      searchParamList[4].length === 0 ?
                          qsTr("List All") : qsTr("Search")

                onClicked: {
                    //start the search
                    resultView.visible = false;

                    searchDictionarySymbolStyleSample.search(searchParamList[SearchDictionarySymbolStyleSample.FieldNames],
                                                             searchParamList[SearchDictionarySymbolStyleSample.FieldTags],
                                                             searchParamList[SearchDictionarySymbolStyleSample.FieldClasses],
                                                             searchParamList[SearchDictionarySymbolStyleSample.FieldCategories],
                                                             searchParamList[SearchDictionarySymbolStyleSample.FieldKeys]);
                }
            }

            Button {
                text: qsTr("Clear")
                height: searchBtn.height
                onClicked: {
                    //Set the results visibility to false
                    resultView.visible = false;
                    //Reset the search parameters
                    searchParamList = [[],[],[],[],[]];
                }
            }

            Button {
                id: hideSearch
                checked: false
                checkable: true
                height: searchBtn.height

                icon {
                    source: checked ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/chevron-down-24.svg" : "qrc:/Samples/Search/SearchDictionarySymbolStyle/chevron-up-24.svg"
                    width: 24
                    height: 24
                    color: palette.buttonText
                }
            }
        }

        Label {
            id: resultText
            visible: resultView.visible
            text: qsTr("Result(s) found: " + resultView.count)
            font.pixelSize: fontSize
        }

        Rectangle {
            id: bottomRectangle
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: palette.base

            //Listview of results returned from Dictionary
            ListView {
                id: resultView
                anchors {
                    fill: parent
                    margins: 10
                }
                spacing: 20

                clip: true
                model: searchResultsListModel

                delegate: Component {
                    Row {
                        anchors {
                            margins: 20
                        }
                        width: resultView.width
                        spacing: 10

                        Image {
                            source: symbolUrl
                        }

                        Column {
                            width: parent.width
                            spacing: 10

                            Label {
                                id: nameText
                                text: qsTr("<b>Name:</b> %1").arg(name)
                                font.pixelSize: fontSize
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Label {
                                text: qsTr("<b>Tags:</b> %1").arg(tags)
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Label {
                                text: qsTr("<b>SymbolClass:</b> %1").arg(symbolClass)
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Label {
                                text: qsTr("<b>Category:</b> %1").arg(category)
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Label {
                                text: qsTr("<b>Key:</b> %1").arg(key)
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }
                    }
                }
            }
        }
    }

    //Search completed
    onSearchCompleted: count => {
                           searchBtn.enabled = true;
                           resultView.visible = true;

                           //Update the number of results retuned
                           resultText.text = "Result(s) found: " + count
                       }
}
