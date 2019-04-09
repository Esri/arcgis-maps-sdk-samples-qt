// [WriteFile Name=GODictionaryRenderer_3D, Category=DisplayInformation]
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

#ifndef GraphicsOverlayDictionaryRenderer_3D_H
#define GraphicsOverlayDictionaryRenderer_3D_H

#include <QQuickItem>
#include <QXmlStreamReader>

#include "Error.h"
#include "Envelope.h"
#include "SpatialReference.h"

namespace Esri
{
  namespace ArcGISRuntime
  {
    class GraphicsOverlay;
    class SceneQuickView;
  }
}

class GODictionaryRenderer_3D : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
  explicit GODictionaryRenderer_3D(QQuickItem* parent = nullptr);
  ~GODictionaryRenderer_3D() override;

  void componentComplete() override;
  static void init();

signals:
  void graphicsLoaded();
  void errorMessageChanged();

private slots:
  void logError(Esri::ArcGISRuntime::Error error);

private:
  static const QString FIELD_CONTROL_POINTS;
  static const QString FIELD_WKID;

  void parseXmlFile();
  void createGraphic(QVariantMap rawAttributes);
  void zoomToGraphics();
  QString errorMessage() const;
  void setErrorMessage(const QString& msg);

  QString m_dataPath;
  QXmlStreamReader m_xmlParser;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Envelope m_bbox;
  QString m_errorMsg;
};

#endif // GraphicsOverlayDictionaryRenderer_3D_H
