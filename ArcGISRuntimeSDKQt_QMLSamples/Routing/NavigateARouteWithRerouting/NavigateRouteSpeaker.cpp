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

#include "NavigateRouteSpeaker.h"
<<<<<<< HEAD
#include <QtTextToSpeech/QTextToSpeech>

NavigateRouteSpeaker::NavigateRouteSpeaker(QObject* parent):
  QObject(parent),
  m_speaker(new QTextToSpeech(this))
=======

// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// #include <QTextToSpeech>

NavigateRouteSpeaker::NavigateRouteSpeaker(QObject* parent):
  QObject(parent)/*,
  m_speaker(new QTextToSpeech(this))*/
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
{
}

NavigateRouteSpeaker::~NavigateRouteSpeaker() = default;

void NavigateRouteSpeaker::textToSpeech(const QString& text)
{
<<<<<<< HEAD
  m_speaker->say(text);
=======
  // m_speaker->say(text);
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
}

bool NavigateRouteSpeaker::textToSpeechEngineReady() const
{
<<<<<<< HEAD
  return m_speaker->state() == QTextToSpeech::State::Ready;
=======
  return false;
  // return m_speaker->state() == QTextToSpeech::State::Ready;
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
}
