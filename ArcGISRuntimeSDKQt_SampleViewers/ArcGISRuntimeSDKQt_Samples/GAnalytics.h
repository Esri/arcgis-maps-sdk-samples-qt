#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QDateTime>
#include <QObject>
#include <QVariantMap>

class GAnalytics : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString clientId READ clientId CONSTANT)
  Q_PROPERTY(QString apiSecret READ apiSecret WRITE setApiSecret NOTIFY apiSecretChanged)
  Q_PROPERTY(QString measurementId READ measurementId WRITE setMeasurementId NOTIFY measurementIdChanged)
  Q_PROPERTY(bool telemetryEnabled READ telemetryEnabled WRITE setTelemetryEnabled NOTIFY telemetryEnabledChanged)
  Q_PROPERTY(bool isVisible READ isVisible WRITE setIsVisible NOTIFY isVisibleChanged)

public:
  explicit GAnalytics(QObject* parent = nullptr);

  ~GAnalytics() override;

  Q_INVOKABLE virtual void init();
  Q_INVOKABLE void postEvent(const QString eventName, QVariantMap parameters);

signals:
  void apiSecretChanged();
  void measurementIdChanged();
  void telemetryEnabledChanged();
  void isVisibleChanged();

private:
  void setApiSecret(const QString apiSecret);
  void setMeasurementId(const QString measurementId);
  void setTelemetryEnabled(const bool telemetryEnabled);
  void setIsVisible(const bool isVisible);
  void startSession();
  void endSession();
  void generateClientId();
  QString apiSecret() const;
  QString clientId() const;
  QString measurementId() const;
  bool telemetryEnabled() const;
  bool isVisible() const;
  QString m_apiSecret;
  QString m_measurementId;
  QString m_clientId;
  qint64 m_startTime;
  bool m_telemetryEnabled;
  bool m_isVisible;
};

#endif // GANALYTICS_H
