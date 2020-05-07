// [WriteFile Name=ControlAnnotationSublayerVisibility, Category=DisplayInformation]
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

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "ControlAnnotationSublayerVisibility.h"

#include "Map.h"
#include "MapQuickView.h"
#include "MobileMapPackage.h"
#include "AnnotationLayer.h"
#include "AnnotationSublayer.h"

#include <QDir>
#include <QtCore/qglobal.h>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}
} // namespace

// sample MMPK location
const QString sampleFileAnno {"/ArcGIS/Runtime/Data/mmpk/GasDeviceAnno.mmpk"};

ControlAnnotationSublayerVisibility::ControlAnnotationSublayerVisibility(QObject* parent /* = nullptr */):
  QObject(parent)
{
  const QString dataPath = defaultDataPath() + sampleFileAnno;

  // connect to the Mobile Map Package instance to know when errors occur
  connect(MobileMapPackage::instance(), &MobileMapPackage::errorOccurred,
          [](Error error)
  {
    if (error.isEmpty())
      return;

    qDebug() << QString("Error: %1 %2").arg(error.message(), error.additionalMessage());
  });

  // Load the MMPK
  createMapPackage(dataPath);
}

ControlAnnotationSublayerVisibility::~ControlAnnotationSublayerVisibility() = default;

void ControlAnnotationSublayerVisibility::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<ControlAnnotationSublayerVisibility>("Esri.Samples", 1, 0, "ControlAnnotationSublayerVisibilitySample");
}

MapQuickView* ControlAnnotationSublayerVisibility::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void ControlAnnotationSublayerVisibility::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
    return;

  m_mapView = mapView;
  m_mapView->setMap(m_map);

  connect(m_mapView, &MapQuickView::mapScaleChanged, this, [this]()
  {
    m_mapScale = m_mapView->mapScale();
    emit mapScaleChanged();

    if (!m_annotationSubLayerOpen)
      return;

    m_visibleAtCurrentExtent = m_annotationSubLayerOpen->isVisibleAtScale(m_mapScale);
    visibleAtCurrentExtentChanged();
  });

  emit mapViewChanged();
}

// create map package
void ControlAnnotationSublayerVisibility::createMapPackage(const QString& path)
{
  //! [open mobile map package cpp snippet]
  // instatiate a mobile map package
  m_mobileMapPackage = new MobileMapPackage(path, this);

  // wait for the mobile map package to load
  connect(m_mobileMapPackage, &MobileMapPackage::doneLoading, this, [this](Error error)
  {
    if (!error.isEmpty())
    {
      qDebug() << QString("Package load error: %1 %2").arg(error.message(), error.additionalMessage());
      return;
    }

    if (!m_mobileMapPackage || !m_mapView || m_mobileMapPackage->maps().isEmpty())
      return;

    // The package contains a list of maps that could be shown in the UI for selection.
    // For simplicity, obtain the first map in the list of maps.
    // set the map on the map view to display
    m_mapView->setMap(m_mobileMapPackage->maps().at(0));

    m_layerListModel = mapView()->map()->operationalLayers();
    for (Layer* layer : *m_layerListModel)
    {
      if (layer->layerType() == LayerType::AnnotationLayer)
      {
        m_annoLayer = layer;
        connect(m_annoLayer, &Layer::doneLoading, this, [this](Error error)
        {
          if (!error.isEmpty())
          {
            qDebug() << QString("Package load error: %1 %2").arg(error.message(), error.additionalMessage());
            return;
          }

          m_annotationSubLayerClosed = dynamic_cast<AnnotationSublayer*>(m_annoLayer->subLayerContents()[0]);
          m_annotationSubLayerOpen = dynamic_cast<AnnotationSublayer*>(m_annoLayer->subLayerContents()[1]);
          m_closedLayerText = m_annotationSubLayerClosed->name();
          m_openLayerText = QString("%1 (1:%2 - 1:%3)").arg(m_annotationSubLayerOpen->name()).arg(m_annotationSubLayerOpen->maxScale()).arg(m_annotationSubLayerOpen->minScale());
          emit openLayerTextChanged();
          emit closedLayerTextChanged();
        });
        layer->load();
      }
    }
  });

  m_mobileMapPackage->load();
  //! [open mobile map package cpp snippet]
}

void ControlAnnotationSublayerVisibility::openLayerVisible()
{
  if (!m_annotationSubLayerOpen)
    return;

  m_annotationSubLayerOpen->setVisible(!m_annotationSubLayerOpen->isVisible());
}

void ControlAnnotationSublayerVisibility::closedLayerVisible()
{
  if (!m_annotationSubLayerClosed)
    return;

  m_annotationSubLayerClosed->setVisible(!m_annotationSubLayerClosed->isVisible());
}
