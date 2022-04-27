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

#ifndef SEARCHFILTERKEYWORDSIMPLECRITERIA_H
#define SEARCHFILTERKEYWORDSIMPLECRITERIA_H

#include "SearchFilterCriteria.h"

/*!
 * \brief The SearchFilterSimpleKeywordCriteria class is the most basic search
 *        system used for sample lookup. In this system, the description, source
 *        code, and name roles of the index are tested, and the returned score
 *        is multiplied by the given modifiers. The best score of
 *        these three checks is returned via the first index of the pair
 *        returned by \c{scoreValue}.
 *        This search does not provide any context into how the search was
 *        performed or where the search-string was found. As such the second
 *        index (QString) of \c{scoreValue} is always blank.
 *        For a search which does provide context-search-strings, look at class
 *        \c{SearchFilterSimpleCriteria}.
 */
class SearchFilterSimpleKeywordCriteria : public SearchFilterCriteria
{
  Q_OBJECT
public:
    // Modify the score if found in the title of the sample
    static constexpr qint64 NAME_MODIFIER = 10;
    // Modify the score if found in the readme of the sample
    static constexpr qint64 DESCRIPTION_MODIFIER = 3;
    // Modify the score if found in the code of the sample
    static constexpr qint64 SOURCE_MODIFIER = 1;
public:
  SearchFilterSimpleKeywordCriteria(QObject* parent = nullptr);

   ~SearchFilterSimpleKeywordCriteria() override;

  void setNameModifier(qint64 modifier);
  qint64 nameModifier() const;

  void setSourceModifier(qint64 modifier);
  qint64 sourceModifer() const;

  void setDescriptionModifier(qint64 modifier);
  qint64 descriptionModifier() const;

  /*!
   * \brief scoreValue tests an index and returns the score value for this index
   *        when compared against the given search string.
   * \param index the index value to retrieve a score for.
   * \param searchString the string to search against.
   * \return pair of score value for this index , and a blank string.
   */
  SearchResult scoreValue(
          const QModelIndex& index,
          const QString& searchString) const override;

protected:
  /*!
   * \brief scoreIndex for a given index, search string, role, and modifier,
   *        returns the score for the given index * modifier, and the position
   *        the search string was discovered in the string provided by \c{role}.
   * \param index index to query data for.
   * \param searchString search string to compare against.
   * \param role role to retrieve the data string data from the index.
   * \param modifier final modifier to multiply the score against.
   * \return pair of score value for role time modifier and index of found
   *         position in search string.
   */
  QPair<qint64, int> scoreIndex(
          const QModelIndex& index,
          const QString& searchString,
          int role,
          qint64 modifier) const;

private:
  qint64 m_titleModifier { NAME_MODIFIER };
  qint64 m_detailModifier { DESCRIPTION_MODIFIER };
  qint64 m_codeModifier { SOURCE_MODIFIER };
};

#endif // SEARCHFILTERKEYWORDSIMPLECRITERIA_H
