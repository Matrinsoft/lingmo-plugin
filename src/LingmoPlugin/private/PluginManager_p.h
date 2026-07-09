#pragma once

#include <LingmoPlugin/PluginManager.h>
#include <LingmoPlugin/PluginLoader.h>
#include <LingmoPlugin/PluginMetadata.h>

#include <map>
#include <memory>
#include <QStringList>

namespace Lingmo {

struct PluginInfo {
    PluginMetadata metadata;
    std::unique_ptr<PluginLoader> loader;
    bool loaded = false;

    PluginInfo() = default;
    PluginInfo(PluginInfo &&) = default;
    PluginInfo &operator=(PluginInfo &&) = default;
    PluginInfo(const PluginInfo &) = delete;
    PluginInfo &operator=(const PluginInfo &) = delete;
};

class PluginManagerPrivate
{
public:
    QStringList searchPaths;
    std::map<QString, PluginInfo> plugins;
};

} // namespace Lingmo
