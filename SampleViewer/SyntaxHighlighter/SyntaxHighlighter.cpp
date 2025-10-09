// [Legal]
// Copyright 2022 Esri.
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

#include "pch.hpp"

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

#include <QTextDocument>
#include <QQuickTextDocument>

#include "QmlHighlighter.h"

#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QObject* parent) :
  QObject(parent)
{
}

void SyntaxHighlighter::setHighlighter(QObject* textArea, bool isDarkTheme) {
    QQuickTextDocument* quickTextDocument = qvariant_cast<QQuickTextDocument*>(textArea->property("textDocument"));
    QTextDocument* document = quickTextDocument->textDocument();
    m_highlighter = new QmlHighlighter(document, isDarkTheme);
    m_highlighter->rehighlight();
}

void SyntaxHighlighter::updateHighlighterTheme(bool isDarkTheme) {
    if (m_highlighter) {
        m_highlighter->updateTheme(isDarkTheme);
    }
}
