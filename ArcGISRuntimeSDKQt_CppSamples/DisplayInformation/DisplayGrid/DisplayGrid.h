// [WriteFile Name=DisplayGrid, Category=DisplayInformation]
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

#ifndef DISPLAYGRID_H
#define DISPLAYGRID_H

namespace Esri
{
  namespace ArcGISRuntime
  {
    class Map;
    class MapQuickView;
  }
}

#include <QQuickItem>

class DisplayGrid : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapQuickView WRITE setMapQuickView NOTIFY mapQuickViewChanged)

  Q_PROPERTY(QString utmGrid READ utmGrid NOTIFY utmGridChanged)
  Q_PROPERTY(QString usngGrid READ usngGrid NOTIFY usngGridChanged)
  Q_PROPERTY(QString latlonGrid READ latlonGrid NOTIFY latlonGridChanged)
  Q_PROPERTY(QString mgrsGrid READ mgrsGrid NOTIFY mgrsGridChanged)
  Q_PROPERTY(QString ddFormat READ ddFormat NOTIFY ddFormatChanged)
  Q_PROPERTY(QString dmsFormat READ dmsFormat NOTIFY dmsFormatChanged)
  Q_PROPERTY(QString geographicPosition READ geographicPosition NOTIFY geographicPositionChanged)
  Q_PROPERTY(QString bottomLeftPosition READ bottomLeftPosition NOTIFY bottomLeftPositionChanged)
  Q_PROPERTY(QString bottomRightPosition READ bottomRightPosition NOTIFY bottomRightPositionChanged)
  Q_PROPERTY(QString topLeftPosition READ topLeftPosition NOTIFY topLeftPositionChanged)
  Q_PROPERTY(QString topRightPosition READ topRightPosition NOTIFY topRightPositionChanged)
  Q_PROPERTY(QString centerPosition READ centerPosition NOTIFY centerPositionChanged)
  Q_PROPERTY(QString allSidesPosition READ allSidesPosition NOTIFY allSidesPositionChanged)
  Q_PROPERTY(QString currentGridColor READ currentGridColor WRITE setCurrentGridColor NOTIFY currentGridColorChanged)
  Q_PROPERTY(QString currentLabelColor READ currentLabelColor WRITE setCurrentLabelColor NOTIFY currentLabelColorChanged)
  Q_PROPERTY(QString currentLabelFormat READ currentLabelFormat WRITE setCurrentLabelFormat NOTIFY currentLabelFormatChanged)
  Q_PROPERTY(QString currentLabelPosition READ currentLabelPosition WRITE setCurrentLabelPosition NOTIFY currentLabelPositionChanged)
  Q_PROPERTY(bool gridVisibility READ gridVisibility WRITE setGridVisibility NOTIFY gridVisibilityChanged)
  Q_PROPERTY(bool gridLabelVisibility READ gridLabelVisibility WRITE setGridLabelVisibility NOTIFY gridLabelVisibilityChanged)

public:
  explicit DisplayGrid(QQuickItem* parent = nullptr);
  ~DisplayGrid() override;

  void componentComplete() override;
  static void init();
  Q_INVOKABLE void changeGrid(const QString& gridType);
  Q_INVOKABLE void changeGridColor(const QString& color);
  Q_INVOKABLE void changeLabelColor(const QString& color);
  Q_INVOKABLE void changeLabelFormat(const QString& format);
  Q_INVOKABLE void changeLabelPosition(const QString& position);

signals:
  void utmGridChanged();
  void usngGridChanged();
  void latlonGridChanged();
  void mgrsGridChanged();
  void ddFormatChanged();
  void dmsFormatChanged();
  void geographicPositionChanged();
  void bottomLeftPositionChanged();
  void bottomRightPositionChanged();
  void topLeftPositionChanged();
  void topRightPositionChanged();
  void centerPositionChanged();
  void allSidesPositionChanged();
  void currentGridColorChanged();
  void currentLabelColorChanged();
  void currentLabelFormatChanged();
  void currentLabelPositionChanged();
  void gridVisibilityChanged();
  void gridLabelVisibilityChanged();
  void mapQuickViewChanged();

private:
    static const QString utmGrid() { return s_utmGrid; }
    static const QString usngGrid() { return s_usngGrid; }
    static const QString latlonGrid() { return s_latlonGrid; }
    static const QString mgrsGrid() { return s_mgrsGrid; }
    static const QString ddFormat() { return s_ddFormat; }
    static const QString dmsFormat() { return s_dmsFormat; }
    static const QString geographicPosition() { return s_geographicPosition; }
    static const QString bottomLeftPosition() { return s_bottomLeftPosition; }
    static const QString bottomRightPosition() { return s_bottomRightPosition; }
    static const QString topLeftPosition() { return s_topLeftPosition; }
    static const QString topRightPosition() { return s_topRightPosition; }
    static const QString centerPosition() { return s_centerPosition; }
    static const QString allSidesPosition() { return s_allSidesPosition; }
    Esri::ArcGISRuntime::MapQuickView* mapQuickView() const;
    void setMapQuickView(Esri::ArcGISRuntime::MapQuickView* mapView);
    QString currentGridColor() const;
    void setCurrentGridColor(const QString& gridColor);
    QString currentLabelColor() const;
    void setCurrentLabelColor(const QString& labelColor);
    QString currentLabelFormat() const;
    void setCurrentLabelFormat(const QString& labelFormat);
    QString currentLabelPosition() const;
    void setCurrentLabelPosition(const QString& labelPosition);
    bool gridVisibility();
    void setGridVisibility(bool visible);
    bool gridLabelVisibility();
    void setGridLabelVisibility(bool visible);

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  static const QString s_utmGrid;
  static const QString s_usngGrid;
  static const QString s_latlonGrid;
  static const QString s_mgrsGrid;
  static const QString s_ddFormat;
  static const QString s_dmsFormat;
  static const QString s_geographicPosition;
  static const QString s_bottomLeftPosition;
  static const QString s_bottomRightPosition;
  static const QString s_topLeftPosition;
  static const QString s_topRightPosition;
  static const QString s_centerPosition;
  static const QString s_allSidesPosition;
  QString m_currentGridColor = QStringLiteral("red");
  QString m_currentLabelColor = QStringLiteral("black");
  QString m_currentLabelFormat;
  QString m_currentLabelPosition;
  bool m_gridVisibility = true;
  bool m_gridLabelVisibility = true;
};

#endif // DISPLAYGRID_H
