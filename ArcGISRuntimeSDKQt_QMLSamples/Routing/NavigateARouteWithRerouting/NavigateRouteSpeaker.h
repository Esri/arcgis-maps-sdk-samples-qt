<<<<<<< HEAD
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
// [WriteFile Name=NavigateARouteWithRerouting, Category=Routing]
// [Legal]
// Copyright 2022 Esri.
=======
// [WriteFile Name=ControlTimeExtentTimeSlider, Category=Features]
// [Legal]
// Copyright 2021 Esri.
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
=======
// [WriteFile Name=NavigateARouteWithRerouting, Category=Routing]
// [Legal]
// Copyright 2022 Esri.
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
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

#include <QObject>

<<<<<<< HEAD
class QTextToSpeech;
class NavigateRouteSpeaker : public QObject
=======
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
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
=======
// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// class QTextToSpeech;
class NavigateRouteSpeaker : public QObject
{
  Q_OBJECT

public:
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(const QString& text);
  Q_INVOKABLE bool textToSpeechEngineReady() const;
<<<<<<< HEAD
=======
  explicit ControlTimeExtentTimeSlider(QObject* parent = nullptr);
  ~ControlTimeExtentTimeSlider();

  static void init();
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h

signals:
  void mapViewChanged();

private:
<<<<<<< HEAD:ArcGISRuntimeSDKQt_QMLSamples/Routing/NavigateARouteWithRerouting/NavigateRouteSpeaker.h
  QTextToSpeech* m_speaker = nullptr;
};

#endif // NAVIGATEROUTESPEAKER_H
=======
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
};

#endif // CONTROLTIMEEXTENTTIMESLIDER_H
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd:ArcGISRuntimeSDKQt_CppSamples/Features/ControlTimeExtentTimeSlider/ControlTimeExtentTimeSlider.h
=======

  // private:
  //   QTextToSpeech* m_speaker = nullptr;
};

#endif // NAVIGATEROUTESPEAKER_H
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
