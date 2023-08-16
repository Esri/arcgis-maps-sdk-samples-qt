// [Legal]
// Copyright 2022 Esri.

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
import Esri.ArcGISRuntimeSamples

Rectangle {
    visible: SampleManager.currentMode === SampleManager.SourceCodeView

    Flickable {
        anchors {
            margins: 15
            fill: parent
        }
        contentWidth: textEdit.width
        contentHeight: textEdit.height
        ScrollIndicator.vertical: ScrollIndicator {}
        ScrollIndicator.horizontal: ScrollIndicator {}

        TextEdit {
            id: textEdit
            anchors.margins: 15
            readOnly: true
<<<<<<< HEAD
            activeFocusOnPress: false
=======
            focus: true
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
            textFormat: Text.PlainText
            selectByMouse: os === "ios" || os === "android" ? false : true
            Component.onCompleted: {
                // Set binding imperatively instead of declaratively. The 5.9 Qt to 5.12 
                // upgrade broke syntax highlighting on windows for the first sample. 
                // Likely a timing issue of setting the highlighter.
                textEdit.text = Qt.binding(function() {return SampleManager.currentSourceCode});
                SyntaxHighlighter.setHighlighter(textEdit);
            }

            Menu {
                id: contextMenu
                width: 72
                height: 36

                MenuItem {
                    width: contextMenu.width - 10
                    height: contextMenu.height - 10
                    text: qsTr('Copy')
                    onTriggered: {
                        textEdit.copy()
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton

                onClicked: {
                    if (mouse.button === Qt.RightButton && textEdit.selectedText !== "") {
                        contextMenu.x = mouse.x;
                        contextMenu.y = mouse.y;
                        contextMenu.open();
                    }
                }
            }
        }
    }

    ComboBox {
        id: control
        anchors {
            right: parent.right
            top: parent.top
            rightMargin: 20
            topMargin: 10
        }
        width: 200
<<<<<<< HEAD
        model: SampleManager.currentSample ? SampleManager.currentSample.codeFiles : []
=======
        model: SampleManager.currentSample.codeFiles
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
        textRole: "name"

        delegate: ItemDelegate {
            width: control.width
            contentItem: Text {
                text: name
                font: control.font
                color: control.currentIndex === index ? Material.accent : "black"
                elide: Text.ElideMiddle
                verticalAlignment: Text.AlignVCenter
            }
        }

        contentItem: Text {
            text: control.displayText
            elide: Text.ElideMiddle
            font: control.font
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

<<<<<<< HEAD
        visible: SampleManager.currentSample ? SampleManager.currentSample.codeFiles.size > 1 : false
=======
        visible: SampleManager.currentSample.codeFiles.size > 1
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
        onCurrentTextChanged: SampleManager.setSourceCodeIndex(currentIndex)

        onModelChanged: {
            // If this is the C++ sample viewer, this code block attempts to set the initial source code file to the primary C++ file

            /*
            * As of update 100.13, C++ samples' README.metadata.json snippets should list the primary C++ first, rendering
            * this code block unnecessary, but this is a more robust means of retrieving the desired file rather than
            * relying on the primary C++ file to be explicitly listed first
            */

            if (SampleManager.api === "C++" && model.size > 1) {
                /*
                * The primary C++ file is usually SampleName.cpp
                * SourceCode.path usually returns "qrc:/Samples/Category/SourceCodeDir"
                * Usually SourceCodeDir === SampleName
                    - We use the SourceCode.path instead of the Sample.name because sometimes the sample name differs from the file names
                    - For example: The sample named "Viewshed (Geoprocessing)" has internal files named "AnalyzeViewshed"
                * Below we pop off the SourceCodeDir of the first SourceCode file (it's the same for all files in the SourceCodeListModel)
                * We then append ".cpp" to it to get our desired primary C++ file name to search for (ie: SampleName.cpp)
                */
                let cppSampleFileName = model.get(0).path.split("/").pop() + ".cpp";

                // We now loop through all files in the SourceCodeListModel to look for a file name equal to SampleName.cpp
                for (let i = 0; i < model.size; i++) {
                    if (cppSampleFileName === model.get(i).name) {
                        control.currentIndex = i;
                        break;
                    }
                }
                // If a matching file is not found, the Sample Viewer default is `control.currentIndex = 0` which is usually the primary C++/QML file depending on the viewer
            }
        }
    }
}
