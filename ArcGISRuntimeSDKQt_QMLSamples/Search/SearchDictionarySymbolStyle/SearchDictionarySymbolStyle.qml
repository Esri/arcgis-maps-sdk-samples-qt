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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    property double fontSize: 16 * scaleFactor
    property var repeaterModel: ["Names", "Tags", "Symbol Classes", "Categories", "Keys"]
    property var hintsModel: ["Fire", "Sustainment Points", "3", "Control Measure", "25212300_6"]
    property var searchParamList: [[],[],[],[],[]]

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/styles/mil2525d.stylx"

    DictionarySymbolStyle {
        id: dictionarySymbolStyle
        specificationType: "mil2525d"
        styleLocation: dataPath

        //Search completed
        onSearchSymbolsStatusChanged:{
            if (searchSymbolsStatus !== Enums.TaskStatusCompleted)
                return;

            resultView.visible = true;

            //Update the number of results retuned
            resultText.text = "Result(s) found: " + searchSymbolsResult.count
        }
    }

    SymbolStyleSearchParameters {
        id: searchParams
    }

    Rectangle {
        id: topRectangle
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: hideSearch.checked ?  searchRow.height + resultText.height + (20 * scaleFactor) :
                                     fieldColumn.childrenRect.height + (20 * scaleFactor)
        width: parent.width *.9

        Column {
            id: fieldColumn
            anchors {
                fill: parent
                margins: 8 * scaleFactor
            }

            spacing: 4 * scaleFactor

            Repeater {
                id: repeater
                model: repeaterModel

                Rectangle {
                    width: parent.width
                    height: hideSearch.checked ? 0 : 72 * scaleFactor
                    color: "lightgrey"
                    border.color: "darkgrey"
                    radius: 4
                    clip: true

                    Text {
                        id: categoryTitle
                        anchors {
                            top: parent.top
                            left: parent.left
                            margins: 8 * scaleFactor
                        }
                        height: categoryEntry.height
                        width: 66 * scaleFactor
                        text: repeaterModel[index]
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        wrapMode: Text.WordWrap
                    }

                    Button {
                        id: addCategoryButton
                        anchors {
                            top: parent.top
                            right: parent.right
                            margins: 8 * scaleFactor
                        }
                        height: categoryEntry.height
                        width: height
                        iconSource: enabled ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_addencircled_light.png" :
                                              "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_addencircled_dark.png"
                        enabled: categoryEntry.text.length > 0

                        onClicked: {
                            if (categoryEntry.text.length === 0)
                                return;

                            var tmp = searchParamList;
                            tmp[index].push(categoryEntry.text);

                            searchParamList = tmp
                            categoryEntry.text = "";
                        }
                    }

                    Button {
                        id: clearCategoryButton
                        anchors {
                            top: addCategoryButton.bottom
                            right: parent.right
                            margins: 8 * scaleFactor
                        }
                        height: categoryEntry.height
                        width: height
                        iconSource: enabled ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_closeclear_light.png" :
                                              "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_closeclear_dark.png"
                        enabled: categoryList.text.length > 0

                        onClicked: {
                            categoryEntry.text = "";
                            var tmp = searchParamList;
                            tmp[index] = [];

                            searchParamList = tmp;
                        }
                    }

                    TextField {
                        id: categoryEntry
                        anchors {
                            top: parent.top
                            right: addCategoryButton.left
                            left: categoryTitle.right
                            margins: 8 * scaleFactor
                        }
                        placeholderText: repeaterModel[index] +" (e.g. "+ hintsModel[index] +")"
                        validator: RegExpValidator{ regExp: /^\s*[\da-zA-Z_][\da-zA-Z\s_]*$/ }
                        onAccepted:  addCategoryButton.clicked();
                    }

                    Label {
                        id: categoryList
                        anchors {
                            top: categoryEntry.bottom
                            right: parent.right
                            left: parent.left
                            margins: 8 * scaleFactor
                        }
                        height: 32 * scaleFactor
                        text: searchParamList[index].length > 0 ? searchParamList[index].join() : ""
                    }
                }
            }

            Row {
                id: searchRow
                anchors {
                    margins: 10 * scaleFactor
                }
                spacing: 10 * scaleFactor

                Button {
                    id: seachBtn
                    width: 100 * scaleFactor
                    height: 32 * scaleFactor
                    text: searchParamList[0].length === 0 &&
                          searchParamList[1].length === 0 &&
                          searchParamList[2].length === 0 &&
                          searchParamList[3].length === 0 &&
                          searchParamList[4].length === 0 ?
                              "List All" : "Search"
                    iconSource: "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_find_light.png"

                    onClicked: {
                        resultView.visible = false;

                        searchParams.names = searchParamList[0];
                        searchParams.tags = searchParamList[1];
                        searchParams.symbolClasses = searchParamList[2];
                        searchParams.categories = searchParamList[3];
                        searchParams.keys = searchParamList[4];

                        dictionarySymbolStyle.searchSymbols(searchParams);
                    }
                }

                Button {
                    text: "Clear"
                    height: seachBtn.height
                    enabled: resultView.count > 0
                    style: seachBtn.style
                    onClicked: {
                        //Set the results visibility to false
                        resultView.visible = false;
                        //Reset the search parameters
                        searchParamList = [[],[],[],[],[]];
                    }
                }

                Button {
                    id: hideSearch
                    height: seachBtn.height
                    checked: false
                    checkable: true
                    iconSource: checked ? "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_collapsed_light.png" :
                                          "qrc:/Samples/Search/SearchDictionarySymbolStyle/ic_menu_expanded_light.png"
                }
            }

            Text {
                id: resultText
                visible: resultView.visible
                text: "Result(s) found: " + resultView.count
                font.pixelSize: fontSize
            }
        }
    }

    Rectangle {
        id: bottomRectangle
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: topRectangle.bottom
        }
        width: parent.width

        //Listview of results returned from Dictionary
        ListView {
            id: resultView
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 20 * scaleFactor

            clip: true
            model: dictionarySymbolStyle.searchSymbolsResult

            delegate: Component {
                Row {
                    anchors {
                        margins: 20 * scaleFactor
                    }
                    width: resultView.width
                    spacing: 10 * scaleFactor

                    Image {
                        source: symbolUrl
                    }

                    Column {
                        width: parent.width
                        spacing: 10 * scaleFactor

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
