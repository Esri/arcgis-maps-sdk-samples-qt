// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef SHOWORGBASEMAPPICKER_H
#define SHOWORGBASEMAPPICKER_H

namespace Esri
{
    namespace ArcGISRuntime
    {
        class AuthenticationManager;
        class BasemapListModel;
        class Map;
        class MapQuickView;
        class Portal;
    }
}

#include <QAbstractListModel>
#include <QQuickItem>

class ShowOrgBasemapPicker : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::AuthenticationManager* authManager READ authManager NOTIFY authManagerChanged)
    Q_PROPERTY(bool portalLoaded READ portalLoaded NOTIFY portalLoadedChanged)
    Q_PROPERTY(QString orgName READ orgName NOTIFY orgNameChanged)
    Q_PROPERTY(QAbstractListModel* basemaps READ basemaps NOTIFY basemapsChanged)
    Q_PROPERTY(QString mapLoadError READ mapLoadError NOTIFY mapLoadErrorChanged)

public:
    ShowOrgBasemapPicker(QQuickItem* parent = nullptr);
    ~ShowOrgBasemapPicker();

    void componentComplete() Q_DECL_OVERRIDE;

    Esri::ArcGISRuntime::AuthenticationManager* authManager() const;
    bool portalLoaded() const;
    QString orgName() const;
    QAbstractListModel* basemaps() const;
    QString mapLoadError() const;

    Q_INVOKABLE void loadSelectedBasemap(int index);
    Q_INVOKABLE void errorAccepted();
    Q_INVOKABLE QString basemapTitle(int index);
    Q_INVOKABLE QUrl basemapThumbnail(int index);

signals:
    void authManagerChanged();
    void portalLoadedChanged();
    void orgNameChanged();
    void basemapsChanged();
    void mapLoadErrorChanged();

private slots:
    void onFetchBasemapsCompleted();

private:
    Esri::ArcGISRuntime::Map* m_map;
    Esri::ArcGISRuntime::MapQuickView* m_mapView;
    Esri::ArcGISRuntime::Portal* m_portal;
    bool m_portalLoaded;
    QString m_mapLoadeError;
};

#endif // SHOWORGBASEMAPPICKER_H
