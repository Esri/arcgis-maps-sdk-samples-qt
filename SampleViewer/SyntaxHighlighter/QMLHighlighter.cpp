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
#include <QRegularExpression>

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
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    m_classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\b[A-Z][A-Za-z]+\\b");
    rule.format = m_classFormat;
    m_highlightingRules.append(rule);

    m_classFormat_2.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ_PROPERTY\\b");
    rule.format = m_classFormat_2;
    m_highlightingRules.append(rule);

    m_classFormat_3.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ_INVOKABLE\\b");
    rule.format = m_classFormat_3;
    m_highlightingRules.append(rule);

    m_memberVarName_1.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\b[a-z][A-Za-z]+(?=:)\\b");
    rule.format = m_memberVarName_1;
    m_highlightingRules.append(rule);

    m_memberVarName_2.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\b[a-z]+(?=:)\\b");
    rule.format = m_memberVarName_2;
    m_highlightingRules.append(rule);

    m_memberVarName_3.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\banchors.\\b");
    rule.format = m_memberVarName_3;
    m_highlightingRules.append(rule);

    m_memberVarName_4.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\banchors\\b");
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
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordOtherFormat;
        m_highlightingRules.append(rule);
    }

    m_singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("^(?!\"([^\"']*)\")\\s*//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    m_multiLineCommentFormat.setForeground(Qt::darkGreen);

    m_quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("(\"[^\"]+\"|'[^\']+')");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}

void QMLHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightingRule& rule, m_highlightingRules)
    {
        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatchIterator matchIterator = expression.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            int index = match.capturedStart();
            int length = match.capturedLength();
            setFormat(index, length, rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = m_commentStartExpression.match(text).capturedStart();

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);

        int endIndex = match.capturedStart();
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startIndex = m_commentStartExpression.match(text, startIndex + commentLength).capturedStart();
    }
}

