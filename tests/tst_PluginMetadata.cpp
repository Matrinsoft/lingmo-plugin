#include <LingmoPlugin/PluginMetadata.h>

#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

class tst_PluginMetadata : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLoadValidPlugin()
    {
        Lingmo::PluginMetadata meta;
        QVERIFY(meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin")));
        QVERIFY(meta.isValid());

        QCOMPARE(meta.id(), QStringLiteral("org.lingmo.test.plugin"));
        QCOMPARE(meta.type(), QStringLiteral("test"));
        QCOMPARE(meta.version(), QStringLiteral("1.0.0"));
        QCOMPARE(meta.apiName(), QStringLiteral("org.lingmo.test"));
        QCOMPARE(meta.apiVersion(), QStringLiteral("1.0"));
        QCOMPARE(meta.library(), QStringLiteral("liblingmo_test_plugin.so"));
    }

    void testNameLocalization()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"));

        QCOMPARE(meta.name(), QStringLiteral("Test Plugin"));
        QCOMPARE(meta.name(QStringLiteral("zh_CN")), QStringLiteral("测试插件"));
    }

    void testOptionalFields()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"));

        QCOMPARE(meta.description(), QStringLiteral("A test plugin for unit tests"));
        QCOMPARE(meta.icon(), QStringLiteral("test-icon"));
        QCOMPARE(meta.category(), QStringLiteral("test"));
    }

    void testDependencies()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/plugin-with-deps.lingmo-plugin"));
        QVERIFY(meta.isValid());

        const auto deps = meta.dependencies();
        QCOMPARE(deps.size(), 1);
        QCOMPARE(deps.at(0).id, QStringLiteral("org.lingmo.test.plugin"));
        QCOMPARE(deps.at(0).version, QStringLiteral(">=1.0"));
    }

    void testNoDependencies()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"));

        const auto deps = meta.dependencies();
        QVERIFY(deps.isEmpty());
    }

    void testInvalidMetadata()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/invalid-plugin.lingmo-plugin"));
        QVERIFY(!meta.isValid());
    }

    void testLoadFromString()
    {
        Lingmo::PluginMetadata meta;
        const QString json = QStringLiteral(R"({
            "id": "org.lingmo.test.inline",
            "type": "test",
            "version": "2.0.0",
            "api": { "name": "org.lingmo.test", "version": "2.0" },
            "library": "libinline.so"
        })");
        QVERIFY(meta.loadFromString(json));
        QVERIFY(meta.isValid());
        QCOMPARE(meta.id(), QStringLiteral("org.lingmo.test.inline"));
    }

    void testInvalidJson()
    {
        Lingmo::PluginMetadata meta;
        QVERIFY(!meta.loadFromString(QStringLiteral("not json at all")));
        QVERIFY(!meta.isValid());
    }

    void testMissingRequiredFields()
    {
        Lingmo::PluginMetadata meta;
        const QString json = QStringLiteral(R"({
            "id": "org.lingmo.test.incomplete"
        })");
        QVERIFY(!meta.loadFromString(json));
    }

    void testNonexistentFile()
    {
        Lingmo::PluginMetadata meta;
        QVERIFY(!meta.loadFromFile(QStringLiteral("/nonexistent/file.lingmo-plugin")));
        QVERIFY(!meta.isValid());
    }

    void testRawMetadata()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"));

        const QVariantMap raw = meta.rawMetadata();
        QVERIFY(raw.contains(QStringLiteral("id")));
        QVERIFY(raw.contains(QStringLiteral("type")));
        QCOMPARE(raw.value(QStringLiteral("id")).toString(),
                 QStringLiteral("org.lingmo.test.plugin"));
    }

    void testFactorySymbol()
    {
        Lingmo::PluginMetadata meta;
        meta.loadFromFile(QStringLiteral(TESTDATA_DIR "/valid-plugin.lingmo-plugin"));
        QCOMPARE(meta.factorySymbol(), QStringLiteral("lingmo_plugin_factory"));
    }
};

QTEST_MAIN(tst_PluginMetadata)
#include "tst_PluginMetadata.moc"
