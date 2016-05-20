// [WriteFile Name=SearchSymbolDictionary, Category=Symbology]
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
    m_SymbolDictionary(nullptr)
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
    m_SymbolDictionary = new SymbolDictionary("mil2525d", datapath);

    //Connect to the search completed signal of the dictionary
    connect(m_SymbolDictionary, &SymbolDictionary::searchSymbolsCompleted, [this](QList<StyleSymbolSearchResult> results)
    {
        m_latestResults = results;
        QList<QString> resultNames;
        foreach (StyleSymbolSearchResult result, results)
        {
            resultNames << result.name();
        }
        emit searchCountUpdate(results.count());
        emit searchCompleted(resultNames);
    });
}

void SearchSymbolDictionary::addNameField(QString field)
{
    //Add the field to the names list
    if(addFieldToModel(m_namesFieldModel, field))
        emit namesFieldModelChanged();
}

void SearchSymbolDictionary::removeNameField(QString field)
{
    //Remove the field from the names list
    if(removeFieldFromModel(m_namesFieldModel, field))
        emit namesFieldModelChanged();
}

void SearchSymbolDictionary::addTagField(QString field)
{
    //Add the field to the tags list
    if(addFieldToModel(m_tagsFieldModel, field))
        emit tagsFieldModelChanged();
}

void SearchSymbolDictionary::removeTagField(QString field)
{
    //Remove the field from the tags list
    if(removeFieldFromModel(m_tagsFieldModel, field))
        emit tagsFieldModelChanged();
}

void SearchSymbolDictionary::addClassField(QString field)
{
    //Add the field to the classes list
    if(addFieldToModel(m_classesFieldModel, field))
        emit classesFieldModelChanged();
}

void SearchSymbolDictionary::removeClassField(QString field)
{
    //Remove the field from the classes list
    if(removeFieldFromModel(m_classesFieldModel, field))
        emit classesFieldModelChanged();
}

void SearchSymbolDictionary::addCatField(QString field)
{
    //Add the field to the categories list
    if(addFieldToModel(m_catsFieldModel, field))
        emit catsFieldModelChanged();
}

void SearchSymbolDictionary::removeCatField(QString field)
{
    //Remove the field from the categories list
    if(removeFieldFromModel(m_catsFieldModel, field))
        emit catsFieldModelChanged();
}

void SearchSymbolDictionary::addKeyField(QString field)
{
    //Add the field to the keys list
    if(addFieldToModel(m_keysFieldModel, field))
        emit keysFieldModelChanged();
}

void SearchSymbolDictionary::removeKeyField(QString field)
{
    //Remove the field from the keys list
    if(removeFieldFromModel(m_keysFieldModel, field))
        emit keysFieldModelChanged();
}

void SearchSymbolDictionary::clearModelsField()
{
    //Clear the lists
    m_namesFieldModel.clear();
    emit namesFieldModelChanged();
    m_tagsFieldModel.clear();
    emit tagsFieldModelChanged();
    m_classesFieldModel.clear();
    emit classesFieldModelChanged();
    m_catsFieldModel.clear();
    emit catsFieldModelChanged();
    m_keysFieldModel.clear();
    emit keysFieldModelChanged();
}

bool SearchSymbolDictionary::addFieldToModel(QStringList& model, QString field)
{
    //Add unique field only
    if(model.indexOf(field) == -1)
    {
        model.append(field);
        return true;
    }
    return false;
}

bool SearchSymbolDictionary::removeFieldFromModel(QStringList& model, QString field)
{
    //Remove the field and return if the field was removed
    return model.removeAll(field) > 0;
}

void SearchSymbolDictionary::search()
{
    //Create search parameters and search with the parameters
    StyleSymbolSearchParameters searchParameters;
    searchParameters.setCategories(m_catsFieldModel);
    searchParameters.setKeys(m_keysFieldModel);
    searchParameters.setNames(m_namesFieldModel);
    searchParameters.setSymbolClasses(m_classesFieldModel);
    searchParameters.setTags(m_tagsFieldModel);
    m_SymbolDictionary->searchSymbols(searchParameters);
}




