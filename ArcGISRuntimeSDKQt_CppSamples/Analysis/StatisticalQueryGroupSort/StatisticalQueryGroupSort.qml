// [WriteFile Name=StatisticalQueryGroupSort, Category=Analysis]
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

import QtQuick
import QtQuick.Controls
import Esri.Samples

StatisticalQueryGroupSortSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    StackView {
        id: stackView
        anchors.fill: parent

        // Initial page is the OptionsPage
        initialItem: OptionsPage {
            id: statisticOptionsPage
            onStatisticButtonClicked: {
                // execute the query
                queryStatistics();

                // show the results page
                stackView.push(resultsPage);
            }
        }

        // The ResultsPage is shown when a query is executed
        ResultsPage {
            id: resultsPage
            width: parent.width
            height: parent.height
            statisticResult: rootRectangle.resultsModel
            onBackClicked: stackView.pop();
        }
    }
}
