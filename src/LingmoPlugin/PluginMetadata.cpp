#include "private/PluginMetadata_p.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Lingmo {

PluginMetadata::PluginMetadata()
    : d(std::make_unique<Impl>())
{
}

PluginMetadata::PluginMetadata(const QString &filePath)
    : d(std::make_unique<Impl>())
{
    loadFromFile(filePath);
}

PluginMetadata::~PluginMetadata() = default;

bool PluginMetadata::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const QByteArray content = file.readAll();
    file.close();

    return loadFromString(QString::fromUtf8(content));
}

bool PluginMetadata::loadFromString(const QString &json)
{
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (doc.isNull() || !doc.isObject())
        return false;

    d->data = doc.object().toVariantMap();

    // Validate required fields
    const bool hasRequired = d->data.contains(QStringLiteral("id"))
        && d->data.contains(QStringLiteral("type"))
        && d->data.contains(QStringLiteral("version"))
        && d->data.contains(QStringLiteral("library"));

    // Validate api block
    if (hasRequired) {
        const QVariantMap api = d->data.value(QStringLiteral("api")).toMap();
        if (!api.contains(QStringLiteral("name"))
            || !api.contains(QStringLiteral("version"))) {
            d->valid = false;
            return false;
        }
    }

    d->valid = hasRequired;
    return d->valid;
}

bool PluginMetadata::isValid() const { return d->valid; }

QString PluginMetadata::id() const
{
    return d->data.value(QStringLiteral("id")).toString();
}

QString PluginMetadata::type() const
{
    return d->data.value(QStringLiteral("type")).toString();
}

QString PluginMetadata::version() const
{
    return d->data.value(QStringLiteral("version")).toString();
}

QString PluginMetadata::apiName() const
{
    return d->data.value(QStringLiteral("api")).toMap()
        .value(QStringLiteral("name")).toString();
}

QString PluginMetadata::apiVersion() const
{
    return d->data.value(QStringLiteral("api")).toMap()
        .value(QStringLiteral("version")).toString();
}

QString PluginMetadata::library() const
{
    return d->data.value(QStringLiteral("library")).toString();
}

QString PluginMetadata::name(const QString &locale) const
{
    const QVariantMap names = d->data.value(QStringLiteral("name")).toMap();
    if (!locale.isEmpty() && names.contains(locale))
        return names.value(locale).toString();
    return names.value(QStringLiteral("default")).toString();
}

QString PluginMetadata::description(const QString &locale) const
{
    Q_UNUSED(locale);
    return d->data.value(QStringLiteral("description")).toString();
}

QString PluginMetadata::icon() const
{
    return d->data.value(QStringLiteral("icon")).toString();
}

QString PluginMetadata::category() const
{
    return d->data.value(QStringLiteral("category")).toString();
}

QList<PluginMetadata::Dependency> PluginMetadata::dependencies() const
{
    QList<Dependency> deps;
    const QJsonArray arr = QJsonArray::fromVariantList(
        d->data.value(QStringLiteral("dependencies")).toList());
    for (const auto &item : arr) {
        const QVariantMap dep = item.toObject().toVariantMap();
        Dependency d;
        d.id = dep.value(QStringLiteral("id")).toString();
        d.version = dep.value(QStringLiteral("version")).toString();
        if (!d.id.isEmpty())
            deps.append(d);
    }
    return deps;
}

QVariantMap PluginMetadata::rawMetadata() const
{
    return d->data;
}

QString PluginMetadata::factorySymbol() const
{
    return d->data.value(QStringLiteral("factorySymbol"),
                         QStringLiteral("lingmo_plugin_factory")).toString();
}

} // namespace Lingmo
