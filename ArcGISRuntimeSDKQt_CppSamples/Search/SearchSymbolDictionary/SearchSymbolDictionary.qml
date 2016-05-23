// [WriteFile Name=SearchSymbolDictionary, Category=Search]
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
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

SearchSymbolDictionarySample {
    id: searchSymbolDictionarySample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property double fontSize: 16 * scaleFactor
    property var repeaterModel: ["Names", "Tags", "Classes", "Categories", "Keys"]
    property var searchParamList: [[],[],[],[],[]]

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/styles/mil2525d.stylx"

    Rectangle {
        id: topRectangle
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: (fieldColumn.childrenRect.height + 20) * scaleFactor
        width: parent.width *.9

        Column {
            id: fieldColumn
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }

            spacing: 10 * scaleFactor

            Repeater {
                id: repeater
                model: repeaterModel

                Row {
                    spacing: 10 * scaleFactor
                    width: parent.width

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 90 * scaleFactor
                        text: repeaterModel[index] + ":"
                        font.pixelSize: fontSize
                    }

                    ComboBox {
                        id: fieldComboBox
                        width: (parent.width * .5)
                        height: seachBtn.height
                        editable: true
                        inputMethodHints : Qt.ImhNoPredictiveText

                        style: ComboBoxStyle {
                            font.pixelSize: fontSize
                        }

                        model: searchParamList[index]

                        validator: RegExpValidator{ regExp: /^\s*[\da-zA-Z][\da-zA-Z\s]*$/  }

                        onAccepted: addField()

                        //Add a new field
                        function addField(){
                            if( searchParamList[index].indexOf(editText) === -1) {
                                searchParamList[index].push(editText);
                                model = searchParamList[index];
                            }
                            currentIndex = -1;
                            editText="";
                        }

                        //Remove a field
                        function removeField(){
                            //searchSymbolDictionarySample.removeNameField(editText);
                            var fieldIndex = searchParamList[index],indexOf;
                            if(fieldIndex !== -1){
                                searchParamList[index].splice(fieldIndex, 1);
                                model = searchParamList[index];
                            }
                            editText ="";
                            currentIndex = -1;
                        }
                    }

                    Button {
                        anchors.verticalCenter: parent.verticalCenter
                        text: " + "
                        style: seachBtn.style
                        enabled: fieldComboBox.acceptableInput

                        //Add field to combobox
                        onClicked: fieldComboBox.addField()
                    }

                    Button {
                        anchors.verticalCenter: parent.verticalCenter
                        text: " - "
                        style: seachBtn.style
                        enabled: fieldComboBox.acceptableInput && fieldComboBox.currentIndex !== -1 && fieldComboBox.count > 0

                        //Remove selected field from combobox
                        onClicked: fieldComboBox.removeField();
                    }

                    //Clear the combobox
                    function clearComboBox(){
                        fieldComboBox.editText ="";
                        searchParamList[index] = [];
                        fieldComboBox.model = searchParamList[index];
                    }
                }
            }

            Row {
                anchors {
                    margins: 10 * scaleFactor
                }
                spacing: 10 * scaleFactor

                Button {
                    id: seachBtn
                    width: text.length * fontSize
                    text: "Search Symbols"
                    style: ButtonStyle {
                        label: Text {
                            text: control.text
                            font.pixelSize: fontSize
                            horizontalAlignment: Text.AlignHCenter
                            color: enabled ? "black" : "grey"
                        }
                    }
                    onClicked:{
                        //Disable the search button and start the search
                        enabled = false;
                        resultView.visible = false;
                        searchSymbolDictionarySample.search(searchParamList[SearchSymbolDictionarySample.FieldNames],
                                                            searchParamList[SearchSymbolDictionarySample.FieldTags],
                                                            searchParamList[SearchSymbolDictionarySample.FieldClasses],
                                                            searchParamList[SearchSymbolDictionarySample.FieldCategories],
                                                            searchParamList[SearchSymbolDictionarySample.FieldKeys]);
                    }
                }

                Button {
                    text: "Clear"
                    enabled: seachBtn.enabled
                    style: seachBtn.style
                    onClicked: {
                        //Set the results visibility to false
                        resultView.visible = false;
                        //Reset the search parameters
                        console.log(repeater.count)
                        console.log(repeater.itemAt(0))
                        for (var i = 0; i < repeater.count; ++i)
                            repeater.itemAt(i).clearComboBox();
                    }
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

            visible: false
            clip: true
            model: styleSymbolSearchResultsListModel

            delegate: Component {
                Row {
                    anchors {
                        margins: 20 * scaleFactor
                    }
                    width: parent.width
                    spacing: 10 * scaleFactor

                    Image {
                        source: symbolImage //would be Symbol.swatchImage
                    }

                    Column {
                        width: parent.width
                        spacing: 10 * scaleFactor

                        Text {
                            id: nameText
                            text: repeaterModel[SearchSymbolDictionarySample.FieldNames] + ": " + name
                            font.pixelSize: fontSize
                            width: parent.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }

                        Text {
                            text: repeaterModel[SearchSymbolDictionarySample.FieldTags] + ": " + tags
                            font.pixelSize: fontSize
                            width: nameText.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }

                        Text {
                            text: repeaterModel[SearchSymbolDictionarySample.FieldClasses] + ": " + classes
                            font.pixelSize: fontSize
                            width: nameText.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }

                        Text {
                            text: repeaterModel[SearchSymbolDictionarySample.FieldCategories] + ": " + categories
                            font.pixelSize: fontSize
                            width: nameText.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }

                        Text {
                            text: repeaterModel[SearchSymbolDictionarySample.FieldKeys] + ": " + keys
                            font.pixelSize: fontSize
                            width: nameText.width
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        }
                    }
                }
            }
        }
    }

    //Update the number of results retuned
    onSearchCountUpdate: resultText.text = "Result(s) found: " + count

    //Search completed
    onSearchCompleted: {
        seachBtn.enabled = true;
        resultView.visible = true;
    }

    ListModel {
        id: styleSymbolSearchResultsListModel
    }
}
