// [WriteFile Name=GenerateOfflineMap, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.4

GenerateOfflineMap_OverridesSample {
    id: offlineMapOverridesSample
    clip: true
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property string outputMapPackage: System.temporaryFolder.path + "/OfflineMap_Overrides_%1.mmpk".arg(new Date().getTime().toString())

    onUpdateStatus: generateWindow.statusText = status;
    onUpdateProgress: generateWindow.progressText = progress;
    onHideWindow: {
        generateWindow.hideWindow(time);

        if (success) {
            extentRectangle.visible = false;
            downloadButton.visible = false;
        }
    }

    onShowLayerErrors: {
        msgDialog.detailedText = error;
        msgDialog.open();
    }

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // Create a button and anchor it to the attribution top
        DownloadButton {
            id: downloadButton
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 5 * scaleFactor
            }
            visible: mapLoaded

            onButtonClicked: {
                setAreaOfInterest(extentRectangle.x, extentRectangle.y, (extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
            }
        }
    }

    // Create an extent rectangle for selecting the offline area
    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - (50 * scaleFactor)
        height: parent.height - (125 * scaleFactor)
        color: "transparent"
        visible: mapLoaded
        border {
            color: "red"
            width: 3 * scaleFactor
        }
    }

    OverridesWindow {
        id: overridesWindow
        anchors.fill: parent
        visible: overridesReady

        onBasemapLODSelected: setBasemapLOD(min, max);
        onBasemapBufferChanged: setBasemapBuffer(buffer);
        onRemoveSystemValvesChanged: removeSystemValves();
        onRemoveServiceConnectionChanged: removeServiceConnection();
        onHydrantWhereClauseChanged: setHydrantWhereClause(whereClause);
        onClipWaterPipesAOIChanged: setClipWaterPipesAOI(clip);
        onOverridesAccepted: {
            generateWindow.visible = true;
            takeMapOffline(outputMapPackage);
        }
    }

    GenerateWindow {
        id: generateWindow
        anchors.fill: parent
    }

    MessageDialog {
        id: msgDialog
        title: "Layer Errors"
        text: "Some layers could not be taken offline."
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: taskBusy
    }

    /* Uncomment this section when running as standalone application
    AuthenticationView {
        anchors.fill: parent
        authenticationManager: offlineMapOverridesSample.authenticationManager // set the authenticationManager property (this needs to be registered)
    }
    */
}
