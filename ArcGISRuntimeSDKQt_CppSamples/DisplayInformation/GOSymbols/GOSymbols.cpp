// [WriteFile Name=GOSymbols, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "GOSymbols.h"

#include "Map.h"
#include "MapQuickView.h"
#include "SpatialReference.h"
#include "Point.h"
#include "PolylineBuilder.h"
#include "Polyline.h"
#include "PolygonBuilder.h"
#include "Polygon.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleFillSymbol.h"
#include "TextSymbol.h"
#include "SimpleRenderer.h"
#include <QList>

using namespace Esri::ArcGISRuntime;

GOSymbols::GOSymbols(QQuickItem* parent) :
  QQuickItem(parent)
{
}

GOSymbols::~GOSymbols() = default;

void GOSymbols::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GOSymbols>("Esri.Samples", 1, 0, "GOSymbolsSample");
}

void GOSymbols::componentComplete()
{
  //! [GOSymbols create graphics overlay]
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the oceans basemap
  constexpr double lat = 56.075844;
  constexpr double lon = -2.681572;
  constexpr int levelOfDetail = 11;
  m_map = new Map(BasemapType::Oceans, lat, lon, levelOfDetail, this);
  // set map on the map view
  m_mapView->setMap(m_map);

  // create a new graphics overlay
  GraphicsOverlay* graphicsOverlay = new GraphicsOverlay(this);

  // add the overlay to the mapview
  m_mapView->graphicsOverlays()->append(graphicsOverlay);
  //! [GOSymbols create graphics overlay]

  //! [GOSymbols add all graphics]
  // add graphics to the graphics overlays
  //  add point graphics
  addBuoyPoints(graphicsOverlay);

  // add line graphics
  addBoatTrip(graphicsOverlay);

  // add polygon graphics
  addNestingGround(graphicsOverlay);

  // add text
  addText(graphicsOverlay);
  //! [GOSymbols add all graphics]
}

//! [GOSymbols addBuoyPoints]
void GOSymbols::addBuoyPoints(GraphicsOverlay* graphicsOverlay)
{
  // create a list of points
  const QList<Point> pointsList { Point(-2.712642647560347, 56.062812566811544, SpatialReference::wgs84())
                                , Point(-2.6908416959572303, 56.06444173689877, SpatialReference::wgs84())
                                , Point(-2.6697273884990937, 56.064250073402874, SpatialReference::wgs84())
                                , Point(-2.6395150461199726, 56.06127916736989, SpatialReference::wgs84())
                          };

  // create the symbology for the points
  constexpr float size = 10.0f;
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), size, this);

  // create a graphic and add each of them to the overlay
  for (const Point& buoyPoint : pointsList)
  {
    Graphic* graphic = new Graphic(buoyPoint, this);
    graphic->setSymbol(sms);
    graphicsOverlay->graphics()->append(graphic);
  }
}
//! [GOSymbols addBuoyPoints]

//! [GOSymbols addBoatTrip]
void GOSymbols::addBoatTrip(GraphicsOverlay* graphicsOverlay)
{
  constexpr float width = 1.0f;
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 255, 180), width, this);

  // json for the polyline
  QString polylineJson = "{\"paths\":[[[-2.7184791227926772,56.06147084563517],"
                         "[-2.7196807500463924,56.06147084563517],"
                         "[-2.722084004553823,56.062141712059706],"
                         "[-2.726375530459948,56.06386674355254],"
                         "[-2.726890513568683,56.0660708381432],"
                         "[-2.7270621746049275,56.06779569383808],"
                         "[-2.7255172252787228,56.068753913653914],"
                         "[-2.723113970771293,56.069424653352335],"
                         "[-2.719165766937657,56.07028701581465],"
                         "[-2.713672613777817,56.070574465681325],"
                         "[-2.7093810878716917,56.07095772883556],"
                         "[-2.7044029178205866,56.07153261642126],"
                         "[-2.698223120515766,56.072394931722265],"
                         "[-2.6923866452834355,56.07325722773041],"
                         "[-2.68672183108735,56.07335303720707],"
                         "[-2.6812286779275096,56.07354465544585],"
                         "[-2.6764221689126497,56.074215311778964],"
                         "[-2.6698990495353394,56.07488595644139],"
                         "[-2.6647492184479886,56.075748196715914],"
                         "[-2.659427726324393,56.076131408423215],"
                         "[-2.654792878345778,56.07622721075461],"
                         "[-2.651359657620878,56.076514616319784],"
                         "[-2.6477547758597324,56.07708942101955],"
                         "[-2.6450081992798125,56.07814320736718],"
                         "[-2.6432915889173625,56.08025069360931],"
                         "[-2.638656740938747,56.08044227755186],"
                         "[-2.636940130576297,56.078813783674946],"
                         "[-2.636425147467562,56.07728102068079],"
                         "[-2.637798435757522,56.076610417698504],"
                         "[-2.638656740938747,56.07507756705851],"
                         "[-2.641231656482422,56.07479015077557],"
                         "[-2.6427766058086277,56.075748196715914],"
                         "[-2.6456948434247924,56.07546078543464],"
                         "[-2.647239792750997,56.074598538729404],"
                         "[-2.6492997251859376,56.072682365868616],"
                         "[-2.6530762679833284,56.0718200569986],"
                         "[-2.655479522490758,56.070861913404286],"
                         "[-2.6587410821794135,56.07047864929729],"
                         "[-2.6633759301580286,56.07028701581465],"
                         "[-2.666637489846684,56.07009538137926],"
                         "[-2.670070710571584,56.06990374599109],"
                         "[-2.6741905754414645,56.069137194910745],"
                         "[-2.678310440311345,56.06808316228391],"
                         "[-2.682086983108735,56.06789151689155],"
                         "[-2.6868934921235956,56.06760404701653],"
                         "[-2.6911850180297208,56.06722075051504],"
                         "[-2.695133221863356,56.06702910083509],"
                         "[-2.698223120515766,56.066837450202335],"
                         "[-2.7016563412406667,56.06645414607839],"
                         "[-2.7061195281830366,56.0660708381432],"
                         "[-2.7100677320166717,56.065591697864576],"
                         "[-2.713329291705327,56.06520838135397],"
                         "[-2.7167625124302273,56.06453756828941],"
                         "[-2.718307461756433,56.06348340989081],"
                         "[-2.719165766937657,56.062812566811544],"
                         "[-2.7198524110826376,56.06204587471371],"
                         "[-2.719165766937657,56.06166252294756],"
                         "[-2.718307461756433,56.06147084563517]]],"
                         "\"spatialReference\":{\"wkid\":4326}}";

  // create a polyline from the json
  Esri::ArcGISRuntime::Polyline polyline = Polyline::fromJson(polylineJson);
  // create a new graphic using the polyline geometry
  Graphic* graphic = new Graphic(polyline, this);
  // set the symbology for the graphic
  graphic->setSymbol(sls);
  // add the graphic to the graphics overlay
  graphicsOverlay->graphics()->append(graphic);
}
//! [GOSymbols addBoatTrip]

//! [GOSymbols addNestingGround]
void GOSymbols::addNestingGround(GraphicsOverlay* graphicsOverlay)
{
  // outline for the polygon
  constexpr float width = 1.0f;
  SimpleLineSymbol* outline = new SimpleLineSymbol(SimpleLineSymbolStyle::Dash, QColor(0, 0, 0, 128), width, this);
  // create a fill symbol for a polygon
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::DiagonalCross, QColor(0, 255, 0, 128), outline, this);

  // create a graphic using polygon as the geometry
  Graphic* graphic = new Graphic(createNestingGround(), this);
  // set the symbology for the graphic
  graphic->setSymbol(sfs);
  // add the graphic to the overlay
  graphicsOverlay->graphics()->append(graphic);
}
//! [GOSymbols addNestingGround]

//! [GOSymbols addText]
void GOSymbols::addText(GraphicsOverlay* graphicsOverlay)
{
  // text symbol
  const float size = 10.0f;
  TextSymbol* textSymbolBassRock = new TextSymbol(QString("Bass Rock"), QColor("blue"), size, HorizontalAlignment::Left, VerticalAlignment::Bottom, this);
  TextSymbol* textSymbolCraigleith = new TextSymbol(QString("Craigleith"), QColor("blue"), size, HorizontalAlignment::Right, VerticalAlignment::Top, this);

  // geometry for the graphics
  Point bassRock(-2.640631, 56.078083, SpatialReference::wgs84());
  Point craigleith(-2.720324, 56.073569, SpatialReference::wgs84());

  // create the graphics and set their symbology
  Graphic* graphicBass = new Graphic(bassRock, this);
  graphicBass->setSymbol(textSymbolBassRock);

  Graphic* graphicCraig = new Graphic(craigleith, this);
  graphicCraig->setSymbol(textSymbolCraigleith);

  // add the graphics to the overlay
  graphicsOverlay->graphics()->append(graphicBass);
  graphicsOverlay->graphics()->append(graphicCraig);
}
//! [GOSymbols addText]

//! [GOSymbols createNestingGround]
// create the polygon geometry
Geometry GOSymbols::createNestingGround()
{
  PolygonBuilder nestingGroundPolygonBuilder(SpatialReference::wgs84());

  // create the polygon using the builder class
  nestingGroundPolygonBuilder.addPoint(-2.643077012566659, 56.077125346044475);
  nestingGroundPolygonBuilder.addPoint(-2.6428195210159444, 56.07717324600376);
  nestingGroundPolygonBuilder.addPoint(-2.6425405718360033, 56.07774804087097);
  nestingGroundPolygonBuilder.addPoint(-2.6427122328698127, 56.077927662508635);
  nestingGroundPolygonBuilder.addPoint(-2.642454741319098, 56.07829887790651);
  nestingGroundPolygonBuilder.addPoint(-2.641853927700763, 56.078526395253725);
  nestingGroundPolygonBuilder.addPoint(-2.6409741649024867, 56.078801809192434);
  nestingGroundPolygonBuilder.addPoint(-2.6399871139580795, 56.07881378366685);
  nestingGroundPolygonBuilder.addPoint(-2.6394077579689705, 56.07908919555142);
  nestingGroundPolygonBuilder.addPoint(-2.638764029092183, 56.07917301616904);
  nestingGroundPolygonBuilder.addPoint(-2.638485079912242, 56.07896945149566);
  nestingGroundPolygonBuilder.addPoint(-2.638570910429147, 56.078203080726844);
  nestingGroundPolygonBuilder.addPoint(-2.63878548672141, 56.077568418396);
  nestingGroundPolygonBuilder.addPoint(-2.6391931816767085, 56.077197195961084);
  nestingGroundPolygonBuilder.addPoint(-2.6399441986996273, 56.07675411934114);
  nestingGroundPolygonBuilder.addPoint(-2.6406523004640934, 56.076730169108444);
  nestingGroundPolygonBuilder.addPoint(-2.6406737580933193, 56.07632301287509);
  nestingGroundPolygonBuilder.addPoint(-2.6401802326211157, 56.075999679860494);
  nestingGroundPolygonBuilder.addPoint(-2.6402446055087943, 56.075844000034046);
  nestingGroundPolygonBuilder.addPoint(-2.640416266542604, 56.07578412301025);
  nestingGroundPolygonBuilder.addPoint(-2.6408883343855822, 56.075808073830935);
  nestingGroundPolygonBuilder.addPoint(-2.6417680971838577, 56.076239186057734);
  nestingGroundPolygonBuilder.addPoint(-2.642197249768383, 56.076251161328514);
  nestingGroundPolygonBuilder.addPoint(-2.6428409786451708, 56.07661041772168);
  nestingGroundPolygonBuilder.addPoint(-2.643077012566659, 56.077125346044475);

  return nestingGroundPolygonBuilder.toGeometry();
}
//! [GOSymbols createNestingGround]
