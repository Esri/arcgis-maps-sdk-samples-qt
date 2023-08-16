// [Legal]
// Copyright 2023 Esri.

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

#include "XmlParser.h"

#include <QFile>
#include <QVariantMap>
#include <QVariantList>
#include <QXmlStreamReader>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>

XmlParser::XmlParser(QObject *parent)
  : QObject{parent}
{
}

void XmlParser::parseXmlFileAsync(const QString& filePath)
{
  m_synchronizer.addFuture(QtConcurrent::run(&XmlParser::parseXmlFileInternal, this, filePath));
}

void XmlParser::parseXmlFileInternal(const QString& filePath)
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
    emit xmlParseComplete(parsedJsonList);
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
  emit xmlParseComplete(parsedJsonList);
}
