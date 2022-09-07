#include "NavigateRouteSpeaker.h"

// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// #include <QTextToSpeech>

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
  // return m_speaker->state() == QTextToSpeech::State::Ready;
}
