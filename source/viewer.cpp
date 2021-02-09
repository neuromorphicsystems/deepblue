#include "../third_party/chameleon/source/background_cleaner.hpp"
#include "../third_party/chameleon/source/delta_t_display.hpp"
#include "../third_party/chameleon/source/dvs_display.hpp"
#include "../third_party/tarsier/source/stitch.hpp"
#include "parse_parameters.hpp"
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

/// exposure_measurement encodes an absolute luminance measurement.
SEPIA_PACK(struct exposure_measurement {
    uint64_t delta_t;
    uint16_t x;
    uint16_t y;
});

int main(int argc, char* argv[]) {
    if (argc != 1 && argc != 2) {
        std::cerr << "syntax: viewer [/path/to/parameters.json]" << std::endl;
        return 1;
    }
    const auto parameters =
        argc == 2 ? parse_parameters(sepia::filename_to_ifstream(argv[1])) : sepia::ccamatis::default_parameters;

    QGuiApplication app(argc, argv);
    qmlRegisterType<chameleon::background_cleaner>("Chameleon", 1, 0, "BackgroundCleaner");
    qmlRegisterType<chameleon::dvs_display>("Chameleon", 1, 0, "DvsDisplay");
    qmlRegisterType<chameleon::delta_t_display>("Chameleon", 1, 0, "DeltaTDisplay");
    QQmlApplicationEngine application_engine;
    application_engine.rootContext()->setContextProperty("camera_width", sepia::ccamatis::width);
    application_engine.rootContext()->setContextProperty("camera_height", sepia::ccamatis::height);
    application_engine.loadData(R""(
        import Chameleon 1.0
        import QtQuick 2.7
        import QtQuick.Layouts 1.1
        import QtQuick.Window 2.2
        Window {
            id: window
            visible: true
            width: camera_width * 2
            height: camera_height
            Timer {
                interval: 20
                running: true
                repeat: true
                onTriggered: {
                    dvs_display.trigger_draw();
                    delta_t_display.trigger_draw();
                }
            }
            BackgroundCleaner {
                width: window.width
                height: window.height
            }
            RowLayout {
                width: window.width
                height: window.height
                spacing: 0
                DvsDisplay {
                    objectName: "dvs_display"
                    id: dvs_display
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    canvas_size: Qt.size(camera_width, camera_height)
                }
                DeltaTDisplay {
                    objectName: "delta_t_display"
                    id: delta_t_display
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    canvas_size: Qt.size(camera_width, camera_height)
                }
            }
        }
    )"");
    auto window = qobject_cast<QQuickWindow*>(application_engine.rootObjects().first());
    {
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        window->setFormat(format);
    }
    auto dvs_display = window->findChild<chameleon::dvs_display*>("dvs_display");
    auto delta_t_display = window->findChild<chameleon::delta_t_display*>("delta_t_display");

    auto camera = sepia::ccamatis::make_buffered_camera(
        sepia::ccamatis::decode(
            sepia::make_split<sepia::type::atis>(
                [&](sepia::dvs_event dvs_event) { dvs_display->push(dvs_event); },
                tarsier::make_stitch<sepia::threshold_crossing, exposure_measurement>(
                    sepia::ccamatis::width,
                    sepia::ccamatis::height,
                    [](sepia::threshold_crossing threshold_crossing, uint64_t delta_t) -> exposure_measurement {
                        return {delta_t, threshold_crossing.x, threshold_crossing.y};
                    },
                    [&](exposure_measurement exposure_measurement) { delta_t_display->push(exposure_measurement); })),
            []() {}),
        [](std::exception_ptr exception) {
            try {
                std::rethrow_exception(exception);
            } catch (const std::exception& exception) {
                std::cerr << exception.what() << std::endl;
            }
        },
        parameters);
    return app.exec();
}
