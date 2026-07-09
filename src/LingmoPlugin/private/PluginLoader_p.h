#pragma once

#include <LingmoPlugin/PluginLoader.h>

#include <QLibrary>
#include <QString>

namespace Lingmo {

class PluginLoaderPrivate
{
public:
    QLibrary library;
    QString errorString;
    QObject *instance = nullptr;
    using FactoryFunc = QObject *(*)();
    FactoryFunc factory = nullptr;
};

} // namespace Lingmo
