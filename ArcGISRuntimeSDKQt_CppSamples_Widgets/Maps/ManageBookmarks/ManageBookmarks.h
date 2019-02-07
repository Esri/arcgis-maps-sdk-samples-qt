// [WriteFile Name=ManageBookmarks, Category=Maps]
// [Legal]
// Copyright 2015 Esri.

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

#ifndef MANAGE_BOOKMARKS_H
#define MANAGE_BOOKMARKS_H

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapGraphicsView;
  }
}

class QComboBox;
class QPushButton;
class QInputDialog;

#include "Viewpoint.h"
#include <QWidget>
#include <QMap>

class ManageBookmarks : public QWidget
{
  Q_OBJECT

public:
  explicit ManageBookmarks(QWidget* parent = nullptr);
  ~ManageBookmarks() override;

private:
  void createUi();
  void createInitialBookmarks();
  void createBookmark(QString name, Esri::ArcGISRuntime::Viewpoint viewpoint);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
  QComboBox* m_bookmarkCombo = nullptr;
  QPushButton* m_button = nullptr;
  QInputDialog* m_inputDialog = nullptr;
  QMap<QString, Esri::ArcGISRuntime::Viewpoint> m_bookmarks;
};

#endif // MANAGE_BOOKMARKS_H
