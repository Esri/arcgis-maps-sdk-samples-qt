#include "XmlParser.h"

#include <QFile>
#include <QVariantMap>
#include <QVariantList>
#include <QXmlStreamReader>
#include <QUrl>

XmlParser::XmlParser(QObject *parent)
  : QObject{parent}
{
}

QVariantList XmlParser::parseXmlFile(QString filePath)
{
  QXmlStreamReader xmlParser;
  QVariantList parsedJsonList;

  bool readingMessage = false;
  QVariantMap elementValues;
  QString currentElementName;

  QUrl fileUrl(filePath);
  const QString pathToLocalFile = fileUrl.toLocalFile();

  QFile xmlFile(pathToLocalFile);

  if (!QFile::exists(pathToLocalFile))
  {
    qDebug() << xmlFile.fileName() << "does not exist";
    return parsedJsonList;
  }

  // Open the file for reading
  if (xmlFile.isOpen())
  {
    xmlFile.reset();
  }
  else
  {
    xmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
  }
  xmlParser.setDevice(&xmlFile);

  while (!xmlParser.atEnd())
  {
    xmlParser.readNext();

    // Is this the start or end of a message element?
    if (xmlParser.name() == QString("message"))
    {
      if (!readingMessage)
      {
        // This is the start of a message element.
        elementValues.clear();
      }
      else
      {
        // This is the end of a message element. Here we have a complete message that defines
        // a military feature to display on the map. Create a graphic from its attributes.
        parsedJsonList.append(elementValues);
      }
      // Either we just started reading a message, or we just finished reading a message.
      readingMessage = !readingMessage;
    }
    // Are we already inside a message element?
    else if (readingMessage)
    {
      // Is this the start of an element inside a message?
      if (xmlParser.isStartElement())
      {
        // Remember which element we're reading
        currentElementName = xmlParser.name().toString();
      }
      // Is this text?
      else if (xmlParser.isCharacters())
      {
        // Is this text inside an element?
        if (!currentElementName.isEmpty())
        {
          // Get the text and store it as the current element's value
          const QStringView trimmedText = xmlParser.text().trimmed();
          if (!trimmedText.isEmpty())
          {
            elementValues[currentElementName] = trimmedText.toString();
          }
        }
      }
    }
  }
  return parsedJsonList;
}
