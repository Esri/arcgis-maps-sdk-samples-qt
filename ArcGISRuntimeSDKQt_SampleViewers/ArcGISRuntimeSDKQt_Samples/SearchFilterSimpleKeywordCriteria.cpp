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

#include "SearchFilterSimpleKeywordCriteria.h"

#include <QStringBuilder>
#include <QStringView>

#include "SampleListModel.h"

#include <algorithm>

namespace
{

// Give points per character match
constexpr qint64 DEFAULT_CHAR_MATCH     = 5;
// Give points per character + case match
constexpr qint64 DEFAULT_CASE_MATCH     = 7;
// Modify the score by how close the score was to the start of the string.
constexpr qint64 DEFAULT_START_MODIFIER = 2;

/*!
 * \brief stringCompare
 *  Compares two strings. Returns a score > 0 if \c{subString} matches string
 *  entirely or if \c{subString}  partially matches the begining of \c{string}.
 *  Otherwise returns a score of 0.
 * \param string String to check against.
 * \param subString Substring to see if partially or fully matches begining.
 * \return score value based on score system values.
 */
qint64 stringCompare(const QStringView string, const QString& subString)
{
  qint64 score = 0;

  // Substring is more specific than string. Abort.
  if (subString.length() > string.length())
    return score;

  const int total = std::min(static_cast<qsizetype>(string.length()), subString.length());
  for (int i = 0; i < total; ++i)
  {
    const auto& a = string.at(i);
    const auto& b = subString.at(i);

    // Make the score better if the categories match.
    // I.E. TitleCase / UpperCase / LowerCase / Punctuation
    const qint64 baseScore = a.category() == b.category() ? DEFAULT_CASE_MATCH
                                                          : DEFAULT_CHAR_MATCH;

    // We'll assume that lowercase categories are always available.
    if (a.toLower() == b.toLower())
    {
      score += baseScore;
    }
    else
    {
      // Strings are different. End now.
      return 0;
    }
  }
  return score;
}

/*!
 * \brief subStringCompare
 *  Checks to see if \c{subString} is a sub-string of \c{string} at any
 *  position in \c{string}. Returns a positive integer and a position when a
 *   match is found. Otherwise will return (0, -1)
 * \param string string to check internally for substring matches.
 * \param subString substring to check occurances of in \c{string}.
 * \param lazy optional value that changes behaviour. When lazy is true then the
 *  first match is considered valid, and the search ends. Otherwise when
 *  the search is not lazy then all scores are accumalated, with the position
 *  returned being the position associated with the best score found.
 * \return pair of: score of match(es), and position of the best match.
 */
QPair<qint64, int> subStringCompare(const QStringView string,
                                    const QString& subString,
                                    bool lazy = true)
{
  qint64 bestResult = 0;
  QPair<qint64, int> output = { 0, -1 };

  const int length = string.length();
  for (int i = 0; i < length; ++i)
  {
    auto result = stringCompare(string.sliced(i, length - i), subString);
    if (i == 0)
      result *= DEFAULT_START_MODIFIER;
    output.first += std::max(0LL, result);
    if (result > bestResult)
    {
      bestResult = result;
      output.second = i;
      if (lazy)
        break;
    }

    //! If substring was a match. Skip ahead to a new second substring has
    //! not touched.
    if (result > 0)
      i += subString.length();
  }
  return output;
}

} // anonymous namespace

SearchFilterSimpleKeywordCriteria::SearchFilterSimpleKeywordCriteria(QObject* parent) :
  SearchFilterCriteria(parent)
{
}

SearchFilterSimpleKeywordCriteria::~SearchFilterSimpleKeywordCriteria() = default;

void SearchFilterSimpleKeywordCriteria::setNameModifier(qint64 modifier)
{
  m_titleModifier = modifier;
}

qint64 SearchFilterSimpleKeywordCriteria::nameModifier() const
{
  return m_titleModifier;
}

void SearchFilterSimpleKeywordCriteria::setSourceModifier(qint64 modifier)
{
  m_codeModifier = modifier;
}

qint64 SearchFilterSimpleKeywordCriteria::sourceModifer() const
{
  return m_codeModifier;
}

void SearchFilterSimpleKeywordCriteria::setDescriptionModifier(qint64 modifier)
{
  m_detailModifier = modifier;
}

qint64 SearchFilterSimpleKeywordCriteria::descriptionModifier() const
{
  return m_detailModifier;
}


SearchResult SearchFilterSimpleKeywordCriteria::scoreValue(
    const QModelIndex& index,
    const QString& searchString) const
{
  const auto nameScore   = scoreIndex(index, searchString, SampleListModel::NameRole, nameModifier());
  const auto descScore   = scoreIndex(index, searchString, SampleListModel::DescriptionRole, descriptionModifier());
  const auto sourceScore = scoreIndex(index, searchString, SampleListModel::SourceRole, sourceModifer());

  qint64 best = sourceScore.first;
  if (descScore.first > best)
    best = descScore.first;
  if (nameScore.first > best)
    best = nameScore.first;
  return { best, "" }; // We don't care for any kind of context help.
}

QPair<qint64, int> SearchFilterSimpleKeywordCriteria::scoreIndex(
    const QModelIndex& index,
    const QString& searchString,
    int role,
    qint64 modifier) const
{
  const auto simpleSearchString = searchString.simplified();
  QPair<qint64, int> bestResult { 0, -1 };

  if (index.isValid())
  {
    auto roleData = index.data(role);
    if (roleData.isValid())
    {
      const auto str = roleData.toString();
      auto r = subStringCompare(str, simpleSearchString);
      r.first *= modifier;
      bestResult = r;
    }
  }
  return bestResult;
}
