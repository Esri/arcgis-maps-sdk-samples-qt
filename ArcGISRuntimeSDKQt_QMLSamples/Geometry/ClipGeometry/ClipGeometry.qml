// [WriteFile Name=ClipGeometry, Category=Geometry]
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
  id: rootRectangle
  clip: true
  width: 800
  height: 600

  property real scaleFactor: System.displayScaleFactor

  MapView {
    id: mapView
    anchors.fill: parent

    Map {
      BasemapTopographic {}

      // Create the envelope for Colorado
      Envelope {
        id: colorado
        xMax: -11362327.128340
        xMin: -12138232.018408
        yMin: 4441198.773776
        yMax: 5012861.290274
        spatialReference: SpatialReference {
          wkid: 3857
        }
      }

      // Create the dotted outline for the clipping envelopes
      SimpleLineSymbol {
        id: redOutline
        style: Enums.SimpleLineSymbolStyleDot
        color: "#FFFF0000"
        width: 3
      }

      // Create the outline and fill for Colorado
      SimpleFillSymbol {
        id: coloradoFillSymbol
        color: "#220000FF"
        style: Enums.SimpleFillSymbolStyleSolid
        SimpleLineSymbol {
          color: "#FF0000FF"
          width: 2.0
          style: Enums.SimpleFillSymbolStyleSolid
        }
      }
    }

    // Create an envelope outside of Colorado
    Envelope {
      id: outsideEnvelope
      xMax: -11858344.321294
      xMin: -12201990.219681
      yMax: 5297071.577304
      yMin: 5147942.225174
    }

    // Create an envelope intersecting Colorado
    Envelope {
      id: intersectingEnvelope
      xMax: -11962086.479298
      xMin: -12260345.183558
      yMax: 4566553.881363
      yMin: 4332053.378376
    }

    // Create an envelope inside of Colorado
    Envelope {
      id: containedEnvelope
      xMax: -11431488.567009
      xMin: -11655182.595204
      yMax: 4741618.772994
      yMin: 4593570.068343
    }

    // Create a graphics overlay to contain the clipping envelopes
    GraphicsOverlay {

      // Colorado
      Graphic {
        geometry: colorado
        symbol: coloradoFillSymbol
      }

      // Outside envelope
      Graphic {
        geometry: outsideEnvelope
        symbol: redOutline
      }

      // Intersecting envelope
      Graphic {
        geometry: intersectingEnvelope
        symbol: redOutline
      }

      // Contained envelope
      Graphic {
        geometry: containedEnvelope
        symbol: redOutline
      }
    }

    // Create a graphics overlay for the clipped graphics
    GraphicsOverlay {
      id: clipAreasOverlay
    }
  }

  Button {
    id: clipButton
  }

}
