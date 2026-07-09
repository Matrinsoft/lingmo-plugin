#include <LingmoPlugin/PluginManager.h>
#include <LingmoPlugin/PluginMetadata.h>

#include <QTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>

class tst_PluginManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInstance()
    {
        auto *mgr = Lingmo::PluginManager::instance();
        QVERIFY(mgr != nullptr);
        QCOMPARE(Lingmo::PluginManager::instance(), mgr);
    }

    void testAddSearchPath()
    {
        auto *mgr = Lingmo::PluginManager::instance();
        const int before = mgr->searchPaths().size();
        mgr->addSearchPath(QStringLiteral("/tmp/test-plugins"));
        QCOMPARE(mgr->searchPaths().size(), before + 1);
        QCOMPARE(mgr->searchPaths().first(), QStringLiteral("/tmp/test-plugins"));
    }

    void testDiscoverInDir()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        // Copy test plugin metadata to temp dir
        const QString src = QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin");
        const QString dst = dir.filePath(QStringLiteral("valid-plugin.lingmo-plugin"));
        QVERIFY(QFile::copy(src, dst));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir.path());
        mgr->discover();

        QVERIFY(mgr->hasPlugin(QStringLiteral("org.lingmo.test.plugin")));
        QCOMPARE(mgr->pluginCount(), 1);

        delete mgr;
    }

    void testDiscoverSkipsInvalid()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        // Copy invalid plugin metadata
        const QString src = QStringLiteral(TESTDATA_DIR "/invalid-plugin.lingmo-plugin");
        const QString dst = dir.filePath(QStringLiteral("invalid.lingmo-plugin"));
        QVERIFY(QFile::copy(src, dst));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir.path());
        mgr->discover();

        QVERIFY(!mgr->hasPlugin(QStringLiteral("org.lingmo.test.invalid")));
        QCOMPARE(mgr->pluginCount(), 0);

        delete mgr;
    }

    void testPluginIds()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const QString src = QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin");
        QVERIFY(QFile::copy(src, dir.filePath(QStringLiteral("p1.lingmo-plugin"))));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir.path());
        mgr->discover();

        const auto ids = mgr->pluginIds();
        QCOMPARE(ids.size(), 1);
        QVERIFY(ids.contains(QStringLiteral("org.lingmo.test.plugin")));

        delete mgr;
    }

    void testMetadata()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const QString src = QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin");
        QVERIFY(QFile::copy(src, dir.filePath(QStringLiteral("p.lingmo-plugin"))));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir.path());
        mgr->discover();

        const auto meta = mgr->metadata(QStringLiteral("org.lingmo.test.plugin"));
        QVERIFY(meta.isValid());
        QCOMPARE(meta.type(), QStringLiteral("test"));

        delete mgr;
    }

    void testPluginsOfType()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const QString src = QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin");
        QVERIFY(QFile::copy(src, dir.filePath(QStringLiteral("p.lingmo-plugin"))));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir.path());
        mgr->discover();

        const auto testPlugins = mgr->pluginsOfType(QStringLiteral("test"));
        QCOMPARE(testPlugins.size(), 1);

        const auto otherPlugins = mgr->pluginsOfType(QStringLiteral("applet"));
        QCOMPARE(otherPlugins.size(), 0);

        delete mgr;
    }

    void testLoadMissingPlugin()
    {
        auto *mgr = new Lingmo::PluginManager(this);
        QSignalSpy errorSpy(mgr, &Lingmo::PluginManager::pluginError);

        QVERIFY(!mgr->loadPlugin(QStringLiteral("nonexistent")));
        QCOMPARE(errorSpy.count(), 1);

        delete mgr;
    }

    void testDiscoverClearsPrevious()
    {
        QTemporaryDir dir1;
        QTemporaryDir dir2;
        QVERIFY(dir1.isValid() && dir2.isValid());

        QVERIFY(QFile::copy(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"),
                            dir1.filePath(QStringLiteral("p1.lingmo-plugin"))));

        auto *mgr = new Lingmo::PluginManager(this);
        mgr->addSearchPath(dir1.path());
        mgr->discover();
        QCOMPARE(mgr->pluginCount(), 1);

        // Re-discover with empty dir should clear
        mgr->addSearchPath(dir2.path());
        mgr->discover();
        QCOMPARE(mgr->pluginCount(), 0);

        delete mgr;
    }
};

QTEST_MAIN(tst_PluginManager)
#include "tst_PluginManager.moc"
