#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// class QTextToSpeech;

#include <QObject>

class NavigateRouteSpeaker : public QObject
{
  Q_OBJECT

public:
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(const QString& text);
  Q_INVOKABLE bool textToSpeechEngineReady() const;

// private:
//   QTextToSpeech* m_speaker = nullptr;
};

#endif // NAVIGATEROUTESPEAKER_H
