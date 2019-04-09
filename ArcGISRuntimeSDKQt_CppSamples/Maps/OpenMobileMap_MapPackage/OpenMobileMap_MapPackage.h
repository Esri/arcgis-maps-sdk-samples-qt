// [WriteFile Name=OpenMobileMap_MapPackage, Category=Maps]
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

#ifndef OPEN_MOBILE_MAP_MAP_PACKAGE_H
#define OPEN_MOBILE_MAP_MAP_PACKAGE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Error;
    class Map;
    class MapQuickView;
    class MobileMapPackage;
  }
}

#include <QQuickItem>
#include <QTemporaryDir>

class OpenMobileMap_MapPackage : public QQuickItem
{
  Q_OBJECT

public:
  explicit OpenMobileMap_MapPackage(QQuickItem* parent = nullptr);
  ~OpenMobileMap_MapPackage() override;

  void componentComplete() override;
  static void init();

private:
  void createMapPackage(const QString& path);

  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MobileMapPackage* m_mobileMapPackage = nullptr;
  QTemporaryDir m_unpackTempDir;
};

#endif // OPEN_MOBILE_MAP_MAP_PACKAGE_H

