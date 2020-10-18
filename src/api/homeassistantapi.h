#ifndef HOMEASSISTANTAPI_H
#define HOMEASSISTANTAPI_H

#include "apiinterface.h"

#define     HASS_API_ENDPOINT_CONFIG                    "/api/config"
#define     HASS_API_ENDPOINT_DEVICE_REGISTRATION       "/api/mobile_app/registrations"
#define     HASS_API_ENDPOINT_SERVICES                  "/api/services"
#define     HASS_API_ENDPOINT_STATES                    "/api/states"

#include "src/crypto/secrets.h"
#include "src/device/device.h"

class HomeassistantApi : public ApiInterface
{
    Q_OBJECT

public:
    explicit HomeassistantApi(Wallet *wallet, QObject *parent = nullptr);

    // api calls
    void callService(const QString &domain, const QString &service, const QString &entityId, const QJsonObject &data = QJsonObject());
    void getConfig();
    void getState(const QString &entityId);
    void getStates();
    void registerDevice(Device *device);

private:
    QStringList m_activeRequests;

    // ApiInterface interface
public:
    QNetworkRequest getRequest(const QString &endpoint) override;

private slots:
    void onRequestFinished(QNetworkReply *reply) override;
};

#endif // HOMEASSISTANTAPI_H
