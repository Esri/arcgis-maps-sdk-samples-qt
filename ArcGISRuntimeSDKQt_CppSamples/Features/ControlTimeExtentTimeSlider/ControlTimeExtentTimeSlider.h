<<<<<<< HEAD
// [WriteFile Name=ControlTimeExtentTimeSlider, Category=Features]
// [Legal]
// Copyright 2021 Esri.
=======
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
// [WriteFile Name=NavigateARouteWithRerouting, Category=Routing]
// [Legal]
// Copyright 2022 Esri.
=======
// [WriteFile Name=ControlTimeExtentTimeSlider, Category=Features]
// [Legal]
// Copyright 2021 Esri.
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd

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

<<<<<<< HEAD
=======
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

#include <QObject>

class QTextToSpeech;
class NavigateRouteSpeaker : public QObject
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
#ifndef CONTROLTIMEEXTENTTIMESLIDER_H
#define CONTROLTIMEEXTENTTIMESLIDER_H

namespace Esri::ArcGISRuntime
{
class Map;
class MapQuickView;
}

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class ControlTimeExtentTimeSlider : public QObject
<<<<<<< HEAD
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
<<<<<<< HEAD
=======
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(const QString& text);
  Q_INVOKABLE bool textToSpeechEngineReady() const;
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
  explicit ControlTimeExtentTimeSlider(QObject* parent = nullptr);
  ~ControlTimeExtentTimeSlider();

  static void init();
<<<<<<< HEAD
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd

signals:
  void mapViewChanged();

private:
<<<<<<< HEAD
=======
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
  QTextToSpeech* m_speaker = nullptr;
};

#endif // NAVIGATEROUTESPEAKER_H
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // CONTROLTIMEEXTENTTIMESLIDER_H
<<<<<<< HEAD
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
