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

#include "pch.hpp"

#include <QStringList>
#include <QRegExp>

#include "QMLHighlighter.h"

QMLHighlighter::QMLHighlighter(QTextDocument* parent) :
  QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    m_keywordFormat.setForeground(Qt::darkBlue);
    QStringList keywordPatterns;
    keywordPatterns << "\\bint\\b"  << "\\bbool\\b" << "\\bstring\\b"
                    << "\\bvar\\b"  << "\\bdouble\\b" << "\\breal\\b"
                    << "\\bcase\\b" << "\\bQString\\b" << "\\bQString\\b"
                    << "\\bifndef\\b" << "\\bQ_OBJECT\\b"
                    << "\\bdefine\\b" << "\\binclude\\b"  << "\\b#\\b"
                    << "\\bendif\\b" << "\\bQ_DECL_OVERRIDE\\b";
    foreach (const QString& pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    m_classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[A-Z][A-Za-z]+\\b");
    rule.format = m_classFormat;
    m_highlightingRules.append(rule);

    m_classFormat_2.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ_PROPERTY\\b");
    rule.format = m_classFormat_2;
    m_highlightingRules.append(rule);

    m_classFormat_3.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ_INVOKABLE\\b");
    rule.format = m_classFormat_3;
    m_highlightingRules.append(rule);

    m_memberVarName_1.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\b[a-z][A-Za-z]+(?=:)\\b");
    rule.format = m_memberVarName_1;
    m_highlightingRules.append(rule);

    m_memberVarName_2.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\b[a-z]+(?=:)\\b");
    rule.format = m_memberVarName_2;
    m_highlightingRules.append(rule);

    m_memberVarName_3.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\banchors.\\b");
    rule.format = m_memberVarName_3;
    m_highlightingRules.append(rule);

    m_memberVarName_4.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\banchors\\b");
    rule.format = m_memberVarName_4;
    m_highlightingRules.append(rule);

    m_keywordOtherFormat.setForeground(Qt::darkYellow);
    QStringList keywordPatternsOther;
    keywordPatternsOther << "\\bfor\\b"      << "\\bforeach\\b"
                         << "\\bif\\b"       << "\\bwhile\\b"
                         << "\\bthis\\b"     << "\\bimport\\b"
                         << "\\breturn\\b"   << "\\break\\b"
                         << "\\bproperty\\b" << "\\btrue\\b"
                         << "\\bfalse\\b"    << "\\bswitch\\b"
                         << "\\bfunction\\b" << "\\bstatic\\b"
                         << "\\bvoid\\b"     << "\\bnamespace\\b"
                         << "\\bclass\\b"    << "\\bnullptr\\b"
                         << "\\busing\\b"    << "\\belse\\b"
                         << "\\bsignals\\b";

    foreach (const QString& pattern, keywordPatternsOther)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = m_keywordOtherFormat;
        m_highlightingRules.append(rule);
    }

    m_singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("^(?!\"([^\"']*)\")\\s*//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    m_multiLineCommentFormat.setForeground(Qt::darkGreen);

    m_quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("(\"[^\"]+\"|'[^\']+')");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    m_commentStartExpression = QRegExp("/\\*");
    m_commentEndExpression = QRegExp("\\*/");
}

void QMLHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightingRule& rule, m_highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = m_commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = m_commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + m_commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = m_commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

