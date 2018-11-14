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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.5

GenerateOfflineMap_OverridesSample {
    id: offlineMapOverridesSample
    clip: true
    width: 800
    height: 600

    
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
                margins: 5
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
        width: parent.width - (50)
        height: parent.height - (125)
        color: "transparent"
        visible: mapLoaded
        border {
            color: "red"
            width: 3
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

    Dialog {
        id: msgDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Layer Errors"
        property alias text : textLabel.text
        property alias detailedText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: "Some layers could not be taken offline."
            }
            Text {
                id: detailsLabel
            }
        }
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
