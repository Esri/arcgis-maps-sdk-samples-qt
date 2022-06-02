#include "NavigateRouteSpeaker.h"
//#include <QTextToSpeech>

NavigateRouteSpeaker::NavigateRouteSpeaker(QObject* parent):
  QObject(parent)/*,
  m_speaker(new QTextToSpeech(this))*/
{
}

NavigateRouteSpeaker::~NavigateRouteSpeaker() = default;

void NavigateRouteSpeaker::textToSpeech(const QString&)
{
//  m_speaker->say(text);
}

bool NavigateRouteSpeaker::textToSpeechEngineReady() const
{
  return false;
//  return m_speaker->state() == QTextToSpeech::State::Ready;
}
