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

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    minimumWidth: 300
    minimumHeight: 400

    readonly property string os: Qt.platform.os
    readonly property string fontFamily: "Helvetica"

    header: ToolBar {
        height: 42

        Image {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            source: "qrc:/drawer.png"
            width: 36
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: drawer.open()
            }
        }

        Label {
            anchors.centerIn: parent
            text: qsTr("Qt Samples - %1").arg(SampleManager.api)
            font {
                pixelSize: 24
                family: fontFamily
            }
            color: "white"
        }

        Image {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            source: "qrc:/menu.png"
            width: 32
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: optionsMenu.open()

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight
                    width: 200
                    height: contentChildren.height

                    readonly property real menuFontSize: 16

                    MenuItem {
                        width: parent.width
                        height: 48
                        text: qsTr("Home")
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
                        text: qsTr("Manage offline data")
                        onTriggered: {
                            aboutView.visible = false;
                            proxySetupView.visible = false;
                            if (SampleManager.currentMode !== SampleManager.DownloadDataView || !SampleManager.downloadInProgress)
                                SampleManager.currentMode = SampleManager.ManageOfflineDataView
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
                        text: qsTr("Proxy Settings")
                        onTriggered: {
                            aboutView.visible = false;
                            proxySetupView.visible = true;
                        }
                    }

                    MenuItem {
                        width: parent.width
                        height: 48
                        text: qsTr("About")
                        onTriggered: {
                            aboutView.visible = true;
                            proxySetupView.visible = false;
                        }
                    }
                }
            }
        }
    }

    CategoryDrawer {
        id: drawer
        width: 240
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

        function onCurrentSampleChanged() {
            clearSample();
            SampleManager.clearCredentialCache();

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
                    && SampleManager.reachability !== SampleManager.ReachabilityOnline){
                SampleManager.currentMode = SampleManager.NetworkRequiredView;
                return;
            // If the sample requires offline data
            } else {
                showSample();
            }
        }

        function onApiKeyRequired(apiKey) {
            qmlArcGISRuntimeEnvironment.setSource("qrc:/qmlsamples/QmlArcGISRuntimeEnvironment.qml", {apiKey: apiKey})
        }

        function onSampleInitComplete() {
            // set AuthenticationView
            if (SampleManager.api === "QML") {
                qmlLoaderAuthView.setSource("qrc:/qml/QmlAuthenticationView.qml");
            } else {
                qmlLoaderAuthView.setSource("qrc:/qml/CppAuthenticationView.qml");
            }

            SampleManager.currentMode = SampleManager.HomepageView;
        }

        function onDoneDownloadingChanged() {
            showSample();
        }

        function onCurrentModeChanged() {
            if (SampleManager.currentMode === SampleManager.LiveSampleView)
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
                if (SampleManager.downloadInProgress)
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
        id: qmlArcGISRuntimeEnvironment
    }

    Loader {
        id: qmlLoaderAuthView
        anchors.fill: parent
    }

    Component.onCompleted: {
        // initialize the SampleManager singleton
        SampleManager.init();
    }
}
