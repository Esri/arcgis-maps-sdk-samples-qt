// [WriteFile Name=ManageBookmarks, Category=Maps]
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

#include "ManageBookmarks.h"

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "SpatialReference.h"
#include "Envelope.h"
#include "Viewpoint.h"
#include "Bookmark.h"

using namespace Esri::ArcGISRuntime;

ManageBookmarks::ManageBookmarks(QQuickItem* parent) :
  QQuickItem(parent)
{
}

ManageBookmarks::~ManageBookmarks() = default;

void ManageBookmarks::init()
{
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ManageBookmarks>("Esri.Samples", 1, 0, "ManageBookmarksSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

void ManageBookmarks::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create a new basemap instance
  Basemap* basemap = Basemap::imageryWithLabels(this);
  // create a new map instance
  m_map = new Map(basemap, this);
  // set map on the map view
  m_mapView->setMap(m_map);

  // create the bookmarks once the map is loaded
  connect(m_map, &Esri::ArcGISRuntime::Map::loadStatusChanged, this, [this](Esri::ArcGISRuntime::LoadStatus loadStatus)
  {
    if (loadStatus == LoadStatus::Loaded)
    {
      m_bookmarks = m_map->bookmarks();
      createInitialBookmarks();
    }
  });
}

void ManageBookmarks::createInitialBookmarks()
{
  // Mysterious Desert Pattern
  Envelope env1(3742993.127298778, 3170396.4675719286, 3744795.1333054285, 3171745.88077, SpatialReference(102100));
  Viewpoint viewpoint1(env1);
  createBookmark("Mysterious Desert Pattern", viewpoint1);

  // Strange Symbol
  Envelope env2(-13009913.860076642, 4495026.9307899885, -13009442.089218518, 4495404.031910696, SpatialReference(102100));
  Viewpoint viewpoint2(env2);
  createBookmark("Strange Symbol", viewpoint2);

  // Guitar-Shaped Forest
  Envelope env3(-7124497.45137465, -4012221.6124684606, -7121131.417429369, -4009697.0870095, SpatialReference(102100));
  Viewpoint viewpoint3(env3);
  createBookmark("Guitar-Shaped Forest", viewpoint3);

  // Grand Prismatic Spring
  Envelope env4(-12338668.348591767, 5546908.424239618, -12338247.594362013, 5547223.989911933, SpatialReference(102100));
  Viewpoint viewpoint4(env4);
  createBookmark("Grand Prismatic Spring", viewpoint4);
}

void ManageBookmarks::createBookmark(QString name, Viewpoint viewpoint)
{
  //! [Add bookmarks to the list model]
  // Create the bookmark from the name and viewpoint
  Bookmark* bookmark = new Bookmark(name, viewpoint, this);

  // Add it to the map's bookmark list
  BookmarkListModel* bookmarks = dynamic_cast<BookmarkListModel*>(m_bookmarks);
  if (!bookmarks)
    return;

  bookmarks->append(bookmark);

  // emit that model has changed
  emit bookmarksChanged();
  //! [Add bookmarks to the list model]
}

void ManageBookmarks::goToBookmark(int bookmarkIndex)
{
  BookmarkListModel* bookmarks = dynamic_cast<BookmarkListModel*>(m_bookmarks);
  if (!bookmarks)
    return;

  m_mapView->setViewpoint(bookmarks->at(bookmarkIndex)->viewpoint());
}

QAbstractListModel* ManageBookmarks::bookmarks() const
{
  return m_bookmarks;
}

void ManageBookmarks::addBookmark(QString newBookmarkName)
{
  createBookmark(newBookmarkName, m_mapView->currentViewpoint(ViewpointType::BoundingGeometry));
}

QString ManageBookmarks::bookmarkNameForIndex(int index) const
{
  return m_bookmarks->data(m_bookmarks->index(index),
                           BookmarkListModel::BookmarkNameRole).toString();
}
