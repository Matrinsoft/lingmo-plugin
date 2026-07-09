#pragma once

#include <LingmoPlugin/PluginMetadata.h>

#include <QVariantMap>

namespace Lingmo {

struct PluginMetadata::Impl {
    QVariantMap data;
    bool valid = false;
};

} // namespace Lingmo
