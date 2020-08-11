// [WriteFile Name=EditFeaturesWithFeatureLinkedAnnotation, Category=EditData]
// [Legal]
// Copyright 2020 Esri.

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

#ifndef EDITFEATURESWITHFEATURELINKEDANNOTATION_H
#define EDITFEATURESWITHFEATURELINKEDANNOTATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Geodatabase;
class FeatureLayer;
class Feature;
class AnnotationLayer;
//class Point;
}
}

#include <QObject>
#include <QDir>

#include "Point.h"

class EditFeaturesWithFeatureLinkedAnnotation : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit EditFeaturesWithFeatureLinkedAnnotation(QObject* parent = nullptr);
  ~EditFeaturesWithFeatureLinkedAnnotation();

  static void init();

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Geodatabase* m_geodatabase = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_pointFeatureLayer = nullptr;
  Esri::ArcGISRuntime::FeatureLayer* m_parcelLinesFeatureLayer = nullptr;
  Esri::ArcGISRuntime::AnnotationLayer* m_addressPointsAnnotationLayer = nullptr;
  Esri::ArcGISRuntime::AnnotationLayer* m_parcelLinesAnnotationLayer = nullptr;
  Esri::ArcGISRuntime::Feature* m_selectedFeature = nullptr;
  Esri::ArcGISRuntime::Point m_screenClickPoint;

  void clearSelection();
  void movePolylineVertex(Esri::ArcGISRuntime::Point mapPoint);

};

#endif // EDITFEATURESWITHFEATURELINKEDANNOTATION_H
