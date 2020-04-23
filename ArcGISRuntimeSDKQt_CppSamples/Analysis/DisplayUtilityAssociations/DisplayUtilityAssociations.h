// [WriteFile Name=DisplayUtilityAssociations, Category=Analysis]
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

#ifndef DISPLAYUTILITYASSOCIATIONS_H
#define DISPLAYUTILITYASSOCIATIONS_H

#include "UtilityNetworkTypes.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GraphicsOverlay;
class Map;
class MapQuickView;
class Symbol;
class UtilityNetwork;
}
}

class SymbolImageProvider;

#include <QObject>

class DisplayUtilityAssociations : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(QString attachmentSymbolUrl READ attachmentSymbolUrl NOTIFY attachmentSymbolUrlChanged)
  Q_PROPERTY(QString connectivitySymbolUrl READ connectivitySymbolUrl NOTIFY connectivitySymbolUrlChanged)
  Q_PROPERTY(bool swatchesCompleted READ swatchesCompleted NOTIFY swatchesCompletedChanged)

public:
  explicit DisplayUtilityAssociations(QObject* parent = nullptr);
  ~DisplayUtilityAssociations();

  static void init();

signals:
  void mapViewChanged();
  void attachmentSymbolUrlChanged();
  void connectivitySymbolUrlChanged();
  void swatchesCompletedChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void addAssociations();
  QString attachmentSymbolUrl() const { return m_attachmentSymbolUrl; }
  QString connectivitySymbolUrl() const { return m_connectivitySymbolUrl; }
  bool swatchesCompleted() const { return m_swatchesCompleted; }

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_associationsOverlay = nullptr;
  Esri::ArcGISRuntime::UtilityNetwork* m_utilityNetwork = nullptr;
  Esri::ArcGISRuntime::Symbol* m_attachmentSymbol = nullptr;
  Esri::ArcGISRuntime::Symbol* m_connectivitySymbol = nullptr;
  QString m_attachmentSymbolUrl = "";
  QString m_connectivitySymbolUrl = "";
  SymbolImageProvider* m_symbolImageProvider = nullptr;
  bool m_swatchesCompleted = false;
};

#endif // DISPLAYUTILITYASSOCIATIONS_H
