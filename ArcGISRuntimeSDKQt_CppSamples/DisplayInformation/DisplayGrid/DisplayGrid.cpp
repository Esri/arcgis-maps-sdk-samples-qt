// [WriteFile Name=DisplayGrid, Category=DisplayInformation]
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

#include "DisplayGrid.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Grid.h"
#include "LatitudeLongitudeGrid.h"
#include "UTMGrid.h"
#include "USNGGrid.h"
#include "MGRSGrid.h"
#include "SimpleLineSymbol.h"
#include "TextSymbol.h"
#include "Viewpoint.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

const QString DisplayGrid::s_utmGrid = QStringLiteral("UTM");
const QString DisplayGrid::s_usngGrid = QStringLiteral("USNG");
const QString DisplayGrid::s_latlonGrid = QStringLiteral("LatLon");
const QString DisplayGrid::s_mgrsGrid = QStringLiteral("MGRS");
const QString DisplayGrid::s_ddFormat = QStringLiteral("Decimal degrees");
const QString DisplayGrid::s_dmsFormat = QStringLiteral( "Degrees minutes seconds");
const QString DisplayGrid::s_geographicPosition = QStringLiteral("Geographic");
const QString DisplayGrid::s_bottomLeftPosition = QStringLiteral("Bottom left");
const QString DisplayGrid::s_bottomRightPosition = QStringLiteral("Bottom right");
const QString DisplayGrid::s_topLeftPosition = QStringLiteral("Top left");
const QString DisplayGrid::s_topRightPosition = QStringLiteral("Top right");
const QString DisplayGrid::s_centerPosition = QStringLiteral("Center");
const QString DisplayGrid::s_allSidesPosition = QStringLiteral("All sides");

DisplayGrid::DisplayGrid(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

DisplayGrid::~DisplayGrid() = default;

void DisplayGrid::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<DisplayGrid>("Esri.Samples", 1, 0, "DisplayGridSample");
}

void DisplayGrid::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the imagery basemap
  m_map = new Map(Basemap::imagery(this), this);
  double targetScale = 6450785;
  m_map->setInitialViewpoint(Viewpoint(Point(-10336141.70018318, 5418213.05332071, SpatialReference::webMercator()), targetScale));

  // Set map to map view
  m_mapView->setMap(m_map);
}

// change the grid type
void DisplayGrid::changeGrid(const QString& gridType)
{
  if (gridType == latlonGrid())
  {
    //! [DisplayGrid Set_LatLon_Grid_Cpp]
    // create a grid for showing Latitude and Longitude (Meridians and Parallels)
    LatitudeLongitudeGrid* latlonGrid = new LatitudeLongitudeGrid(this);
    m_mapView->setGrid(latlonGrid);
    //! [DisplayGrid Set_LatLon_Grid_Cpp]
  }
  else if (gridType == utmGrid())
  {
    UTMGrid* utmGrid = new UTMGrid(this);
    m_mapView->setGrid(utmGrid);
  }
  else if (gridType == usngGrid())
  {
    USNGGrid* utmGrid = new USNGGrid(this);
    m_mapView->setGrid(utmGrid);
  }
  else if (gridType == mgrsGrid())
  {
    MGRSGrid* utmGrid = new MGRSGrid(this);
    m_mapView->setGrid(utmGrid);
  }

  // apply any styling that has been set
  changeGridColor(currentGridColor());
  changeLabelColor(currentLabelColor());
  changeLabelFormat(currentLabelFormat());
  changeLabelPosition(currentLabelPosition());
  setGridLabelVisibility(gridVisibility());
  setGridVisibility(gridVisibility());
}

// change the grid color
void DisplayGrid::changeGridColor(const QString& color)
{
  if (m_mapView->grid())
  {
    // find the number of resolution levels in the grid
    int gridLevels = m_mapView->grid()->levelCount();

    //! [DisplayGrid Set_Grid_Lines_Cpp]
    for (int level = 0; level < gridLevels; level++)
    {
      const float width = 1 + level;
      SimpleLineSymbol* lineSym = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(color), width, this);
      m_mapView->grid()->setLineSymbol(level, lineSym);
    }
    //! [DisplayGrid Set_Grid_Lines_Cpp]
  }
}

// change the grid label color
void DisplayGrid::changeLabelColor(const QString& color)
{
  if (m_mapView->grid())
  {
    //! [DisplayGrid Grid_Levels_Cpp]
    // find the number of resolution levels in the grid
    int gridLevels = m_mapView->grid()->levelCount();
    //! [DisplayGrid Grid_Levels_Cpp]

    //! [DisplayGrid Set_Grid_Labels_Cpp]
    for (int level = 0; level < gridLevels; level++)
    {
      constexpr float size = 14.0f;
      TextSymbol* textSym = new TextSymbol("text", QColor(color), size, HorizontalAlignment::Left, VerticalAlignment::Bottom, this);
      textSym->setHaloColor(QColor("white"));
      textSym->setHaloWidth(2.0f + level);
      m_mapView->grid()->setTextSymbol(level, textSym);
    }
    //! [DisplayGrid Set_Grid_Labels_Cpp]
  }
}

// change the grid label format
void DisplayGrid::changeLabelFormat(const QString& format)
{
  if (m_mapView->grid())
  {
    if (m_mapView->grid()->gridType() == GridType::LatitudeLongitudeGrid)
    {
      if (format == ddFormat())
      {
        // format the labels to display in decimal degrees
        static_cast<LatitudeLongitudeGrid*>(m_mapView->grid())->setLabelFormat(LatitudeLongitudeGridLabelFormat::DecimalDegrees);
      }
      else if (format == dmsFormat())
      {
        //! [DisplayGrid Set_Label_Format_Cpp]
        // format the labels to display in degrees, minutes and seconds
        static_cast<LatitudeLongitudeGrid*>(m_mapView->grid())->setLabelFormat(LatitudeLongitudeGridLabelFormat::DegreesMinutesSeconds);
        //! [DisplayGrid Set_Label_Format_Cpp]
      }
    }
  }
}

// change the grid label placement
void DisplayGrid::changeLabelPosition(const QString& position)
{
  if (m_mapView->grid())
  {
    if (position == geographicPosition())
    {
      //! [DisplayGrid Set_Label_Placement_Geographic_Cpp]
      // update the label positioning to geographic
      m_mapView->grid()->setLabelPosition(GridLabelPosition::Geographic);
      //! [DisplayGrid Set_Label_Placement_Geographic_Cpp]
    }
    else if (position == bottomLeftPosition())
    {
      // update the label positioning to bottom left
      m_mapView->grid()->setLabelPosition(GridLabelPosition::BottomLeft);
    }
    else if (position == bottomRightPosition())
    {
      // update the label positioning to bottom right
      m_mapView->grid()->setLabelPosition(GridLabelPosition::BottomRight);
    }
    else if (position == topLeftPosition())
    {
      // update the label positioning to top left
      m_mapView->grid()->setLabelPosition(GridLabelPosition::TopLeft);
    }
    else if (position == topRightPosition())
    {
      //! [DisplayGrid Set_Label_Placement_Top_Right_Cpp]
      // update the label positioning to top right
      m_mapView->grid()->setLabelPosition(GridLabelPosition::TopRight);
      //! [DisplayGrid Set_Label_Placement_Top_Right_Cpp]
    }
    else if (position == centerPosition())
    {
      // update the label positioning to center
      m_mapView->grid()->setLabelPosition(GridLabelPosition::Center);
    }
    else if (position == allSidesPosition())
    {
      // update the label positioning to all sides
      m_mapView->grid()->setLabelPosition(GridLabelPosition::AllSides);
    }
  }
}

MapQuickView* DisplayGrid::mapQuickView() const
{
  return m_mapView;
}

void DisplayGrid::setMapQuickView(MapQuickView* mapView)
{
  m_mapView = mapView;
  emit mapQuickViewChanged();
}

QString DisplayGrid::currentGridColor() const
{
  return m_currentGridColor;
}

void DisplayGrid::setCurrentGridColor(const QString& gridColor)
{
  m_currentGridColor = gridColor;
  changeGridColor(m_currentGridColor);
}

QString DisplayGrid::currentLabelColor() const
{
  return m_currentLabelColor;
}

void DisplayGrid::setCurrentLabelColor(const QString& labelColor)
{
  m_currentLabelColor = labelColor;
  changeLabelColor(m_currentLabelColor);
}

QString DisplayGrid::currentLabelFormat() const
{
  return m_currentLabelFormat;
}

void DisplayGrid::setCurrentLabelFormat(const QString& labelFormat)
{
  m_currentLabelFormat = labelFormat;
  changeLabelFormat(m_currentLabelFormat);
}

QString DisplayGrid::currentLabelPosition() const
{
  return m_currentLabelPosition;
}

void DisplayGrid::setCurrentLabelPosition(const QString& labelPosition)
{
  m_currentLabelPosition = labelPosition;
  changeLabelPosition(m_currentLabelPosition);
}

bool DisplayGrid::gridVisibility()
{
  return m_gridVisibility;
}

void DisplayGrid::setGridVisibility(bool visible)
{
  m_gridVisibility = visible;

  //! [DisplayGrid Grid_Visible_Cpp]
  m_mapView->grid()->setVisible(m_gridVisibility);
  //! [DisplayGrid Grid_Visible_Cpp]
}

bool DisplayGrid::gridLabelVisibility()
{
  return m_gridLabelVisibility;
}

void DisplayGrid::setGridLabelVisibility(bool visible)
{
  m_gridLabelVisibility = visible;

  //! [DisplayGrid Label_Visible_Cpp]
  m_mapView->grid()->setLabelsVisible(m_gridLabelVisibility);
  //! [DisplayGrid Label_Visible_Cpp]
}
