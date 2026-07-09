#pragma once

#include <LingmoPlugin/LingmoPluginExport.h>

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QExplicitlySharedDataPointer>
#include <QSharedData>

namespace Lingmo {

class PluginMetadataData : public QSharedData
{
public:
    QVariantMap data;
    bool valid = false;
};

// Parsed metadata from a .lingmo-plugin JSON file.
// Uses Qt explicit sharing (copy-on-write) — safe to store in containers.
class LINGMOPLUGIN_EXPORT PluginMetadata
{
public:
    PluginMetadata();
    explicit PluginMetadata(const QString &filePath);
    PluginMetadata(const PluginMetadata &other);
    PluginMetadata &operator=(const PluginMetadata &other);
    ~PluginMetadata();

    bool loadFromFile(const QString &filePath);
    bool loadFromString(const QString &json);

    bool isValid() const;

    QString id() const;
    QString type() const;
    QString version() const;
    QString apiName() const;
    QString apiVersion() const;
    QString library() const;

    QString name(const QString &locale = {}) const;
    QString description(const QString &locale = {}) const;
    QString icon() const;
    QString category() const;

    struct Dependency {
        QString id;
        QString version;
    };
    QList<Dependency> dependencies() const;

    QVariantMap rawMetadata() const;
    QString factorySymbol() const;

private:
    QExplicitlySharedDataPointer<PluginMetadataData> d;
};

} // namespace Lingmo
