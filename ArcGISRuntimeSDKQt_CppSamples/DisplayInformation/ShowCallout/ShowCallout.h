// [WriteFile Name=ShowCallout, Category=DisplayInformation]
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

#ifndef SHOWCALLOUT_H
#define SHOWCALLOUT_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
    class CalloutData;
  }
}

#include <QQuickItem>

class ShowCallout : public QQuickItem
{
  Q_OBJECT
  //! [expose callout property]
  Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* calloutData READ calloutData NOTIFY calloutDataChanged)
  //! [expose callout property]

public:
  explicit ShowCallout(QQuickItem* parent = nullptr);
  ~ShowCallout() override;

  void componentComplete() override;
  static void init();

signals:
  void calloutDataChanged();

private:
  Esri::ArcGISRuntime::CalloutData* calloutData() const;

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::CalloutData* m_calloutData = nullptr;
};

#endif // SHOWCALLOUT_H
