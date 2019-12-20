// [WriteFile Name=TakeScreenshot, Category=Maps]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "TakeScreenshot.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MapImageProvider.h"

#include <QQmlContext>

using namespace Esri::ArcGISRuntime;

TakeScreenshot::TakeScreenshot(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void TakeScreenshot::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<TakeScreenshot>("Esri.Samples", 1, 0, "TakeScreenshotSample");
}

void TakeScreenshot::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the imagery basemap
  m_map = new Map(Basemap::imagery(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Get the image provider from the QML Engine
  QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
  m_imageProvider = dynamic_cast<MapImageProvider*>(engine->imageProvider(MapImageProvider::imageProviderId()));

  // Connect to the exportImageCompleted signal
  connect(m_mapView, &MapQuickView::exportImageCompleted, this, [this](QUuid id, QImage img)
  {
    // convert the QUuid into a QString
    const QString imageId = id.toString().remove("{").remove("}");
    // add the image to the provider
    m_imageProvider->addImage(imageId, img);
    // update the URL with the unique id
    m_mapImageUrl = QString("image://%1/%2").arg(MapImageProvider::imageProviderId(), imageId);
    // emit the signal to trigger the QML Image to update
    emit mapImageUrlChanged();
  });
}

// Q_INVOKABLE function to kick off the export image asynchronous task
void TakeScreenshot::captureScreenshot()
{
  m_mapView->exportImage();
}

QUrl TakeScreenshot::mapImageUrl() const
{
  return m_mapImageUrl;
}
