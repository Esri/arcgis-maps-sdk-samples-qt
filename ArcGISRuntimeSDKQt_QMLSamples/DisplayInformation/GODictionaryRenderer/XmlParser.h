#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>

class XmlParser : public QObject
{
  Q_OBJECT
public:
  explicit XmlParser(QObject *parent = nullptr);
  Q_INVOKABLE QVariantList parseXmlFile(QString filePath);

};

#endif // XMLPARSER_H
