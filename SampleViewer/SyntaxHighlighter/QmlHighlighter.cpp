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

// Qt headers
#include <QRegularExpression>
#include <QStringList>

// Other headers
#include "QmlHighlighter.h"

QmlHighlighter::QmlHighlighter(QTextDocument* parent, bool isDarkTheme) :
  QSyntaxHighlighter(parent),
  m_isDarkTheme(isDarkTheme)
{
  setupColors(isDarkTheme);

  HighlightingRule rule;
  QStringList keywordPatterns;
  keywordPatterns << "\\bint\\b" << "\\bbool\\b" << "\\bstring\\b"
                  << "\\bvar\\b" << "\\bdouble\\b" << "\\breal\\b"
                  << "\\bcase\\b" << "\\bQString\\b" << "\\bQString\\b"
                  << "\\bifndef\\b" << "\\bQ_OBJECT\\b"
                  << "\\bdefine\\b" << "\\binclude\\b" << "\\b#\\b"
                  << "\\bendif\\b" << "\\bQ_DECL_OVERRIDE\\b";
  foreach (const QString& pattern, keywordPatterns)
  {
    rule.pattern = QRegularExpression(pattern);
    rule.format = m_keywordFormat;
    m_highlightingRules.append(rule);
  }

  rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Z][A-Za-z]+\\b"));
  rule.format = m_classFormat;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\bQ_PROPERTY\\b"));
  rule.format = m_classFormat_2;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\bQ_INVOKABLE\\b"));
  rule.format = m_classFormat_3;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\b[a-z][A-Za-z]+(?=:)\\b"));
  rule.format = m_memberVarName_1;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\b[a-z]+(?=:)\\b"));
  rule.format = m_memberVarName_2;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\banchors.\\b"));
  rule.format = m_memberVarName_3;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("\\banchors\\b"));
  rule.format = m_memberVarName_4;
  m_highlightingRules.append(rule);

  QStringList keywordPatternsOther;
  keywordPatternsOther << "\\bfor\\b" << "\\bforeach\\b"
                       << "\\bif\\b" << "\\bwhile\\b"
                       << "\\bthis\\b" << "\\bimport\\b"
                       << "\\breturn\\b" << "\\break\\b"
                       << "\\bproperty\\b" << "\\btrue\\b"
                       << "\\bfalse\\b" << "\\bswitch\\b"
                       << "\\bfunction\\b" << "\\bstatic\\b"
                       << "\\bvoid\\b" << "\\bnamespace\\b"
                       << "\\bclass\\b" << "\\bnullptr\\b"
                       << "\\busing\\b" << "\\belse\\b"
                       << "\\bsignals\\b";

  foreach (const QString& pattern, keywordPatternsOther)
  {
    rule.pattern = QRegularExpression(pattern);
    rule.format = m_keywordOtherFormat;
    m_highlightingRules.append(rule);
  }

  rule.pattern = QRegularExpression(QStringLiteral("^(?!\"([^\"']*)\")\\s*//[^\n]*"));
  rule.format = m_singleLineCommentFormat;
  m_highlightingRules.append(rule);

  rule.pattern = QRegularExpression(QStringLiteral("(\"[^\"]+\"|'[^\']+')"));
  rule.format = m_quotationFormat;
  m_highlightingRules.append(rule);

  m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
  m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void QmlHighlighter::highlightBlock(const QString& text)
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
  {
    startIndex = m_commentStartExpression.match(text).capturedStart();
  }

  while (startIndex >= 0)
  {
    int endIndex = m_commentEndExpression.match(text, startIndex).capturedStart();
    int commentLength;
    if (endIndex == -1)
    {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    }
    else
    {
      commentLength = endIndex - startIndex + m_commentEndExpression.match(text, startIndex).capturedLength();
    }
    setFormat(startIndex, commentLength, m_multiLineCommentFormat);
    startIndex = m_commentStartExpression.match(text, startIndex + commentLength).capturedStart();
  }
}

void QmlHighlighter::setupColors(bool isDarkTheme)
{
  if (isDarkTheme)
  {
    m_keywordFormat.setForeground(QColor("#569CD6")); // Blue - keywords/types (int, bool, var, etc.)
    m_classFormat.setForeground(QColor("#4EC9B0")); // Teal - class/type names
    m_classFormat_2.setForeground(QColor("#4EC9B0")); // Teal - Q_PROPERTY
    m_classFormat_3.setForeground(QColor("#4EC9B0")); // Teal - Q_INVOKABLE
    m_memberVarName_1.setForeground(QColor("#9CDCFE")); // Light blue - properties/variables
    m_memberVarName_2.setForeground(QColor("#9CDCFE")); // Light blue - properties
    m_memberVarName_3.setForeground(QColor("#9CDCFE")); // Light blue - anchors
    m_memberVarName_4.setForeground(QColor("#9CDCFE")); // Light blue - anchors
    m_keywordOtherFormat.setForeground(QColor("#C586C0")); // Purple - control flow (if, for, return, function)
    m_singleLineCommentFormat.setForeground(QColor("#6A9955")); // Green - comments
    m_multiLineCommentFormat.setForeground(QColor("#6A9955")); // Green - comments
    m_quotationFormat.setForeground(QColor("#D4A574")); // Light brown/tan - strings
  }
  else
  {
    m_keywordFormat.setForeground(QColor("#0000FF")); // Blue - keywords/types (int, bool, var, etc.)
    m_classFormat.setForeground(QColor("#267F99")); // Teal - class/type names
    m_classFormat_2.setForeground(QColor("#267F99")); // Teal - Q_PROPERTY
    m_classFormat_3.setForeground(QColor("#267F99")); // Teal - Q_INVOKABLE
    m_memberVarName_1.setForeground(QColor("#001080")); // Dark blue - properties/variables
    m_memberVarName_2.setForeground(QColor("#001080")); // Dark blue - properties
    m_memberVarName_3.setForeground(QColor("#001080")); // Dark blue - anchors
    m_memberVarName_4.setForeground(QColor("#001080")); // Dark blue - anchors
    m_keywordOtherFormat.setForeground(QColor("#AF00DB")); // Purple - control flow (if, for, return, function)
    m_singleLineCommentFormat.setForeground(QColor("#008000")); // Green - comments
    m_multiLineCommentFormat.setForeground(QColor("#008000")); // Green - comments
    m_quotationFormat.setForeground(QColor("#B07800")); // Brown/orange - strings
  }
}
