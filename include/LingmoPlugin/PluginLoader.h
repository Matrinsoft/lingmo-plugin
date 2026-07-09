#pragma once

#include <LingmoPlugin/LingmoPluginExport.h>

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <memory>

namespace Lingmo {

class PluginMetadata;

// Loads a plugin shared library and resolves its factory symbol.
//
// Each plugin .so must export:
//   extern "C" QObject* lingmo_plugin_factory();
//
// Usage:
//   PluginLoader loader;
//   if (loader.load(metadata)) {
//       QObject *instance = loader.createInstance();
//   }
class LINGMOPLUGIN_EXPORT PluginLoader : public QObject
{
    Q_OBJECT

public:
    explicit PluginLoader(QObject *parent = nullptr);
    ~PluginLoader() override;

    // Load a plugin from metadata (reads library path from metadata)
    bool load(const PluginMetadata &metadata);

    // Load a plugin from a specific .so path
    bool loadFromPath(const QString &libraryPath);

    // Create an instance via the factory symbol
    QObject *createInstance();

    // Unload the plugin
    void unload();

    bool isLoaded() const;
    QString errorString() const;

Q_SIGNALS:
    void loaded(const QString &pluginId);
    void unloaded(const QString &pluginId);
    void error(const QString &pluginId, const QString &message);

private:
    std::unique_ptr<class PluginLoaderPrivate> d;
};

} // namespace Lingmo
