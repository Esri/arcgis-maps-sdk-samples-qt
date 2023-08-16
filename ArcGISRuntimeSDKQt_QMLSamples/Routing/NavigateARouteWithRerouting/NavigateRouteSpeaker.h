// [WriteFile Name=NavigateARouteWithRerouting, Category=Routing]
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

#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

#include <QObject>

<<<<<<< HEAD
// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// class QTextToSpeech;
=======
class QTextToSpeech;
>>>>>>> v.next
class NavigateRouteSpeaker : public QObject
{
  Q_OBJECT

public:
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(const QString& text);
  Q_INVOKABLE bool textToSpeechEngineReady() const;

<<<<<<< HEAD
  // private:
  //   QTextToSpeech* m_speaker = nullptr;
=======
private:
  QTextToSpeech* m_speaker = nullptr;
>>>>>>> v.next
};

#endif // NAVIGATEROUTESPEAKER_H
