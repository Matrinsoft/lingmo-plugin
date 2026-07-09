#pragma once

#include <LingmoPlugin/LingmoPluginExport.h>
#include <LingmoPlugin/PluginMetadata.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <memory>

namespace Lingmo {

class PluginManagerPrivate;

// Central plugin discovery and lifecycle management.
//
// Scans directories for .lingmo-plugin JSON files, validates metadata,
// loads plugins on demand, and manages plugin lifecycle.
//
// Discovery paths (first match wins):
//   1. $LINGMO_PLUGINS_PATH (environment, development)
//   2. ~/.local/lib/lingmo/plugins/ (user-installed)
//   3. /usr/lib/lingmo/plugins/ (system-installed)
//
// Usage:
//   auto *mgr = Lingmo::PluginManager::instance();
//   mgr->addSearchPath("/usr/lib/lingmo/plugins");
//   mgr->discover();
//   mgr->loadPlugin("org.lingmo.applet.clock");
class LINGMOPLUGIN_EXPORT PluginManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int pluginCount READ pluginCount NOTIFY pluginCountChanged)

public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager() override;

    static PluginManager *instance();

    // Add a directory to scan for plugins
    void addSearchPath(const QString &path);

    // Get all search paths
    QStringList searchPaths() const;

    // Discover all plugins in search paths
    void discover();

    // Get all discovered plugin IDs
    QStringList pluginIds() const;

    // Get metadata for a plugin
    PluginMetadata metadata(const QString &pluginId) const;

    // Check if a plugin is discovered
    bool hasPlugin(const QString &pluginId) const;

    // Load a plugin by ID (lazy — only loads if not already loaded)
    bool loadPlugin(const QString &pluginId);

    // Get a loaded plugin instance by ID
    QObject *plugin(const QString &pluginId) const;

    // Check if a plugin is loaded
    bool isLoaded(const QString &pluginId) const;

    // Unload a plugin
    void unloadPlugin(const QString &pluginId);

    // Get plugins of a specific type
    QStringList pluginsOfType(const QString &type) const;

    int pluginCount() const;

Q_SIGNALS:
    void pluginCountChanged();
    void pluginLoaded(const QString &pluginId);
    void pluginUnloaded(const QString &pluginId);
    void pluginError(const QString &pluginId, const QString &message);

private:
    std::unique_ptr<PluginManagerPrivate> d;
};

} // namespace Lingmo
