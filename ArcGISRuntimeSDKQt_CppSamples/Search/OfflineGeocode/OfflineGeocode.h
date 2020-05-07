// [WriteFile Name=OfflineGeocode, Category=Search]
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

#ifndef OFFLINEGEOCODE_H
#define OFFLINEGEOCODE_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class Graphic;
    class LocatorTask;
    class CalloutData;
    class MapQuickView;
    class GraphicsOverlay;
    class ArcGISTiledLayer;
    class SuggestListModel;
  }
}

#include "Point.h"
#include "Error.h"
#include "SuggestResult.h"
#include "GeocodeParameters.h"
#include "ReverseGeocodeParameters.h"

#include <QQuickItem>

class OfflineGeocode : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::SuggestListModel* suggestions READ suggestions NOTIFY suggestionsChanged)
  Q_PROPERTY(bool geocodeInProgress READ geocodeInProgress NOTIFY geocodeInProgressChanged)
  Q_PROPERTY(bool suggestInProgress READ suggestInProgress NOTIFY suggestInProgressChanged)
  Q_PROPERTY(bool noResults READ noResults NOTIFY noResultsChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit OfflineGeocode(QQuickItem* parent = nullptr);
  ~OfflineGeocode() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void geocodeWithSuggestion(int index);
  Q_INVOKABLE void geocodeWithText(const QString& address);
  Q_INVOKABLE void setSuggestionsText(const QString& searchText);

signals:
  void noResultsChanged();
  void suggestionsChanged();
  void suggestInProgressChanged();
  void geocodeInProgressChanged();
  void dismissSuggestions();
  void errorMessageChanged();

private slots:
  void logError(const Esri::ArcGISRuntime::Error error);

private:
  Esri::ArcGISRuntime::SuggestListModel* suggestions() const;
  bool geocodeInProgress() const;
  bool noResults() const;
  bool suggestInProgress() const;
  void connectSignals();
  QString errorMessage() const;
  void setErrorMessage(const QString& msg);

  bool m_isReverseGeocode = false;
  bool m_geocodeInProgress = false;
  bool m_isPressAndHold = false;
  bool m_noResults = false;
  bool m_suggestInProgress = false;
  QString m_dataPath;
  QString m_errorMsg;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::Point m_clickedPoint;
  Esri::ArcGISRuntime::Graphic* m_pinGraphic = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::CalloutData* m_calloutData = nullptr;
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask = nullptr;
  Esri::ArcGISRuntime::ArcGISTiledLayer* m_tiledLayer = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::SuggestListModel* m_suggestListModel = nullptr;
  Esri::ArcGISRuntime::GeocodeParameters m_geocodeParameters;
  Esri::ArcGISRuntime::ReverseGeocodeParameters m_reverseGeocodeParameters;
};

#endif // OFFLINEGEOCODE_H
