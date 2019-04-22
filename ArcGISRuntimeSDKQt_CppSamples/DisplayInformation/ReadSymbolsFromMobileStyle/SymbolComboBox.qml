// [WriteFile Name=ReadSymbolsFromMobileStyle, Category=DisplayInformation]
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

import QtQuick 2.11
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

ComboBox {
    id: rootComboBox
    textRole: "name"
    delegate: Item {
        id: cboDelegate
        height: 30
        width: parent.width

        RowLayout {
            Image {
                id: img
                source: symbolUrl
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
            }
            Label {
                text: name
                Layout.preferredWidth: cboDelegate.width - img.width
                elide: Text.ElideRight
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                rootComboBox.currentIndex = index;
                rootComboBox.popup.close();
            }
        }
    }
}
