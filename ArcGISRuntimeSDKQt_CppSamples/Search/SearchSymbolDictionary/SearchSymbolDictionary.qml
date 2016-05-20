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

            Row {
                id: nameRow
                spacing: 10 * scaleFactor
                width: parent.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 90 * scaleFactor
                    text: repeaterModel[SearchSymbolDictionarySample.FieldNames] + ":"
                    font.pixelSize: fontSize
                }

                ComboBox {
                    id: nameComboBox
                    width: (parent.width * .5)
                    height: seachBtn.height
                    editable: true
                    inputMethodHints : Qt.ImhNoPredictiveText

                    style: ComboBoxStyle {
                        font.pixelSize: fontSize
                    }

                    model: searchSymbolDictionarySample.namesFieldModel;

                    validator: RegExpValidator{ regExp: /^\s*[\da-zA-Z][\da-zA-Z\s]*$/ }
                    onAccepted: {
                        addField();
                    }

                    //Add a new field
                    function addField(){
                        searchSymbolDictionarySample.addNameField(editText);
                        currentIndex = -1;
                        editText="";
                    }

                    //Remove a field
                    function removeField(){
                        searchSymbolDictionarySample.removeNameField(editText);
                        editText ="";
                        currentIndex = -1;
                    }

                    //Clear the combobox
                    function clearFields(){
                        editText ="";
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " + "
                    style: seachBtn.style
                    enabled: nameComboBox.acceptableInput
                    onClicked: {
                        //Add field to combobox
                        nameComboBox.addField();
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " - "
                    style: seachBtn.style
                    enabled: ( nameComboBox.acceptableInput && nameComboBox.currentIndex !== -1 && nameComboBox.count > 0)? true : false
                    onClicked: {
                        //Remove selected field from combobox
                        nameComboBox.removeField();
                    }
                }
            }

            Row {
                id: catRow
                spacing: 10 * scaleFactor
                width: parent.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 90 * scaleFactor
                    text: repeaterModel[SearchSymbolDictionarySample.FieldCategories] + ":"
                    font.pixelSize: fontSize
                }

                ComboBox {
                    id: categoryComboBox
                    width: (parent.width * .5)
                    height: seachBtn.height
                    editable: true
                    inputMethodHints : Qt.ImhNoPredictiveText

                    style: nameComboBox.style

                    model: searchSymbolDictionarySample.catsFieldModel;

                    validator:nameComboBox.validator

                    onAccepted: {
                        addField();
                    }

                    //Add a new field
                    function addField(){
                        searchSymbolDictionarySample.addCatField(editText);
                        currentIndex = -1;
                        editText="";
                    }

                    //Remove a field
                    function removeField(){
                        searchSymbolDictionarySample.removeCatField(editText);
                        editText ="";
                        currentIndex = -1;
                    }

                    //Clear the combobox
                    function clearFields(){
                        editText ="";
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " + "
                    style: seachBtn.style
                    enabled: categoryComboBox.acceptableInput
                    onClicked: {
                        //Add field to combobox
                        categoryComboBox.addField();
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " - "
                    style: seachBtn.style
                    enabled: ( categoryComboBox.acceptableInput && categoryComboBox.currentIndex !== -1 && categoryComboBox.count > 0)? true : false
                    onClicked: {
                        //Remove selected field from combobox
                        categoryComboBox.removeField();
                    }
                }
            }

            Row {
                id: tagRow
                spacing: 10 * scaleFactor
                width: parent.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 90 * scaleFactor
                    text: repeaterModel[SearchSymbolDictionarySample.FieldTags] + ":"
                    font.pixelSize: fontSize
                }

                ComboBox {
                    id: tagComboBox
                    width: (parent.width * .5)
                    height: seachBtn.height
                    editable: true
                    inputMethodHints : Qt.ImhNoPredictiveText

                    style: nameComboBox.style

                    model: searchSymbolDictionarySample.tagsFieldModel;

                    validator:nameComboBox.validator

                    onAccepted: {
                        addField();
                    }

                    //Add a new field
                    function addField(){
                        searchSymbolDictionarySample.addTagField(editText);
                        currentIndex = -1;
                        editText="";
                    }

                    //Remove a field
                    function removeField(){
                        searchSymbolDictionarySample.removeTagField(editText);
                        editText ="";
                        currentIndex = -1;
                    }

                    //Clear the combobox
                    function clearFields(){
                        editText ="";
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " + "
                    style: seachBtn.style
                    enabled: tagComboBox.acceptableInput
                    onClicked: {
                        //Add field to combobox
                        tagComboBox.addField();
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " - "
                    style: seachBtn.style
                    enabled: ( tagComboBox.acceptableInput && tagComboBox.currentIndex !== -1 && tagComboBox.count > 0)? true : false
                    onClicked: {
                        //Remove selected field from combobox
                        tagComboBox.removeField();
                    }
                }
            }

            Row {
                id: keyRow
                spacing: 10 * scaleFactor
                width: parent.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 90 * scaleFactor
                    text: repeaterModel[SearchSymbolDictionarySample.FieldKeys] + ":"
                    font.pixelSize: fontSize
                }

                ComboBox {
                    id: keyComboBox
                    width: (parent.width * .5)
                    height: seachBtn.height
                    editable: true
                    inputMethodHints : Qt.ImhNoPredictiveText

                    style: nameComboBox.style

                    model: searchSymbolDictionarySample.keysFieldModel;

                    validator:nameComboBox.validator

                    onAccepted: {
                        addField();
                    }

                    //Add a new field
                    function addField(){
                        searchSymbolDictionarySample.addKeyField(editText);
                        currentIndex = -1;
                        editText="";
                    }

                    //Remove a field
                    function removeField(){
                        searchSymbolDictionarySample.removeKeyField(editText);
                        editText ="";
                        currentIndex = -1;
                    }

                    //Clear the combobox
                    function clearFields(){
                        editText ="";
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " + "
                    style: seachBtn.style
                    enabled: keyComboBox.acceptableInput
                    onClicked: {
                        //Add field to combobox
                        keyComboBox.addField();
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " - "
                    style: seachBtn.style
                    enabled: ( keyComboBox.acceptableInput && keyComboBox.currentIndex !== -1 && keyComboBox.count > 0)? true : false
                    onClicked: {
                        //Remove selected field from combobox
                        keyComboBox.removeField();
                    }
                }
            }

            Row {
                id: classRow
                spacing: 10 * scaleFactor
                width: parent.width

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 90 * scaleFactor
                    text: repeaterModel[SearchSymbolDictionarySample.FieldClasses] + ":"
                    font.pixelSize: fontSize
                }

                ComboBox {
                    id: classComboBox
                    width: (parent.width * .5)
                    height: seachBtn.height
                    editable: true
                    inputMethodHints : Qt.ImhNoPredictiveText

                    style: nameComboBox.style

                    model: searchSymbolDictionarySample.classesFieldModel;

                    validator:nameComboBox.validator

                    onAccepted: {
                        addField();
                    }

                    //Add a new field
                    function addField(){
                        searchSymbolDictionarySample.addClassField(editText);
                        currentIndex = -1;
                        editText="";
                    }

                    //Remove a field
                    function removeField(){
                        searchSymbolDictionarySample.removeClassField(editText);
                        editText ="";
                        currentIndex = -1;
                    }

                    //Clear the combobox
                    function clearFields(){
                        editText ="";
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " + "
                    style: seachBtn.style
                    enabled: classComboBox.acceptableInput
                    onClicked: {
                        //Add field to combobox
                        classComboBox.addField();
                    }
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " - "
                    style: seachBtn.style
                    enabled: ( classComboBox.acceptableInput && classComboBox.currentIndex !== -1 && classComboBox.count > 0)? true : false
                    onClicked: {
                        //Remove selected field from combobox
                        classComboBox.removeField();
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
                        searchSymbolDictionarySample.search();
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
                        nameComboBox.clearFields();
                        tagComboBox.clearFields();
                        classComboBox.clearFields();
                        keyComboBox.clearFields();
                        categoryComboBox.clearFields();
                        searchSymbolDictionarySample.clearModelsField();
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

                    Image {
                        source: symbolImage //would be Symbol.swatchImage
                    }

                    Column {
                        width: parent.width
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

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/BlueStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/BlueStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/BlueStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/RedStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }

        ListElement {
            symbolImage: "http://static.arcgis.com/images/Symbols/Basic/YellowStickpin.png"
            name: "some name"
            classes: "some classes"
            tags: "some tags"
            categories: "some categories"
            keys: "some categories"
        }
    }
}
