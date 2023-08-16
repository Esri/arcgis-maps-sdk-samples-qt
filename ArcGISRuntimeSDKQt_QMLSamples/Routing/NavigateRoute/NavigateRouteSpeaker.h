#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

<<<<<<< HEAD
class QTextToSpeech;
=======
// NOTE: As of Qt 6.2, QTextToSpeech is not supported. Instances of this class have been commented out for compatibility, but remain for reference
// class QTextToSpeech;
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd

#include <QObject>

class NavigateRouteSpeaker : public QObject
{
  Q_OBJECT

public:
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(const QString& text);
  Q_INVOKABLE bool textToSpeechEngineReady() const;

<<<<<<< HEAD
private:
  QTextToSpeech* m_speaker = nullptr;
=======
// private:
//   QTextToSpeech* m_speaker = nullptr;
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
};

#endif // NAVIGATEROUTESPEAKER_H
