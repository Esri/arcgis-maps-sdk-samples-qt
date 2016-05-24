// [WriteFile Name=SearchSymbolDictionary, Category=Search]
// [Legal]
// Copyright 2016 Esri.

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

#include "SearchSymbolDictionary.h"
#include "SymbolDictionary.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

SearchSymbolDictionary::SearchSymbolDictionary(QQuickItem* parent) :
    QQuickItem(parent),
    m_symbolDictionary(nullptr),
    m_searchResults(nullptr)
{
}

SearchSymbolDictionary::~SearchSymbolDictionary()
{
}

void SearchSymbolDictionary::componentComplete()
{
    QQuickItem::componentComplete();

    //Get the data path
    QString datapath = QQmlProperty::read(this, "dataPath").toString();

    //Create the dictionary from datapath
    m_symbolDictionary = new SymbolDictionary("mil2525d", datapath, this);

    //Connect to the search completed signal of the dictionary
    connect(m_symbolDictionary, &SymbolDictionary::searchSymbolsCompleted, [this](StyleSymbolSearchResultListModel* results)
    {
        m_searchResults = results;
        emit searchResultsListModelChanged();
        emit searchCompleted(results->size());
    });
}

StyleSymbolSearchResultListModel* SearchSymbolDictionary::searchResultsListModel() const
{
    return m_searchResults;
}

void SearchSymbolDictionary::search(const QStringList& namesSearchParam, const QStringList& tagsSearchParam,
                                    const QStringList& classesSearchParam,const QStringList& categoriesSearchParam,
                                    const QStringList& keysSearchParam)
{
    //Create search parameters and search with the parameters
    StyleSymbolSearchParameters searchParameters;
    searchParameters.setCategories(categoriesSearchParam);
    searchParameters.setKeys(keysSearchParam);
    searchParameters.setNames(namesSearchParam);
    searchParameters.setSymbolClasses(classesSearchParam);
    searchParameters.setTags(tagsSearchParam);
    m_symbolDictionary->searchSymbols(searchParameters);
}
