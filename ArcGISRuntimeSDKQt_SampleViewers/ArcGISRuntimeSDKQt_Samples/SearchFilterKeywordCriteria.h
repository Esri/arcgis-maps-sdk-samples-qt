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

#ifndef SEARCHFILTERKEYWORDCRITERIA_H
#define SEARCHFILTERKEYWORDCRITERIA_H

#include "SearchFilterSimpleKeywordCriteria.h"

/*!
 * \brief The SearchFilterKeywordCriteria class is an implementation of the
 *        SeatchFilterSimpleKeywordCriteria, where the second parameter returned
 *        by \c{scoreValue} is not blank, but is instead a context sensitive
 *        string of where the searchString matched in the search parameters.
 */
class SearchFilterKeywordCriteria : public SearchFilterSimpleKeywordCriteria
{
  Q_OBJECT
public:
  SearchFilterKeywordCriteria(QObject* parent = nullptr);

  ~SearchFilterKeywordCriteria() override;

  SearchResult scoreValue(const QModelIndex& index,
                          const QString& searchString) const override;
};

#endif // SEARCHFILTERKEYWORDCRITERIA_H
