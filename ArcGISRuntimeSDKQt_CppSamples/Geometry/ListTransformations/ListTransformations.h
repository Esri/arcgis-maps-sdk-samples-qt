// [WriteFile Name=ListTransformations, Category=Geometry]
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

#ifndef LISTTRANSFORMATIONS_H
#define LISTTRANSFORMATIONS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class DatumTransformation;
}
}

#include "Point.h"

#include <QQuickItem>
#include <QList>
#include <QVariantList>

class ListTransformations : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QVariantList transformationList MEMBER m_transformationList NOTIFY transformationListChanged)

public:
  explicit ListTransformations(QQuickItem* parent = nullptr);
  ~ListTransformations() override = default;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void refreshTransformationList(bool orderBySuitability);
  Q_INVOKABLE void updateGraphicTransformation(int index);

signals:
  void transformationListChanged();
  void showStatusBar(QString message = "");

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Point m_originalPoint;
  Esri::ArcGISRuntime::Graphic* m_originalGraphic = nullptr;
  Esri::ArcGISRuntime::Graphic* m_projectedGraphic = nullptr;
  QList<Esri::ArcGISRuntime::DatumTransformation*> m_transformations;
  QVariantList m_transformationList;

  void addGraphics();
};

#endif // LISTTRANSFORMATIONS_H
