// [WriteFile Name=OpenScene, Category=Scenes]
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
import Esri.ArcGISRuntime 100.3

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // declare a SceneView
    SceneView {
        anchors.fill: parent

        // Declare a Scene
        Scene {
            // Declare a PortalItem by setting Item ID.
            // PortalItem is a default property of Scene
            PortalItem {
                itemId: "a13c3c3540144967bc933cb5e498b8e4"
            }
        }
    }
}
