// [WriteFile Name=OpenExistingMap, Category=Maps]
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

#ifndef SEARCHSYMBOLDICTIONARY_H
#define SEARCHSYMBOLDICTIONARY_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class SymbolDictionary;
    class StyleSymbolSearchResultListModel;
  }
}

#include <QQuickItem>
#include <QUuid>
#include <QAbstractListModel>

class SearchSymbolDictionary : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(FieldEnum)

    Q_PROPERTY(Esri::ArcGISRuntime::StyleSymbolSearchResultListModel* searchResultsListModel READ searchResultsListModel NOTIFY searchResultsListModelChanged)
    Q_PROPERTY(QStringList namesFieldModel READ namesFieldModel NOTIFY namesFieldModelChanged )
    Q_PROPERTY(QStringList tagsFieldModel READ tagsFieldModel NOTIFY tagsFieldModelChanged )
    Q_PROPERTY(QStringList classesFieldModel READ classesFieldModel NOTIFY classesFieldModelChanged )
    Q_PROPERTY(QStringList catsFieldModel READ catsFieldModel NOTIFY catsFieldModelChanged )
    Q_PROPERTY(QStringList keysFieldModel READ keysFieldModel NOTIFY keysFieldModelChanged )

public:
    explicit SearchSymbolDictionary(QQuickItem *parent = 0);
    ~SearchSymbolDictionary();

    enum class FieldEnum {
        FieldNames,
        FieldTags,
        FieldClasses,
        FieldCategories,
        FieldKeys
    };

    void componentComplete() Q_DECL_OVERRIDE;
    Q_INVOKABLE void search();
    Q_INVOKABLE void addNameField(QString field);
    Q_INVOKABLE void removeNameField(QString field);
    Q_INVOKABLE void addTagField(QString field);
    Q_INVOKABLE void removeTagField(QString field);
    Q_INVOKABLE void addClassField(QString field);
    Q_INVOKABLE void removeClassField(QString field);
    Q_INVOKABLE void addCatField(QString field);
    Q_INVOKABLE void removeCatField(QString field);
    Q_INVOKABLE void addKeyField(QString field);
    Q_INVOKABLE void removeKeyField(QString field);
    Q_INVOKABLE void clearModelsField();

signals:
    void searchResultsListModelChanged();
    void namesFieldModelChanged();
    void tagsFieldModelChanged();
    void classesFieldModelChanged();
    void catsFieldModelChanged();
    void keysFieldModelChanged();
    void searchCountUpdate(int count);
    void searchCompleted(QList<QString> resultNames);

private:
    bool addFieldToModel(QStringList& model, QString field);
    bool removeFieldFromModel(QStringList& model, QString field);

    Esri::ArcGISRuntime::StyleSymbolSearchResultListModel* searchResultsListModel() const;
    QStringList namesFieldModel() const { return m_namesFieldModel; }
    QStringList tagsFieldModel() const { return m_tagsFieldModel; }
    QStringList classesFieldModel() const { return m_classesFieldModel; }
    QStringList catsFieldModel() const { return m_catsFieldModel; }
    QStringList keysFieldModel() const { return m_keysFieldModel; }

    Esri::ArcGISRuntime::SymbolDictionary* m_SymbolDictionary;
    Esri::ArcGISRuntime::StyleSymbolSearchResultListModel* m_searchResults;

    QStringList m_namesFieldModel;
    QStringList m_tagsFieldModel;
    QStringList m_classesFieldModel;
    QStringList m_catsFieldModel;
    QStringList m_keysFieldModel;
};

#endif // SEARCHSYMBOLDICTIONARY_H

