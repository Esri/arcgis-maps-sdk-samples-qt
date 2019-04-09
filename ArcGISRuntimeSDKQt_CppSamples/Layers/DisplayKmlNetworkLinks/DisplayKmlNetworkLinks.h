// [WriteFile Name=DisplayKmlNetworkLinks, Category=Layers]
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

#ifndef DISPLAYKMLNETWORKLINKS_H
#define DISPLAYKMLNETWORKLINKS_H

namespace Esri
{
namespace ArcGISRuntime
{
class SceneQuickView;
}
}

#include <QQuickItem>

class DisplayKmlNetworkLinks : public QQuickItem
{
  Q_PROPERTY(QString currentKmlNetworkMessage READ currentKmlNetworkMessage WRITE setCurrentKmlNetworkMessage NOTIFY kmlMessageRecieved)
  Q_OBJECT

public:
  explicit DisplayKmlNetworkLinks(QQuickItem* parent = nullptr);
  ~DisplayKmlNetworkLinks() override;

  void componentComplete() override;
  static void init();

  QString currentKmlNetworkMessage() const;

signals:
  void kmlMessageRecieved(const QString& message);

private:
  void setCurrentKmlNetworkMessage(const QString& message);

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  QString m_currentKmlNetworkMessage;
};

#endif // DISPLAYKMLNETWORKLINKS_H
