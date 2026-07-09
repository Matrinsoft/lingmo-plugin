#pragma once

#include <LingmoPlugin/PluginManager.h>
#include <LingmoPlugin/PluginLoader.h>
#include <LingmoPlugin/PluginMetadata.h>

#include <QMap>
#include <QStringList>

namespace Lingmo {

struct PluginInfo {
    PluginMetadata metadata;
    QScopedPointer<PluginLoader> loader;
    bool loaded = false;
};

class PluginManagerPrivate
{
public:
    QStringList searchPaths;
    QMap<QString, PluginInfo> plugins;
};

} // namespace Lingmo
