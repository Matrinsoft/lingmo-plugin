#include "private/PluginManager_p.h"
#include <LingmoPlugin/PluginLoader.h>
#include <LingmoPlugin/PluginMetadata.h>

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>

namespace Lingmo {

static PluginManager *s_instance = nullptr;

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<PluginManagerPrivate>())
{
    // Add default search paths
    const QString envPath = QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("LINGMO_PLUGINS_PATH"));
    if (!envPath.isEmpty()) {
        d->searchPaths << envPath.split(QLatin1Char(':'), Qt::SkipEmptyParts);
    }

    d->searchPaths << QDir::homePath() + QStringLiteral("/.local/lib/lingmo/plugins");
    d->searchPaths << QStringLiteral("/usr/lib/lingmo/plugins");
    d->searchPaths << QStringLiteral("/usr/lib64/lingmo/plugins");
}

PluginManager::~PluginManager() = default;

PluginManager *PluginManager::instance()
{
    if (!s_instance) {
        s_instance = new PluginManager;
    }
    return s_instance;
}

void PluginManager::addSearchPath(const QString &path)
{
    if (!d->searchPaths.contains(path)) {
        d->searchPaths.prepend(path);
    }
}

QStringList PluginManager::searchPaths() const
{
    return d->searchPaths;
}

void PluginManager::discover()
{
    d->plugins.clear();

    for (const QString &searchPath : d->searchPaths) {
        const QDir dir(searchPath);
        if (!dir.exists()) continue;

        const QStringList entries = dir.entryList(
            QStringList() << QStringLiteral("*.lingmo-plugin"),
            QDir::Files);

        for (const QString &entry : entries) {
            const QString filePath = dir.absoluteFilePath(entry);

            PluginMetadata metadata(filePath);
            if (!metadata.isValid()) continue;

            const QString pluginId = metadata.id();
            if (d->plugins.contains(pluginId)) continue;

            PluginInfo info;
            info.metadata = std::move(metadata);
            d->plugins.insert(pluginId, std::move(info));
        }
    }

    emit pluginCountChanged();
}

QStringList PluginManager::pluginIds() const
{
    return d->plugins.keys();
}

PluginMetadata PluginManager::metadata(const QString &pluginId) const
{
    return d->plugins.value(pluginId).metadata;
}

bool PluginManager::hasPlugin(const QString &pluginId) const
{
    return d->plugins.contains(pluginId);
}

bool PluginManager::loadPlugin(const QString &pluginId)
{
    auto it = d->plugins.find(pluginId);
    if (it == d->plugins.end()) {
        emit pluginError(pluginId, QStringLiteral("Plugin not found"));
        return false;
    }

    if (it->loaded) return true;

    // Check dependencies
    const auto deps = it->metadata.dependencies();
    for (const auto &dep : deps) {
        if (!hasPlugin(dep.id)) {
            const QString msg = QStringLiteral("Missing dependency: %1").arg(dep.id);
            emit pluginError(pluginId, msg);
            return false;
        }
        if (!isLoaded(dep.id)) {
            if (!loadPlugin(dep.id)) {
                const QString msg = QStringLiteral("Failed to load dependency: %1").arg(dep.id);
                emit pluginError(pluginId, msg);
                return false;
            }
        }
    }

    // Create loader and load
    it->loader.reset(new PluginLoader);

    connect(it->loader.get(), &PluginLoader::error,
            this, [this, pluginId](const QString &msg) {
                emit pluginError(pluginId, msg);
            });

    if (!it->loader->load(it->metadata)) {
        it->loader.reset();
        return false;
    }

    it->loaded = true;
    emit pluginLoaded(pluginId);
    return true;
}

QObject *PluginManager::plugin(const QString &pluginId) const
{
    auto it = d->plugins.constFind(pluginId);
    if (it == d->plugins.constEnd() || !it->loaded || !it->loader)
        return nullptr;

    return it->loader->createInstance();
}

bool PluginManager::isLoaded(const QString &pluginId) const
{
    auto it = d->plugins.constFind(pluginId);
    return it != d->plugins.constEnd() && it->loaded;
}

void PluginManager::unloadPlugin(const QString &pluginId)
{
    auto it = d->plugins.find(pluginId);
    if (it == d->plugins.end() || !it->loaded)
        return;

    if (it->loader) {
        it->loader->unload();
        it->loader.reset();
    }
    it->loaded = false;
    emit pluginUnloaded(pluginId);
}

QStringList PluginManager::pluginsOfType(const QString &type) const
{
    QStringList result;
    for (auto it = d->plugins.constBegin(); it != d->plugins.constEnd(); ++it) {
        if (it->metadata.type() == type)
            result.append(it.key());
    }
    return result;
}

int PluginManager::pluginCount() const
{
    return d->plugins.size();
}

} // namespace Lingmo
