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
<<<<<<< HEAD
import Telemetry
=======
>>>>>>> v.next

Page {
    id: categoryView

    header: Column {
        spacing: 2
        ToolBar {
            height: 42
            Label {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Categories")
                font {
                    family: fontFamily
                    pixelSize: 18
                }
                color: "white"
            }
            anchors.left: parent.left
            anchors.right: parent.right
        }

        SearchBar {
            id: searchBar
            anchors {
                left: parent.left
                right: parent.right
                margins: 10
            }
            font.pixelSize: 14
            placeholderText: qsTr("Find a sample...")
            padding: 10
<<<<<<< HEAD

            onTextChanged: {
                if (GAnalytics.telemetryEnabled)
                    searchQueryEventTimer.start();
            }
=======
>>>>>>> v.next
        }
    }

    GridView {
        anchors {
            fill: parent
            leftMargin: 10
            topMargin: 10
            bottomMargin: 10
        }
        clip: true
        model: SampleManager.categories
        delegate: categoryCard
        cellWidth: width / 2
        cellHeight: cellWidth
        visible: searchBar.text === ""
    }

    Rectangle {
        color: "transparent"
        anchors {
            fill: parent
            margins: 10
        }

        SearchView {
            anchors {
                fill: parent
            }
            model: SampleSearchFilterModel {
                sourceModel: SampleManager.samples
                filterString: searchBar.text
            }
            visible: searchBar.text !== ""
        }
    }
    // Ensure virtual keyboard is not persisted after this item has been
    // hidden. This is an issue on iOS and Android devices.
    onVisibleChanged:  searchBar.focus = false;
<<<<<<< HEAD

    Timer {
        id: searchQueryEventTimer
        interval: 2500 // milliseconds
        running: false
        repeat: false
        onTriggered: {
            if (GAnalytics.telemetryEnabled && searchBar.text !== "")
                GAnalytics.postEvent("search_query", {"search_text": searchBar.text});
        }
    }
=======
>>>>>>> v.next
}
