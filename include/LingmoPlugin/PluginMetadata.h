#pragma once

#include <LingmoPlugin/LingmoPluginExport.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace Lingmo {

// Parsed metadata from a .lingmo-plugin JSON file.
//
// Example .lingmo-plugin file:
// {
//     "id": "org.lingmo.applet.clock",
//     "type": "applet",
//     "version": "1.0.0",
//     "api": {
//         "name": "org.lingmo.applet",
//         "version": "3.0"
//     },
//     "name": { "default": "Digital Clock" },
//     "library": "liblingmo_applet_clock.so",
//     "dependencies": []
// }
class LINGMOPLUGIN_EXPORT PluginMetadata
{
public:
    PluginMetadata();
    explicit PluginMetadata(const QString &filePath);
    ~PluginMetadata();

    // Parse a .lingmo-plugin JSON file
    bool loadFromFile(const QString &filePath);

    // Parse from JSON string
    bool loadFromString(const QString &json);

    bool isValid() const;

    // Required fields
    QString id() const;
    QString type() const;
    QString version() const;
    QString apiName() const;
    QString apiVersion() const;
    QString library() const;

    // Optional fields
    QString name(const QString &locale = {}) const;
    QString description(const QString &locale = {}) const;
    QString icon() const;
    QString category() const;

    // Dependencies
    struct Dependency {
        QString id;
        QString version; // semver range, e.g. ">=1.0 <2.0"
    };
    QList<Dependency> dependencies() const;

    // All raw metadata values
    QVariantMap rawMetadata() const;

    // Factory symbol name (default: "lingmo_plugin_factory")
    QString factorySymbol() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace Lingmo
