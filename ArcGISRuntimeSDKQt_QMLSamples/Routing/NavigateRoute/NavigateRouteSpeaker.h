#ifndef NAVIGATEROUTESPEAKER_H
#define NAVIGATEROUTESPEAKER_H

#include <QObject>

class QTextToSpeech;
class NavigateRouteSpeaker : public QObject
{
  Q_OBJECT

public:
  explicit NavigateRouteSpeaker(QObject* parent = nullptr);
  ~NavigateRouteSpeaker();

  Q_INVOKABLE void textToSpeech(QString text);

protected:
  QTextToSpeech* speaker = nullptr;
};

#endif // NAVIGATEROUTESPEAKER_H
