// [WriteFile Name=ReadSymbolsFromMobileStyle, Category=DisplayInformation]
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

#ifndef READSYMBOLSFROMMOBILESTYLE_H
#define READSYMBOLSFROMMOBILESTYLE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class MultilayerPointSymbol;
class SymbolStyle;
class SymbolStyleSearchResultListModel;
}
}

class SymbolImageProvider;
class QAbstractListModel;

#include <QObject>
#include <QUuid>
#include <QColor>
#include <QUrl>
#include <QList>
#include <QScopedPointer>

class ReadSymbolsFromMobileStyle : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* hatResults READ hatResults NOTIFY symbolResultsChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* mouthResults READ mouthResults NOTIFY symbolResultsChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* eyesResults READ eyeResults NOTIFY symbolResultsChanged)
  Q_PROPERTY(QUrl symbolImageUrl MEMBER m_symbolImageUrl NOTIFY symbolImageUrlChanged)

public:
  explicit ReadSymbolsFromMobileStyle(QObject* parent = nullptr);
  ~ReadSymbolsFromMobileStyle();

  static void init();

  Q_INVOKABLE void clearGraphics();
  Q_INVOKABLE void updateSymbol(int hatIndex, int mouthIndex, int eyeIndex, QColor color, int size);

signals:
  void mapViewChanged();
  void symbolResultsChanged();
  void symbolImageUrlChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* hatResults() const;
  Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* mouthResults() const;
  Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* eyeResults() const;
  Esri::ArcGISRuntime::SymbolStyleSearchResultListModel* faceResults() const;

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::MultilayerPointSymbol* m_currentSymbol = nullptr;
  Esri::ArcGISRuntime::SymbolStyle* m_symbolStyle = nullptr;
  SymbolImageProvider* m_symbolImageProvider = nullptr;
  QList<Esri::ArcGISRuntime::SymbolStyleSearchResultListModel*> m_models = { nullptr, nullptr, nullptr, nullptr };
  QList<QUuid> m_taskIds;
  QColor m_currentColor = QColor(Qt::yellow);
  int m_symbolSize = 40;
  QUrl m_symbolImageUrl;
  QScopedPointer<QObject> m_graphicParent;
};

#endif // READSYMBOLSFROMMOBILESTYLE_H
