// [Legal]
// Copyright 2022 Esri.

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
/// \file pch.hpp

#ifndef SAMPLEAPPS_PCH_HPP
#define SAMPLEAPPS_PCH_HPP

#include <QtCore/qglobal.h>

#ifdef CPP_VIEWER
#include "ArcGISTiledElevationSource.h"
#include "Basemap.h"
#include "Envelope.h"
#include "FeatureLayer.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"
#include "Polygon.h"
#include "Polyline.h"
#include "PortalItem.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "ServiceFeatureTable.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "Viewpoint.h"
#endif // CPP_VIEWER

#include <QAbstractListModel>
#include <QByteArray>
#include <QDir>
#include <QFuture>
#include <QGuiApplication>
#include <QHash>
#include <QList>
#include <QObject>
#include <QQmlEngine>
#include <QQmlProperties>
#include <QQuickItem>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QVariantMap>

#ifdef Q_OS_IOS
#include <QStandardPaths>
#endif // Q_OS_IOS

#ifndef Q_OS_WIN
// this brings in WinGDI headers on Windows which has its own Polygon and Polyline
// and causes build errors. You can fix it by adding Esri::ArcGISRuntime namespaces
// but that makes some samples inconsistent
#include <QQuickView>
#endif // Q_OS_WIN

#endif // SAMPLEAPPS_PCH_HPP
