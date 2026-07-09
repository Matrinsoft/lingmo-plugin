#include "private/PluginLoader_p.h"
#include <LingmoPlugin/PluginMetadata.h>

#include <QDir>
#include <QFileInfo>

namespace Lingmo {

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<PluginLoaderPrivate>())
{
}

PluginLoader::~PluginLoader()
{
    unload();
}

bool PluginLoader::load(const PluginMetadata &metadata)
{
    if (!metadata.isValid()) {
        d->errorString = QStringLiteral("Invalid plugin metadata");
        emit error(metadata.id(), d->errorString);
        return false;
    }

    // Find library in standard paths
    const QString libName = metadata.library();
    const QStringList searchPaths = {
        QStringLiteral("/usr/lib/lingmo/plugins"),
        QStringLiteral("/usr/lib64/lingmo/plugins"),
        QDir::homePath() + QStringLiteral("/.local/lib/lingmo/plugins"),
    };

    QString fullPath;
    for (const QString &path : searchPaths) {
        const QString candidate = path + QLatin1Char('/') + libName;
        if (QFileInfo::exists(candidate)) {
            fullPath = candidate;
            break;
        }
    }

    // Also try as absolute path or relative to CWD
    if (fullPath.isEmpty() && QFileInfo::exists(libName))
        fullPath = libName;

    if (fullPath.isEmpty()) {
        d->errorString = QStringLiteral("Library not found: %1").arg(libName);
        emit error(metadata.id(), d->errorString);
        return false;
    }

    return loadFromPath(fullPath);
}

bool PluginLoader::loadFromPath(const QString &libraryPath)
{
    d->library.setFileName(libraryPath);

    if (!d->library.load()) {
        d->errorString = d->library.errorString();
        return false;
    }

    // Resolve factory symbol
    d->factory = reinterpret_cast<PluginLoaderPrivate::FactoryFunc>(
        d->library.resolve("lingmo_plugin_factory"));

    if (!d->factory) {
        d->errorString = QStringLiteral("Symbol 'lingmo_plugin_factory' not found");
        d->library.unload();
        return false;
    }

    return true;
}

QObject *PluginLoader::createInstance()
{
    if (!d->factory) return nullptr;

    d->instance = d->factory();
    return d->instance;
}

void PluginLoader::unload()
{
    if (d->instance) {
        delete d->instance;
        d->instance = nullptr;
    }
    if (d->library.isLoaded()) {
        d->library.unload();
    }
    d->factory = nullptr;
}

bool PluginLoader::isLoaded() const
{
    return d->library.isLoaded();
}

QString PluginLoader::errorString() const
{
    return d->errorString;
}

} // namespace Lingmo
