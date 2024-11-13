// [Legal]
// Copyright 2022 Esri.

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

#ifndef QMLHIGHLIGHTER_H
#define QMLHIGHLIGHTER_H

#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QList>
#include <QRegExp>

class QMLHighlighter : public QSyntaxHighlighter {

public:
    explicit QMLHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> m_highlightingRules;

    QRegExp m_commentStartExpression;
    QRegExp m_commentEndExpression;

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_keywordOtherFormat;
    QTextCharFormat m_classFormat;
    QTextCharFormat m_classFormat_2;
    QTextCharFormat m_classFormat_3;
    QTextCharFormat m_memberVarName_1;
    QTextCharFormat m_memberVarName_2;
    QTextCharFormat m_memberVarName_3;
    QTextCharFormat m_memberVarName_4;
    QTextCharFormat m_singleLineCommentFormat;
    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_quotationFormat;
};

#endif // QMLHIGHLIGHTER_H
