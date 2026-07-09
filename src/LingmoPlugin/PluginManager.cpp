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
            if (d->plugins.count(pluginId)) continue;

            PluginInfo info;
            info.metadata = std::move(metadata);
            d->plugins.emplace(pluginId, std::move(info));
        }
    }

    emit pluginCountChanged();
}

QStringList PluginManager::pluginIds() const
{
    QStringList ids;
    ids.reserve(static_cast<int>(d->plugins.size()));
    for (const auto &[key, value] : d->plugins) {
        ids.append(key);
    }
    return ids;
}

PluginMetadata PluginManager::metadata(const QString &pluginId) const
{
    auto it = d->plugins.find(pluginId);
    if (it != d->plugins.end())
        return it->second.metadata;
    return {};
}

bool PluginManager::hasPlugin(const QString &pluginId) const
{
    return d->plugins.count(pluginId) > 0;
}

bool PluginManager::loadPlugin(const QString &pluginId)
{
    auto it = d->plugins.find(pluginId);
    if (it == d->plugins.end()) {
        emit pluginError(pluginId, QStringLiteral("Plugin not found"));
        return false;
    }

    if (it->second.loaded) return true;

    const auto deps = it->second.metadata.dependencies();
    for (const auto &dep : deps) {
        if (!hasPlugin(dep.id)) {
            emit pluginError(pluginId, QStringLiteral("Missing dependency: %1").arg(dep.id));
            return false;
        }
        if (!isLoaded(dep.id)) {
            if (!loadPlugin(dep.id)) {
                emit pluginError(pluginId, QStringLiteral("Failed to load dependency: %1").arg(dep.id));
                return false;
            }
        }
    }

    it->second.loader = std::make_unique<PluginLoader>();

    connect(it->second.loader.get(), &PluginLoader::error,
            this, [this, pluginId](const QString &msg) {
                emit pluginError(pluginId, msg);
            });

    if (!it->second.loader->load(it->second.metadata)) {
        it->second.loader.reset();
        return false;
    }

    it->second.loaded = true;
    emit pluginLoaded(pluginId);
    return true;
}

QObject *PluginManager::plugin(const QString &pluginId) const
{
    auto it = d->plugins.find(pluginId);
    if (it == d->plugins.end() || !it->second.loaded || !it->second.loader)
        return nullptr;

    return it->second.loader->createInstance();
}

bool PluginManager::isLoaded(const QString &pluginId) const
{
    auto it = d->plugins.find(pluginId);
    return it != d->plugins.end() && it->second.loaded;
}

void PluginManager::unloadPlugin(const QString &pluginId)
{
    auto it = d->plugins.find(pluginId);
    if (it == d->plugins.end() || !it->second.loaded)
        return;

    if (it->second.loader) {
        it->second.loader->unload();
        it->second.loader.reset();
    }
    it->second.loaded = false;
    emit pluginUnloaded(pluginId);
}

QStringList PluginManager::pluginsOfType(const QString &type) const
{
    QStringList result;
    for (const auto &[key, value] : d->plugins) {
        if (value.metadata.type() == type)
            result.append(key);
    }
    return result;
}

int PluginManager::pluginCount() const
{
    return static_cast<int>(d->plugins.size());
}

} // namespace Lingmo
