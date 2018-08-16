// [WriteFile Name=SearchDictionarySymbolStyle, Category=Search]
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

#include "SearchDictionarySymbolStyle.h"
#include "DictionarySymbolStyle.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

SearchDictionarySymbolStyle::SearchDictionarySymbolStyle(QQuickItem* parent) :
  QQuickItem(parent),
  m_dictionarySymbolStyle(nullptr),
  m_searchResults(nullptr)
{
}

SearchDictionarySymbolStyle::~SearchDictionarySymbolStyle()
{
}

void SearchDictionarySymbolStyle::init()
{
  qmlRegisterType<SearchDictionarySymbolStyle>("Esri.Samples", 1, 0, "SearchDictionarySymbolStyleSample");
  qmlRegisterUncreatableType<QAbstractListModel>("Esri.Samples", 1, 0, "AbstractListModel", "AbstractListModel is uncreateable");
}

void SearchDictionarySymbolStyle::componentComplete()
{
  QQuickItem::componentComplete();

  //Get the data path
  QString datapath = QQmlProperty::read(this, "dataPath").toUrl().toLocalFile();

  //Create the dictionary from datapath
  m_dictionarySymbolStyle = new DictionarySymbolStyle("mil2525d", datapath, this);

  //Connect to the search completed signal of the dictionary
  connect(m_dictionarySymbolStyle, &DictionarySymbolStyle::searchSymbolsCompleted, this, [this](QUuid, SymbolStyleSearchResultListModel* results)
  {
    m_searchResults = results;
    emit searchResultsListModelChanged();
    emit searchCompleted(results->size());
  });
}

QAbstractListModel* SearchDictionarySymbolStyle::searchResultsListModel() const
{
  return m_searchResults;
}

void SearchDictionarySymbolStyle::search(const QStringList& namesSearchParam, const QStringList& tagsSearchParam,
                                         const QStringList& classesSearchParam,const QStringList& categoriesSearchParam,
                                         const QStringList& keysSearchParam)
{
  //Create search parameters and search with the parameters
  SymbolStyleSearchParameters searchParameters;
  searchParameters.setCategories(categoriesSearchParam);
  searchParameters.setKeys(keysSearchParam);
  searchParameters.setNames(namesSearchParam);
  searchParameters.setSymbolClasses(classesSearchParam);
  searchParameters.setTags(tagsSearchParam);
  m_dictionarySymbolStyle->searchSymbols(searchParameters);
}
