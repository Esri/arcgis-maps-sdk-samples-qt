// [WriteFile Name=OpenMapUrl, Category=Maps]
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

#ifndef OPEN_MAP_URL_H
#define OPEN_MAP_URL_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class MapQuickView;
  }
}

class QString;

#include <QQuickItem>

class OpenMapUrl : public QQuickItem
{
  Q_OBJECT

public:
  explicit OpenMapUrl(QQuickItem* parent = nullptr);
  ~OpenMapUrl() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void openMap(const QString& itemId);

private:
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // OPEN_MAP_URL_H

