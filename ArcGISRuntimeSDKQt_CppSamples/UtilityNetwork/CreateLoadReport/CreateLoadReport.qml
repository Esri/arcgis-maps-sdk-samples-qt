// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
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
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateLoadReportSample {
        id: sampleModel
        mapView: view
    }

        GridLayout {
            id: grid
            columns: 3

            Text { text: sampleModel.phases[0][0]; font.bold: true; }
            Text { text: sampleModel.phases[0][1]; color: "red"; visible: false}
            Text { text: "in"; font.underline: true; visible: false }
            Text { text: "a"; font.pixelSize: 20 }
            Text { text: "row"; font.strikeout: true }
            Component.onCompleted: {

            }
        }
    }
