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

#include "SearchFilterKeywordCriteria.h"

// STL headers
#include <algorithm>

// Other headers
#include "SampleListModel.h"

namespace
{

QString highlightedMidpoint(const QString& string,
                            int subStringLength,
                            int pos,
                            int catchment = 25)
{
    QString builder;

    // Find the portion of string that goes before the catchment.
    int startPos = std::max(0, pos-catchment);
    const QStringView startPart(&string, startPos, catchment);

    // Start section
    builder = builder + startPart.toString().toHtmlEscaped();

    // The found portion of string to be highlighted
    const QStringView caughtString(&string, pos, subStringLength);
    builder = builder + QStringLiteral("<b>")
                      + caughtString.toString().toHtmlEscaped()
                      + QStringLiteral("</b>");

    // Find the portion of string that goes after the catchment.
    int endPos = std::min(string.length(), pos+ subStringLength + catchment);
    const QStringView endPart(&string, pos + subStringLength, endPos);
    builder = builder + endPart.toString().toHtmlEscaped();

    return builder;
}

}

SearchFilterKeywordCriteria::SearchFilterKeywordCriteria(QObject* parent) :
    SearchFilterSimpleKeywordCriteria(parent)
{
}

SearchFilterKeywordCriteria::~SearchFilterKeywordCriteria() = default;

SearchResult SearchFilterKeywordCriteria::scoreValue(
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

    if (best == nameScore.first)
    {
        return { nameScore.first,
                 highlightedMidpoint(index.data(SampleListModel::NameRole).toString(),
                                     searchString.length(),
                                     nameScore.second)
                };
    }
    else if (best == descScore.first)
    {
        return { descScore.first,
                 highlightedMidpoint(index.data(SampleListModel::DescriptionRole).toString(),
                                     searchString.length(),
                                     descScore.second)
                };
    }
    else if (best == sourceScore.first)
    {
        return { sourceScore.first,
                 highlightedMidpoint(index.data(SampleListModel::SourceRole).toString(),
                                     searchString.length(),
                                     sourceScore.second)
                };
    }
    return SearchResult();
}
