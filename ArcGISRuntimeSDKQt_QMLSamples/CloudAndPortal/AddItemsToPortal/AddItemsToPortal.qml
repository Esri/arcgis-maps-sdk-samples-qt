// [WriteFile Name=AddItemsToPortal, Category=CloudAndPortal]
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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url localCSVFile: "qrc:/Samples/CloudAndPortal/SearchForWebmap/stratigraphy.csv"
    property string currentLink

    PortalItem {
        id: itemToAdd
        portal: portal
        type: Enums.PortalItemTypeCSV
        title: "stratigraphy"

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            deleteItemButton.enabled = true;
        }
    }

    PortalUser {
        id: myUser
        portal: portal
        username: portal.portalUser ? portal.portalUser.username : ""

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad)
                return;

            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            statusBar.text = "Connected as " + username
            currentLink = portal.url
            addItemButton.enabled = true;
            createFolderButton.enabled = true;
        }

        onUsernameChanged: {
            if (loadStatus === Enums.LoadStatusLoaded)
                return;

            // If the username is valid and the portal has been loaded, we are ready to load the user
            if (username.length > 0 && portal.loadStatus === Enums.LoadStatusLoaded)
                load();
        }

        onErrorChanged: {
            statusBar.text = error.message + ": " + error.additionalMessage;
        }

        onAddPortalItemStatusChanged: {
            if (addPortalItemStatus !== Enums.TaskStatusCompleted)
                return;

            addItemButton.iconSource = "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_checkedcircled_light.png"
            statusBar.text = "Successfully added item:";
            currentLink = itemToAdd.url.toString();

            addItemButton.enabled = false;
            deleteItemButton.enabled = false;
            itemToAdd.load();
        }

        onCreateFolderStatusChanged: {
            if (createFolderStatus !== Enums.TaskStatusCompleted)
                return;

            statusBar.text = "created: " + createFolderResult.folderId;
            createFolderButton.enabled = false;
            createFolderButton.iconSource = "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_checkedcircled_light.png"
            moveItemButton.enabled = true;
            deleteFolderButton.enabled = true;
        }

        onMovePortalItemStatusChanged: {
            if (movePortalItemStatus !== Enums.TaskStatusCompleted)
                return;

            moveItemButton.iconSource = "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_checkedcircled_light.png"
            statusBar.text = "moved item to " + createFolderResult.title + "/";
            moveItemButton.enabled = false;
        }

        onDeletePortalItemStatusChanged: {
            if (deletePortalItemStatus !== Enums.TaskStatusCompleted)
                return;

            deleteItemButton.iconSource = "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_checkedcircled_light.png"
            statusBar.text = "deleted item " + itemToAdd.itemId;
            deleteItemButton.enabled = false;
        }

        onDeleteFolderStatusChanged: {
            if (deleteFolderStatus !== Enums.TaskStatusCompleted)
                return;

            deleteFolderButton.iconSource = "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_checkedcircled_light.png"
            statusBar.text = "deleted " + createFolderResult.title + "/";
            deleteFolderButton.enabled = false;
        }
    }

    Portal {
        id: portal
        loginRequired: true
        credential: Credential {
            oAuthClientInfo: OAuthClientInfo {
                oAuthMode: Enums.OAuthModeUser
                clientId: "W3hPKzPbeJ0tr8aj"
            }
        }

        Component.onCompleted: load();

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad)
                return;

            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            // The user cannot be loaded until it has been notified of the name change
            if (myUser.username.length > 0)
                myUser.load();
        }
    }

    Column {
        anchors {
            fill: parent
            margins: 16 * scaleFactor
        }
        spacing: 16 * scaleFactor
        visible: portal.loadStatus === Enums.LoadStatusLoaded

        Button {
            id: addItemButton
            anchors{
                left: parent.left
                right: parent.right
            }
            text: "Step 1: Add Item"
            enabled: false
            iconSource: "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_addencircled_light.png"

            onClicked: {
                myUser.addPortalItemWithUrl(itemToAdd, localCSVFile, "stratigraphy.csv");
            }
        }

        Button {
            id: createFolderButton
            anchors{
                left: parent.left
                right: parent.right
            }
            height: addItemButton.height
            text: "Step 2: Create Folder"
            enabled: false
            iconSource: "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_folder_light.png"

            onClicked: myUser.createFolder("Add Items To Portal Sample");
        }

        Button {
            id: moveItemButton
            anchors{
                left: parent.left
                right: parent.right
            }
            height: addItemButton.height
            text: "Step 3: Move to Folder"
            enabled: false
            iconSource: "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_actionarrow_light.png"

            onClicked: myUser.movePortalItem(itemToAdd, myUser.createFolderResult);
        }

        Button {
            id: deleteItemButton
            anchors{
                left: parent.left
                right: parent.right
            }
            height: addItemButton.height
            text: "Step 4: Delete Item"
            enabled: false
            iconSource: "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_subtract_light.png"

            onClicked: {
                myUser.deletePortalItem(itemToAdd);
            }
        }

        Button {
            id: deleteFolderButton
            anchors{
                left: parent.left
                right: parent.right
            }
            height: addItemButton.height
            text: "Step 5: Delete Folder"
            enabled: false
            iconSource: "qrc:/Samples/CloudAndPortal/AddItemsToPortal/ic_menu_subtract_light.png"

            onClicked: myUser.deleteFolder(myUser.createFolderResult);
        }


        Text {
            id: statusBar
            anchors{
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.Wrap
            elide: Text.ElideRight
        }

        Text {
            id: hyperlink
            text: '<html><a href="' + currentLink + '">' + currentLink +'</a></html>'
            onLinkActivated: Qt.openUrlExternally(currentLink)
        }

    }

    AuthenticationView {
        authenticationManager: AuthenticationManager
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: portal.loadStatus === Enums.TaskStatusInProgress ||
                 myUser.loadStatus === Enums.TaskStatusInProgress ||
                 myUser.addPortalItemStatus === Enums.TaskStatusInProgress ||
                 myUser.createFolderStatus === Enums.TaskStatusInProgress ||
                 myUser.movePortalItemStatus === Enums.TaskStatusInProgress ||
                 myUser.deletePortalItemStatus === Enums.TaskStatusInProgress ||
                 myUser.deleteFolderStatus === Enums.TaskStatusInProgress
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
