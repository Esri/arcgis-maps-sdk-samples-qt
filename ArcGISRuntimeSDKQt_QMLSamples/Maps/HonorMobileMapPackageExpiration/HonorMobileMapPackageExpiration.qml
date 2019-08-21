// [WriteFile Name=HonorMobileMapPackageExpiration, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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
import Esri.ArcGISRuntime 100.6
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mmpk/"
    property url unpackPath: System.temporaryFolder.url + "/MmpkQml_%1.mmpk".arg(new Date().getTime().toString())
    property string expirationDate
    property string expirationTime
    property string expirationMessage

    // Create MapView
    MapView {
        id: mapView
        anchors.fill: parent
    }

    Component.onCompleted: {
        // check if direct read is supported before proceeding
        MobileMapPackageUtility.isDirectReadSupported(mmpk.path);
    }

    // Create a Mobile Map Package and set the path
    MobileMapPackage {
        id: mmpk
        path: dataPath + "LothianRiversAnno.mmpk"

        // wait for the mobile map package to load
        onLoadStatusChanged: {
            // only proceed once the map package is loaded
            if (loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }

            if (mmpk.maps.length < 1) {
                return;
            }

            // check if the package is expired
            if (expiration.expired) {
                const dateTime = expiration.dateTime;
                expirationDate = dateTime.toISOString().split("T")[0];
                expirationTime = dateTime.toISOString().split("T")[1].substring(0,8)
                expirationMessage = expiration.message;
            }

            // set the map view's map to the first map in the mobile map package
            mapView.map = mmpk.maps[0];
        }

        onErrorChanged: {
            console.log("Mobile Map Package Error: %1 %2".arg(error.message).arg(error.additionalMessage));
        }
    }

    // Connect to the various signals on MobileMapPackageUtility
    // to determine if direct read is supported or if an unpack
    // is needed.
    Connections {
        target: MobileMapPackageUtility

        onIsDirectReadSupportedStatusChanged: {
            if (MobileMapPackageUtility.isDirectReadSupportedStatus !== Enums.TaskStatusCompleted) {
                return;
            }

            // if direct read is supported, load the MobileMapPackage
            if (MobileMapPackageUtility.isDirectReadSupportedResult) {
                mmpk.load();
            } else {
                // direct read is not supported, and the data must be unpacked
                MobileMapPackageUtility.unpack(mmpk.path, unpackPath)
            }
        }

        onUnpackStatusChanged: {
            if (MobileMapPackageUtility.unpackStatus !== Enums.TaskStatusCompleted)
                return;

            // set the new path to the unpacked mobile map package
            mmpk.path = unpackPath;

            // load the mobile map package
            mmpk.load();
        }
    }

    Rectangle {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: 100
        color: "#303030"
        opacity: 0.75
        visible: mmpk.expiration ? mmpk.expiration.expired : false

        Text {
            id: expirationText
            anchors.centerIn: parent
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: "white"
            text: "%1\nExpired on %2 at %3 UTC".arg(expirationMessage).arg(expirationDate).arg(expirationTime)
        }
    }
}
