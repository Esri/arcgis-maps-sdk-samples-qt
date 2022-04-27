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

import QtQuick 2.5
import Esri.ArcGISRuntime 100.14
import Esri.ArcGISRuntimeSamples 1.0

Item {
    property PortalItem portalItem: null

    property bool portalIsLoaded: portal.loadStatus === Enums.LoadStatusLoaded

    signal portalDoneLoading(bool success)

    signal portalItemDoneLoading(bool success, string itemId, string name)

    signal portalItemFetchDataProgress(string itemId, int progress)

    signal portalItemDataCompleted(string itemId, bool success)

    Portal {
        id: portal
        url: "https://www.arcgis.com"
    }

    Connections {
        target: portal
        function onLoadStatusChanged() {
            if (portal.loadStatus === Enums.LoadStatusLoading)
                return;

            // This is the earliest point we can register these connections -
            // as SampleManager will crash before the contexts are populated.
            // Here we take control of the SamepleManager output signals and
            // force it to signal when we require it. This is intended for the
            // QmlSampleManager to copy the functionality of the
            // CppSampleManager.
            portalDoneLoading.connect(SampleManager.portalDoneLoading);
            portalItemDoneLoading.connect(SampleManager.portalItemDoneLoading);
            portalItemFetchDataProgress.connect(SampleManager.portalItemFetchDataProgress);
            portalItemDataCompleted.connect(SampleManager.portalItemFetchDataCompleted);

            // Connections made now we emit our first signal.
            portalDoneLoading(portalIsLoaded);
        }
    }

    function load() {
        portal.load();
    }

    function fetch(outputPath) {
        portalItem.fetchData(outputPath);
    }

    function createPortalItem(itemId) {
        portalItem = ArcGISRuntimeEnvironment.createObject(
                    "PortalItem", { portal: portal, itemId: itemId });

        portalItem.loadStatusChanged.connect(()=> {
            if (portalItem.loadStatus === Enums.LoadStatusLoading)
                return;

            portalItemDoneLoading(
                        portalItem.loadStatus === Enums.LoadStatusLoaded,
                        portalItem.itemId,
                        portalItem.type === Enums.PortalItemTypeCodeSample ? portalItem.name : "");
        });

        portalItem.fetchDataProgressChanged.connect(()=> {
            portalItemFetchDataProgress(portalItem, portalItem.fetchDataProgress.progressPercentage);
        });

        portalItem.fetchDataStatusChanged.connect(()=> {
            if (portalItem.fetchDataStatus === Enums.TaskStatusInProgress)
                return;

            portalItemDataCompleted(portalItem.itemId,
                                    portalItem.fetchDataStatus === Enums.TaskStatusCompleted);
        });

        portalItem.load();
    }
}
