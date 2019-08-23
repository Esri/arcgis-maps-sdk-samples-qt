// [WriteFile Name=IdentifyLayers, Category=Maps]
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

#ifndef IDENTIFYLAYERS_H
#define IDENTIFYLAYERS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QQuickItem>

class IdentifyLayers : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)

public:
  explicit IdentifyLayers(QQuickItem* parent = nullptr);
  ~IdentifyLayers() override = default;

  void componentComplete() override;
  static void init();

signals:
  void messageChanged();
  void showMessage();

private:
  void connectSignals();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QString m_message;
};

#endif // IDENTIFYLAYERS_H
