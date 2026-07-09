# lingmo-plugin

Lingmo Desktop - Plugin system library.

## Overview

`lingmo-plugin` provides the plugin infrastructure for Lingmo Desktop:

- **PluginMetadata**: JSON metadata parser for `.lingmo-plugin` files
- **PluginLoader**: shared library loading with factory symbol resolution
- **PluginManager**: discovery, registration, version checking, lazy loading

## Plugin Format

Each plugin requires a `.lingmo-plugin` JSON file:

```json
{
    "id": "org.lingmo.applet.clock",
    "type": "applet",
    "version": "1.0.0",
    "api": {
        "name": "org.lingmo.applet",
        "version": "3.0"
    },
    "name": { "default": "Digital Clock" },
    "library": "liblingmo_applet_clock.so",
    "dependencies": []
}
```

## Dependencies

- Qt 6 (Core)

## License

GPL-2.0-or-later
