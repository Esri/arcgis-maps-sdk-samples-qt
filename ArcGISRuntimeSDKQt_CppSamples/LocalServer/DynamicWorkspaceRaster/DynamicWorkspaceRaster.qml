// [WriteFile Name=DynamicWorkspaceRaster, Category=LocalServer]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

DynamicWorkspaceRasterSample {
    id: rootRectangle
    clip: true

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    property var supportedFormats: ["Raster Files (*.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)"]

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Button {
        id: addDataButton
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        text: "Add Raster"
        onClicked: fileDialog.open();
    }

    FileDialog {
        id: fileDialog
        nameFilters: supportedFormats
        folder: dataPath
        onAccepted: startLocalService(file, folder);
    }

    onLocalServerInitializationComplete: {
        if (!success) {
            addDataButton.text = "Error Initializing Local Server";
            addDataButton.enabled = false;
            return;
        }
        startLocalService(dataPath + "/usa_raster.tif", dataPath);
    }
}
