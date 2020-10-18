#include "homeassistantapi.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkReply>

HomeassistantApi::HomeassistantApi(Wallet *wallet, QObject *parent) :
    ApiInterface(wallet, parent)
{

}

void HomeassistantApi::callService(const QString &domain, const QString &service, const QString &entityId, const QJsonObject &data)
{
    QJsonObject payload = data;
    payload.insert(QStringLiteral("entity_id"), entityId);

    request(getRequest(QStringLiteral(HASS_API_ENDPOINT_SERVICES) + "/" + domain + "/" + service), payload);
}

void HomeassistantApi::getConfig()
{
    request(getRequest(QStringLiteral(HASS_API_ENDPOINT_CONFIG)));
}

void HomeassistantApi::getState(const QString &entityId)
{
    request(getRequest(QStringLiteral(HASS_API_ENDPOINT_STATES) + "/" + entityId));
}

void HomeassistantApi::getStates()
{
    request(getRequest(HASS_API_ENDPOINT_STATES));
}

void HomeassistantApi::registerDevice(Device *device)
{
    if (!device)
        return;

    QJsonObject data;
    data.insert(QStringLiteral("device_id"), device->id());
    data.insert(QStringLiteral("app_id"), "org.nubecula.harbour.quartermaster");
    data.insert(QStringLiteral("app_name"), QCoreApplication::applicationName());
    data.insert(QStringLiteral("app_version"), QCoreApplication::applicationVersion());
    data.insert(QStringLiteral("device_name"), device->name());
    data.insert(QStringLiteral("manufacturer"), device->manufacturer());
    data.insert(QStringLiteral("model"), device->model());
    data.insert(QStringLiteral("os_name"), device->softwareName());
    data.insert(QStringLiteral("os_version"), device->softwareVersion());
    data.insert(QStringLiteral("supports_encryption"), device->encryption());

    request(getRequest(QStringLiteral(HASS_API_ENDPOINT_DEVICE_REGISTRATION)), data);
}

QNetworkRequest HomeassistantApi::getRequest(const QString &endpoint)
{
    QNetworkRequest request = ApiInterface::getRequest(endpoint);
    request.setRawHeader("Authorization", "Bearer " + wallet()->token().toLatin1());

    m_activeRequests.append(endpoint);

    return request;
}

void HomeassistantApi::onRequestFinished(QNetworkReply *reply)
{
    if (!reply)
        return;

#ifdef QT_DEBUG
    qDebug() << "API REPLY";
#endif

    // read data
    const QString endpoint = reply->url().toString().remove(baseUrl());
    const QByteArray &data = reply->readAll();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    m_activeRequests.removeAll(endpoint);

#ifdef QT_DEBUG
    qDebug() << endpoint;
    qDebug() << data;
    qDebug() << status;
#endif

    // handel errors
    if (reply->error()) {
#ifdef QT_DEBUG
        qDebug() << reply->errorString();
#endif

        reply->deleteLater();
        return;
    }

    // delete reply
    reply->deleteLater();

    QJsonParseError error{};

    const QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error) {
#ifdef QT_DEBUG
        qDebug() << "JSON PARSE ERROR";
        qDebug() << error.errorString();
#endif
        return;
    }

    // logging
    if (logging())
        logData(QStringLiteral("rest_api_reply"), doc.toJson(QJsonDocument::Indented));

    emit dataAvailable(endpoint, doc);
}

