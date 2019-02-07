// [WriteFile Name=OpenExistingMap, Category=Maps]
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

#ifndef OPEN_EXISTING_MAP_H
#define OPEN_EXISTING_MAP_H

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class MapGraphicsView;
  }
}

class QPushButton;
class QInputDialog;

#include "PortalItem.h"
#include <QWidget>
#include <QMap>

class OpenExistingMap : public QWidget
{
  Q_OBJECT

public:
  explicit OpenExistingMap(QWidget* parent = nullptr);
  ~OpenExistingMap() override;

private:
  void createUi();
  void createPortalMaps();

private:
  Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
  QPushButton* m_button = nullptr;
  QInputDialog* m_inputDialog = nullptr;
  QMap<QString, QString> m_portalIds;

};

#endif // OPEN_EXISTING_MAP_H
