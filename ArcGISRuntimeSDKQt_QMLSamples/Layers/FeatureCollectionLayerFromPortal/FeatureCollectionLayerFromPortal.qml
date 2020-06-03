// [WriteFile Name=FeatureCollectionLayerFromPortal, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url portalUrl: "https://www.arcgis.com/"

    MessageDialog {
          id: messageDialog
          title: "Feature collection"
          onTextChanged: visible = true;
          visible: false
          onRejected: {
              visible = false;
          }
      }

    MapView {
        id: mapView
        anchors.fill: parent

        Rectangle {
            id: backgroundRect
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"

            GridLayout {
                rows: 3
                columns: 1
                rowSpacing: 0

                RowLayout {
                    Text {
                        text: "Enter item ID and click button"
                        font.bold: true
                    }
                }
                RowLayout {
                    TextField {
                        id: textfield
                        text: "32798dfad17942858d5eef82ee802f0b"
                    }
                }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Button {
                        text: "Open from portal item"
                        onClicked: {
                            const trimmedItemId = textfield.text.trim();

                            if (!trimmedItemId || trimmedItemId === "") {
                                messageDialog.text = "Please enter a portal item ID";
                                return;
                            }

                            portalItem.itemId = trimmedItemId;
                            portalItem.load();
                        }
                    }
                }
            }
        }

        Map {
            id: map
            BasemapOceans {}
        }

        PortalItem {
            id: portalItem
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusFailedToLoad) {
                    console.warn(loadError.message, loadError.additionalMessage);
                    return;
                }
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                console.log("loaded");

                if (type === Enums.PortalItemTypeFeatureCollection) {
                    console.log("is feature collection");
                    let featureCollection = ArcGISRuntimeEnvironment.createObject("FeatureCollection", {item: portalItem});
                    let featureCollectionLayer = ArcGISRuntimeEnvironment.createObject("FeatureCollectionLayer", {featureCollection: featureCollection});
                    map.operationalLayers.append(featureCollectionLayer);
                } else {
                    console.log("Portal item with ID '" + itemId + "' is not a feature collection.");
                    messageDialog.text = "Portal item with ID '" + itemId + "' is not a feature collection.";
                }

            }
            Portal {
                url: portalUrl
            }
//            itemId: textfield.text
        }
    }
}
