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
import Calcite

ApplicationWindow {
    id: window
    // Intially hidden - unhide window in Component.onCompleted after overriding color so color switch is not visible
    visible: false
    width: 800
    height: 600
    minimumWidth: 300
    minimumHeight: 400

    palette {
        button: Calcite.brand
        buttonText: Calcite.text1
        highlight: Calcite.brandHover
        base: Calcite.background
        text: Calcite.text1
        mid: Calcite.foreground1
    }

    readonly property string os: Qt.platform.os
    readonly property string fontFamily: "Helvetica"

    header: ToolBar {
        height: 42

        background: Rectangle {
            color: Calcite.brand
        }

        ToolButton {
            id: hamburgerButton
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            icon.source: "qrc:/hamburger.svg"
            icon.width: 28
            icon.height: 28
            icon.color: Calcite.offWhite
            flat: true

            background: Rectangle {
                implicitWidth: 36
                implicitHeight: 30
                radius: 5
                color: hamburgerButton.hovered ? "#25FFFFFF" : "transparent"

                Behavior on color {
                    ColorAnimation { duration: 150 }
                }

                Rectangle {
                    id: hamburgerFlash
                    anchors.fill: parent
                    radius: parent.radius
                    color: "#40000000"
                    opacity: 0
                }
            }

            SequentialAnimation {
                id: hamburgerButtonTapAnim
                ParallelAnimation {
                    NumberAnimation { target: hamburgerButton; property: "scale"; to: 0.92; duration: 80; easing.type: Easing.OutQuad }
                    NumberAnimation { target: hamburgerFlash; property: "opacity"; to: 1; duration: 80 }
                }
                ParallelAnimation {
                    NumberAnimation { target: hamburgerButton; property: "scale"; to: 1.0; duration: 200; easing.type: Easing.OutBack }
                    NumberAnimation { target: hamburgerFlash; property: "opacity"; to: 0; duration: 250; easing.type: Easing.OutQuad }
                }
            }

            onClicked: {
                hamburgerButtonTapAnim.restart()
                if (drawer.opened) {
                    drawer.close()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            anchors.centerIn: parent
            text: qsTr("Qt Samples")
            font {
                pixelSize: 24
                family: fontFamily
            }
            color: Calcite.offWhite
        }

        ToolButton {
            id: menuButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            icon.source: "qrc:/ellipsis.svg"
            icon.width: 28
            icon.height: 28
            icon.color: Calcite.offWhite
            flat: true

            background: Rectangle {
                implicitWidth: 36
                implicitHeight: 30
                radius: 5
                color: menuButton.hovered ? "#25FFFFFF" : "transparent"

                Behavior on color {
                    ColorAnimation { duration: 150 }
                }

                Rectangle {
                    id: menuButtonFlash
                    anchors.fill: parent
                    radius: parent.radius
                    color: "#40000000"
                    opacity: 0
                }
            }

            SequentialAnimation {
                id: menuButtonTapAnim
                ParallelAnimation {
                    NumberAnimation { target: menuButton; property: "scale"; to: 0.92; duration: 80; easing.type: Easing.OutQuad }
                    NumberAnimation { target: menuButtonFlash; property: "opacity"; to: 1; duration: 80 }
                }
                ParallelAnimation {
                    NumberAnimation { target: menuButton; property: "scale"; to: 1.0; duration: 200; easing.type: Easing.OutBack }
                    NumberAnimation { target: menuButtonFlash; property: "opacity"; to: 0; duration: 250; easing.type: Easing.OutQuad }
                }
            }

            onClicked: {
                menuButtonTapAnim.restart()
                if (drawer.opened) {
                    drawer.close()
                }
                optionsMenu.open()
            }

            Menu {
                id: optionsMenu
                x: parent.width - width
                transformOrigin: Item.TopRight
                width: 200

                readonly property real menuFontSize: 16

                enter: Transition {
                    NumberAnimation { property: "scale"; from: 0.0; to: 1.0; duration: 60; easing.type: Easing.Linear }
                }

                MenuItem {
                    width: parent.width
                    height: visible? 48 : 0
                    text: qsTr("Home")
                    visible: SampleManager.currentMode != SampleManager.HomepageView
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        SampleManager.currentMode = SampleManager.HomepageView
                    }
                }
                MenuItem {
                    width: parent.width
                    height: visible ? 48 : 0
                    text: qsTr("Live Sample")
                    visible: SampleManager.currentSample
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        SampleManager.currentMode = SampleManager.LiveSampleView
                        // postEvent handled in
                        // - SampleListView.qml for categories
                        // - SearchView.qml for searches
                    }
                }
                MenuItem {
                    width: parent.width
                    height: visible ? 48 : 0
                    text: qsTr("Source Code")
                    visible: SampleManager.currentSample
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        SampleManager.currentMode = SampleManager.SourceCodeView
                    }
                }
                MenuItem {
                    width: parent.width
                    height: visible ? 48 : 0
                    text: qsTr("Description")
                    visible: SampleManager.currentSample
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        SampleManager.currentMode = SampleManager.DescriptionView
                    }
                }

                MenuItem {
                    width: parent.width
                    height: 48
                    text: qsTr("API Reference")
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        Qt.openUrlExternally(SampleManager.apiReferenceUrl)
                    }
                }

                MenuItem {
                    width: parent.width
                    height: 48
                    text: qsTr("Manage offline data")
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = false;
                        if (SampleManager.currentMode !== SampleManager.DownloadDataView || !SampleManager.downloadsManager.downloadInProgress)
                            SampleManager.currentMode = SampleManager.ManageOfflineDataView
                    }
                }

                MenuItem {
                    width: parent.width
                    height: 48
                    text: qsTr("Proxy Settings")
                    onTriggered: {
                        aboutView.visible = false;
                        proxySetupView.visible = true;
                    }
                }

                MenuItem {
                    width: parent.width
                    height: visible ? 48 : 0
                    text: qsTr("About")
                    visible: SampleManager.currentMode == SampleManager.HomepageView
                    onTriggered: {
                        aboutView.visible = true;
                        proxySetupView.visible = false;
                    }
                }

                MenuItem {
                    id: favoriteMenuItem
                    width: parent.width
                    height: visible ? 48 : 0

                    property bool currentSampleIsFavorite: false

                    text: currentSampleIsFavorite ? qsTr("Remove Favorite") : qsTr("Favorite")

                    visible: SampleManager.currentSample && (
                                 SampleManager.currentMode === SampleManager.LiveSampleView ||
                                 SampleManager.currentMode === SampleManager.DescriptionView ||
                                 SampleManager.currentMode === SampleManager.SourceCodeView ||
                                 SampleManager.currentMode === SampleManager.DownloadDataView)

                    onTriggered: {
                        const styledName = "<font color='" + Calcite.brand + "'>" + SampleManager.currentSample.name + "</font>";
                        if (currentSampleIsFavorite) {
                            SampleManager.removeSampleFromFavorites(SampleManager.currentSample);
                            showToast(qsTr("Removed %1 from favorites").arg(styledName));
                        } else {
                            SampleManager.addSampleToFavorites(SampleManager.currentSample);
                            showToast(qsTr("Added %1 to favorites").arg(styledName));
                        }
                    }

                    Connections {
                        target: SampleManager
                        function onFavoriteSamplesChanged() {
                            favoriteMenuItem.currentSampleIsFavorite =
                                    SampleManager.currentSample ? SampleManager.isFavorite(SampleManager.currentSample) : false;
                        }
                        function onCurrentSampleChanged() {
                            favoriteMenuItem.currentSampleIsFavorite =
                                    SampleManager.currentSample ? SampleManager.isFavorite(SampleManager.currentSample) : false;
                        }
                    }

                    Component.onCompleted: {
                        currentSampleIsFavorite =
                                SampleManager.currentSample ? SampleManager.isFavorite(SampleManager.currentSample) : false;
                    }
                }
            }
        }
    }

    CategoryDrawer {
        id: drawer
        width: 252
        height: parent.height
    }

    SourceCodeView {
        id: sourceCodeView
        anchors.fill: parent
    }

    DescriptionView {
        id: descriptionView
        anchors.fill: parent
    }

    ManageOfflineDataView {
        id: manageOfflineDataView
        anchors.fill: parent
    }

    HomepageView {
        id: homepageView
        anchors.fill: parent
    }

    LiveSampleView {
        id: liveSample
        anchors.fill: parent
    }

    NetworkRequiredView {
        id: networkRequiredView
        anchors.fill: parent
    }

    DataDownloadView {
        id: dataDownloadView
        anchors.fill: parent
    }

    ProxySetupView {
        id: proxySetupView
        anchors.fill: parent
    }

    AboutView {
        id: aboutView
        anchors.fill: parent
    }

    Connections {
        target: SampleManager

        property var pendingSampleChangeConnection: null

        function onCurrentSampleChanged() {
            // If we're in ManageOfflineData view and a download is in progress,
            // cancel all downloads and wait for completion before changing samples
            if (SampleManager.currentMode === SampleManager.ManageOfflineDataView &&
                    SampleManager.downloadsManager.downloadInProgress) {
                pendingSampleChangeConnection = SampleManager.downloadsManager.downloadInProgressChanged.connect(function() {
                    if (!SampleManager.downloadsManager.downloadInProgress) {
                        if (pendingSampleChangeConnection) {
                            SampleManager.downloadsManager.downloadInProgressChanged.disconnect(pendingSampleChangeConnection);
                            pendingSampleChangeConnection = null;
                        }
                        sampleChangeHelper();
                    }
                });

                // Cancel all downloads - this will show the busy indicator
                SampleManager.cancelAllDownloads();
                return;
            }

            // If no download in progress, proceed normally
            sampleChangeHelper();
        }

        function sampleChangeHelper() {
            clearSample();
            SampleManager.resetAuthenticationState();

            const noAuthSamplesList = [
                                        "qrc:/Samples/CloudAndPortal/AddItemsToPortal/AddItemsToPortal.qml",
                                        "qrc:/Samples/CloudAndPortal/IntegratedWindowsAuthentication/IntegratedWindowsAuthentication.qml",
                                        "qrc:/Samples/CloudAndPortal/PortalUserInfo/PortalUserInfo.qml",
                                        "qrc:/Samples/CloudAndPortal/SearchForWebmap/SearchForWebmap.qml",
                                        "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ShowOrgBasemaps.qml",
                                        "qrc:/Samples/CloudAndPortal/TokenAuthentication/TokenAuthentication.qml",
                                        "qrc:/Samples/EditData/EditWithBranchVersioning/EditWithBranchVersioning.qml",
                                        "qrc:/Samples/Maps/CreateAndSaveMap/CreateAndSaveMap.qml",
                                        "qrc:/Samples/UtilityNetwork/DisplayContentOfUtilityNetworkContainer/DisplayContentOfUtilityNetworkContainer.qml"
                                    ];

            if (noAuthSamplesList.indexOf(SampleManager.currentSample.source.toString()) === -1) {
                qmlLoaderAuthView.active = true;
            } else {
                qmlLoaderAuthView.active = false;
            }

            // If the sample requires online resources but there is no network connectivity
            if (SampleManager.currentSample.dataItems.size === 0
                    && SampleManager.reachability !== SampleManager.ReachabilityOnline
                    && SampleManager.reachability !== SampleManager.ReachabilityUnknown){
                SampleManager.currentMode = SampleManager.NetworkRequiredView;
                return;
                // If the sample requires offline data
            } else {
                showSample();
            }
        }

        function onSampleInitComplete() {
            // set Authenticator
            qmlLoaderAuthView.setSource("qrc:/qml/Authenticator.qml");

            SampleManager.currentMode = SampleManager.HomepageView;
        }

        function onCurrentModeChanged() {
            if (SampleManager.currentMode === SampleManager.LiveSampleView)
                showSample();
        }
    }

    Connections {
        target: SampleManager.downloadsManager

        function onDoneDownloadingChanged() {
            if (SampleManager.currentMode !== SampleManager.ManageOfflineDataView)
                showSample();
        }
    }

    // set the Loader's source and set the description text to the converted markdown
    function showSample() {
        if (SampleManager.currentSample) {
            descriptionView.descriptionText = SampleManager.currentSample.description;
            if (SampleManager.dataItemsExists()) {
                if (SampleManager.currentMode === SampleManager.ManageOfflineDataView
                        || SampleManager.currentMode === SampleManager.DownloadDataView
                        || SampleManager.currentMode === SampleManager.HomepageView)
                    SampleManager.currentMode = SampleManager.LiveSampleView;

                liveSample.source = SampleManager.currentSample.source;
            } else {
                if (SampleManager.downloadsManager.downloadInProgress)
                    SampleManager.currentMode = SampleManager.ManageOfflineDataView;
                else
                    SampleManager.currentMode = SampleManager.DownloadDataView;
            }
        }
    }

    function clearSample() {
        if (SampleManager.currentSample) {
            liveSample.source = "";
            descriptionView.descriptionText = "";
            gc();
        }
    }

    Loader {
        id: qmlLoaderAuthView
        anchors.fill: parent
    }

    // Toast notification
    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        width: toastLabel.implicitWidth + 32
        height: 40
        radius: 8
        color: Calcite.foreground3
        opacity: 0
        visible: opacity > 0

        Label {
            id: toastLabel
            anchors.centerIn: parent
            color: Calcite.text1
            font.pixelSize: 14
            textFormat: Text.StyledText
        }

        OpacityAnimator {
            id: toastFadeIn
            target: toast
            from: 0; to: 1
            duration: 200
        }

        OpacityAnimator {
            id: toastFadeOut
            target: toast
            from: 1; to: 0
            duration: 300
        }

        Timer {
            id: toastTimer
            interval: 2000
            onTriggered: toastFadeOut.start()
        }
    }

    function showToast(message) {
        toastLabel.text = message;
        toastFadeIn.start();
        toastTimer.restart();
    }

    Component.onCompleted: {
        // color overrides - purple
        Calcite.brand = Calcite.Theme.Light ? "#7938B6" : "#8F53CA";
        Calcite.brandHover = Calcite.Theme.Light ? "#652E98" : "#7938B6";
        Calcite.brandPress = Calcite.Theme.Light ? "#51247A" : "#652E98";

        window.visible = true;

        // initialize the SampleManager singleton
        SampleManager.init();
    }
}
