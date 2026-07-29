// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

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

#include "TextDocumentUtils.h"

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFragment>
#include <QTextFrame>
#include <QTextImageFormat>
#include <QTextLength>
#include <QVector>

namespace
{
  struct ImageRange
  {
    int position;
    int length;
    QTextImageFormat format;
  };

  void collectImageRanges(QTextFrame* frame, QVector<ImageRange>& imageRanges)
  {
    for (auto frameIterator = frame->begin(); !frameIterator.atEnd(); ++frameIterator)
    {
      if (QTextFrame* childFrame = frameIterator.currentFrame())
      {
        collectImageRanges(childFrame, imageRanges);
        continue;
      }

      const QTextBlock block = frameIterator.currentBlock();
      for (auto blockIterator = block.begin(); !blockIterator.atEnd(); ++blockIterator)
      {
        const QTextFragment fragment = blockIterator.fragment();
        if (fragment.isValid() && fragment.charFormat().isImageFormat())
        {
          imageRanges.append({fragment.position(), fragment.length(), fragment.charFormat().toImageFormat()});
        }
      }
    }
  }
} // namespace

TextDocumentUtils::TextDocumentUtils(QObject* parent) :
  QObject(parent)
{
}

void TextDocumentUtils::constrainImageWidths(QQuickTextDocument* quickTextDocument) const
{
  if (!quickTextDocument || !quickTextDocument->textDocument())
  {
    return;
  }

  QTextDocument* document = quickTextDocument->textDocument();
  QVector<ImageRange> imageRanges;
  collectImageRanges(document->rootFrame(), imageRanges);

  if (imageRanges.isEmpty())
  {
    return;
  }

  QTextCursor cursor(document);
  cursor.beginEditBlock();
  for (ImageRange& imageRange : imageRanges)
  {
    imageRange.format.setMaximumWidth(QTextLength(QTextLength::PercentageLength, 100));
    cursor.setPosition(imageRange.position);
    cursor.setPosition(imageRange.position + imageRange.length, QTextCursor::KeepAnchor);
    cursor.setCharFormat(imageRange.format);
  }
  cursor.endEditBlock();
}
