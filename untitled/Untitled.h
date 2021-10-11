// Copyright 2021 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef UNTITLED_H
#define UNTITLED_H

namespace Esri
{
namespace ArcGISRuntime
{
class MapQuickView;
}
}

#include <QObject>

class Untitled : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
  explicit Untitled(QObject* parent = nullptr);
  ~Untitled() override;

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void setupMapFromMmpk(QString pathToMmpk);

  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // UNTITLED_H
