// [WriteFile Name=HonorMobileMapPackageExpiration, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

#ifndef HONORMOBILEMAPPACKAGEEXPIRATION_H
#define HONORMOBILEMAPPACKAGEEXPIRATION_H

namespace Esri
{
namespace ArcGISRuntime
{
class MapQuickView;
class MobileMapPackage;
}
}

#include <QObject>

class HonorMobileMapPackageExpiration : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString expirationString MEMBER m_expirationString NOTIFY expirationStringChanged)

public:
  explicit HonorMobileMapPackageExpiration(QObject* parent = nullptr);
  ~HonorMobileMapPackageExpiration();

  static void init();

signals:
  void mapViewChanged();
  void expirationStringChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void createMapPackage(const QString& path);

  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;      
  Esri::ArcGISRuntime::MobileMapPackage* m_mobileMapPackage = nullptr;

  QString m_dataPath;
  QString m_expirationString;
};

#endif // HONORMOBILEMAPPACKAGEEXPIRATION_H
