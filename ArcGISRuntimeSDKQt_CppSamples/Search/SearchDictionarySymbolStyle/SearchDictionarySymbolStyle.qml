// [WriteFile Name=SearchDictionarySymbolStyle, Category=Search]
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
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

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
                    color: "lightgrey"
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
                        Text {
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
                            validator: RegExpValidator{ regExp: /^\s*[\da-zA-Z_][\da-zA-Z\s_]*$/ }
                            onAccepted: addCategoryButton.mouseArea.clicked();
                        }

                        Rectangle {
                            id: addCategoryButton
                            height: childrenRect.height
                            width: height
                            color: "transparent"
                            Image {
                                source: parent.enabled ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_addencircled_light.png"
                                                       : "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_addencircled_dark.png"
                            }
                            enabled: categoryEntry.text.length > 0

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                onClicked: {
                                    if (categoryEntry.text.length === 0)
                                        return;

                                    const tmp = searchParamList;
                                    tmp[index].push(categoryEntry.text);

                                    searchParamList = tmp;
                                    categoryEntry.text = "";
                                }
                            }
                        }

                        Label {
                            id: categoryList
                            Layout.fillWidth: true
                            Layout.column: 1
                            Layout.row: 1
                            text: searchParamList[index].length > 0 ? searchParamList[index].join() : ""
                        }

                        Rectangle {
                            height: childrenRect.height
                            width: height
                            color: "transparent"
                            Image {
                                source: parent.enabled ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_closeclear_light.png" :
                                                         "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_closeclear_dark.png"
                            }
                            enabled: categoryList.text.length > 0

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    categoryEntry.text = "";
                                    const tmp = searchParamList;
                                    tmp[index] = [];

                                    searchParamList = tmp;
                                }
                            }
                        }
                    }
                }
            }
        }

        Row {
            spacing: 10

            Button {
                id: seachBtn
                width: childrenRect.width
                Image {
                    id: searchImage
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        margins: 3
                    }
                    source: "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_find_light.png"
                }

                Text {
                    anchors {
                        left: searchImage.right
                        verticalCenter: parent.verticalCenter
                        margins: 3
                    }
                    text: searchParamList[0].length === 0 &&
                          searchParamList[1].length === 0 &&
                          searchParamList[2].length === 0 &&
                          searchParamList[3].length === 0 &&
                          searchParamList[4].length === 0 ?
                              "List All" : "Search"
                }

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
                text: "Clear"
                enabled: resultView.count > 0
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
                Image {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                        margins: 3
                    }
                    source: parent.checked ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_collapsed_light.png" :
                                             "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_expanded_light.png"
                }
            }
        }

        Text {
            id: resultText
            visible: resultView.visible
            text: "Result(s) found: " + resultView.count
            font.pixelSize: fontSize
        }

        Rectangle {
            id: bottomRectangle
            Layout.fillHeight: true
            Layout.fillWidth: true

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

                            Text {
                                id: nameText
                                text: "<b>Name:</b> " + name
                                font.pixelSize: fontSize
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                text: "<b>Tags:</b> " + tags
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                text: "<b>SymbolClass:</b> " + symbolClass
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                text: "<b>Category:</b> " + category
                                font.pixelSize: fontSize
                                width: nameText.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                text: "<b>Key:</b> " + key
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
    onSearchCompleted: {
        seachBtn.enabled = true;
        resultView.visible = true;

        //Update the number of results retuned
        resultText.text = "Result(s) found: " + count
    }
}
