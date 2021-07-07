#include "../third_party/chameleon/source/background_cleaner.hpp"
#include "../third_party/chameleon/source/delta_t_display.hpp"
#include "../third_party/chameleon/source/dvs_display.hpp"
#include "../third_party/roo/sepia.hpp"
#include "../third_party/tarsier/source/replicate.hpp"
#include "../third_party/tarsier/source/stitch.hpp"
#include <QQmlPropertyMap>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <iostream>

/// exposure_measurement encodes an absolute luminance measurement.
SEPIA_PACK(struct exposure_measurement {
    uint64_t delta_t;
    uint16_t x;
    uint16_t y;
});

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "syntax: player /path/to/recording.es" << std::endl;
        return 1;
    }
    const auto header = sepia::read_header(sepia::filename_to_ifstream(argv[1]));
    std::vector<std::pair<std::size_t, uint64_t>> seeks_and_ts;
    uint64_t end_t = 0;
    {
        std::size_t count = 0;
        sepia::join_observable<sepia::type::atis>(
            sepia::filename_to_ifstream(argv[1]),
            [&](sepia::atis_event event) {
                if (seeks_and_ts.empty()) {
                    std::pair<std::size_t, uint64_t> seek_and_t;
                    seek_and_t.first = 0;
                    seek_and_t.second = event.t;
                    seeks_and_ts.push_back(seek_and_t);
                }
                end_t = event.t;
            },
            [&](std::pair<std::size_t, uint64_t> offset_and_previous_t) {
                if (count == 100 - 1) {
                    seeks_and_ts.push_back(offset_and_previous_t);
                } else {
                    ++count;
                }
            });
        if (seeks_and_ts.empty()) {
            seeks_and_ts.emplace_back(0, 0);
        }
    }

    QGuiApplication app(argc, argv);
    qmlRegisterType<chameleon::background_cleaner>("Chameleon", 1, 0, "BackgroundCleaner");
    qmlRegisterType<chameleon::dvs_display>("Chameleon", 1, 0, "DvsDisplay");
    qmlRegisterType<chameleon::delta_t_display>("Chameleon", 1, 0, "DeltaTDisplay");
    QQmlApplicationEngine application_engine;
    application_engine.rootContext()->setContextProperty("header_width", header.width);
    application_engine.rootContext()->setContextProperty("header_height", header.height);
    application_engine.rootContext()->setContextProperty("begin_t", static_cast<quint64>(seeks_and_ts.front().second));
    application_engine.rootContext()->setContextProperty("end_t", static_cast<quint64>(end_t));
    std::atomic_flag accessing_play;
    uint64_t play_t = 0;
    application_engine.rootContext()->setContextProperty("current_t", static_cast<quint64>(play_t));
    accessing_play.clear(std::memory_order_release);
    std::atomic_flag accessing_playback;
    float playback_speed = 1;
    bool playback_speed_updated = false;
    uint64_t playback_t = seeks_and_ts.front().second;
    bool playback_t_updated = false;
    QQmlPropertyMap playback;
    playback.insert("speed", playback_speed);
    playback.insert("t", static_cast<quint64>(playback_t));
    accessing_playback.clear(std::memory_order_release);
    QQmlPropertyMap::connect(
        &playback, &QQmlPropertyMap::valueChanged, &playback, [&](const QString& name, const QVariant& value) {
            while (accessing_playback.test_and_set(std::memory_order_acquire)) {
            }
            if (name == "speed") {
                playback_speed = value.toFloat();
                playback_speed_updated = true;
            } else if (name == "t") {
                playback_t = value.toUInt();
                playback_t_updated = true;
            }
            accessing_playback.clear(std::memory_order_release);
        });
    application_engine.rootContext()->setContextProperty("playback", &playback);
    application_engine.loadData(R""(
        import Chameleon 1.0
        import QtQuick 2.7
        import QtQuick.Layouts 1.1
        import QtQuick.Controls 2.2
        import QtQuick.Window 2.2
        Window {
            id: window
            visible: true
            width: header_width * 2
            height: header_height + 160
            Timer {
                interval: 20
                running: true
                repeat: true
                onTriggered: {
                    dvs_display.trigger_draw()
                    delta_t_display.trigger_draw()
                    if (!slider.pressed && !slider.locked) {
                        slider.value = current_t
                    }
                    t_text.text = current_t.toLocaleString(Qt.locale("en-US"), "f", 0) + " μs"
                }
            }
            BackgroundCleaner {
                width: window.width
                height: window.height
                color: "#272727"
            }
            ColumnLayout {
                width: window.width
                height: window.height
                spacing: 0
                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0
                    DvsDisplay {
                        objectName: "dvs_display"
                        id: dvs_display
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        canvas_size: Qt.size(header_width, header_height)
                    }
                    DeltaTDisplay {
                        objectName: "delta_t_display"
                        id: delta_t_display
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        canvas_size: Qt.size(header_width, header_height)
                    }
                }
                RowLayout {
                    id: control
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 20
                    Layout.bottomMargin: 20
                    Layout.minimumHeight: 40
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    property var speeds: [0.001, 0.003, 0.01, 0.03, 0.1, 0.3, 1, 3, 10]
                    property var speed_index: 6
                    property var playing: true
                    property var update_speed: (function() {
                        if (playing) {
                            playback.speed = speeds[speed_index]
                        } else {
                            playback.speed = 0
                        }
                        speed_text.text = 'x ' + speeds[speed_index].toString()
                    })
                    Button {
                        id: play_button
                        Layout.rightMargin: control.height
                        text: "\u2161"
                        onClicked: {
                            if (control.playing) {
                                text = "\u25b6"
                                control.playing = false
                            } else {
                                text = "\u2161"
                                control.playing = true
                            }
                            control.update_speed()
                        }
                        background: Rectangle {
                            implicitWidth: control.height
                            implicitHeight: control.height
                            color: play_button.down ? "#dddddd" : "#ffffff"
                            radius: control.height / 2
                        }
                    }
                    Button {
                        id: slower_button
                        text: "\u227a"
                        onClicked: {
                            if (control.speed_index > 0) {
                                --control.speed_index
                                if (control.speed_index == 0) {
                                    slower_button.enabled = false
                                }
                                faster_button.enabled = true
                                control.update_speed()
                            }
                        }
                        background: Rectangle {
                            implicitWidth: control.height
                            implicitHeight: control.height
                            color: slower_button.down ? "#dddddd" : "#ffffff"
                            radius: control.height / 2
                        }
                    }
                    Item {
                        implicitWidth: 80
                        Text {
                            id: speed_text
                            anchors.centerIn: parent
                            font.pointSize: 16
                            text: 'x 1'
                            color: "#ffffff"
                        }
                    }
                    Button {
                        id: faster_button
                        text: "\u227b"
                        onClicked: {
                            if (control.speed_index < control.speeds.length - 1) {
                                ++control.speed_index
                                if (control.speed_index == control.speeds.length - 1) {
                                    faster_button.enabled = false
                                }
                                slower_button.enabled = true
                                control.update_speed()
                            }
                        }
                        background: Rectangle {
                            implicitWidth: control.height
                            implicitHeight: control.height
                            color: faster_button.down ? "#dddddd" : "#ffffff"
                            radius: control.height / 2
                        }
                    }
                }
                Slider {
                    id: slider
                    objectName: "slider"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    width: window.width
                    height: 40
                    from: begin_t
                    to: end_t
                    property var locked: false
                    onPressedChanged: {
                        slider.locked = true
                        if (!pressed) {
                            playback.t = value
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    height: 40
                    Text {
                        id: t_text
                        font.pointSize: 16
                        anchors.centerIn: parent
                        text: begin_t.toLocaleString(Qt.locale("en-US"), "f", 0) + " μs"
                        color: "#ffffff"
                    }
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
    auto slider = window->findChild<QObject*>("slider");

    std::atomic_bool running(true);
    application_engine.rootContext()->setContextProperty(
        "current_t", static_cast<quint64>(seeks_and_ts.front().second));
    auto control_loop = std::thread([&]() {
        while (running.load(std::memory_order_acquire)) {
            while (accessing_play.test_and_set(std::memory_order_acquire)) {
            }
            application_engine.rootContext()->setContextProperty("current_t", static_cast<quint64>(play_t));
            accessing_play.clear(std::memory_order_release);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    auto play_loop = std::thread([&]() {
        uint64_t previous_t = 0;
        auto time_reference = std::chrono::high_resolution_clock::now();
        auto split = sepia::make_split<sepia::type::atis>(
            [&](sepia::dvs_event dvs_event) { dvs_display->push(dvs_event); },
            tarsier::make_stitch<sepia::threshold_crossing, exposure_measurement>(
                header.width,
                header.height,
                [](sepia::threshold_crossing threshold_crossing, uint64_t delta_t) -> exposure_measurement {
                    return {delta_t, threshold_crossing.x, threshold_crossing.y};
                },
                [&](exposure_measurement exposure_measurement) { delta_t_display->push(exposure_measurement); }));
        std::unique_ptr<sepia::capture_exception> capture_exception;
        std::unique_ptr<sepia::base_observable> observable;
        while (running.load(std::memory_order_acquire)) {
            while (accessing_playback.test_and_set(std::memory_order_acquire)) {
            }
            auto local_playback_speed = playback_speed;
            playback_speed_updated = false;
            auto local_playback_t = playback_t;
            playback_t_updated = false;
            accessing_playback.clear(std::memory_order_release);
            auto seek_and_t = std::lower_bound(
                seeks_and_ts.begin(),
                seeks_and_ts.end(),
                local_playback_t,
                [](const std::pair<std::size_t, uint64_t>& seek_and_t, uint64_t t) { return seek_and_t.second < t; });
            if (seek_and_t != seeks_and_ts.begin()) {
                seek_and_t = std::prev(seek_and_t);
            }
            try {
                previous_t = 0;
                capture_exception.reset(new sepia::capture_exception());
                {
                    std::vector<sepia::dvs_event> events(
                        header.width * header.height,
                        sepia::dvs_event{std::numeric_limits<uint64_t>::max(), 0, 0, false});
                    dvs_display->assign(events.begin(), events.end());
                }
                {
                    std::vector<uint32_t> delta_ts(header.width * header.height, std::numeric_limits<uint32_t>::max());
                    delta_t_display->assign(delta_ts.begin(), delta_ts.end());
                }
                auto began = false;
                observable = sepia::make_observable<sepia::type::atis>(
                    sepia::filename_to_ifstream(argv[1]),
                    [&](sepia::atis_event event) {
                        event.t += seek_and_t->second;
                        if (event.t > previous_t) {
                            if (!running.load(std::memory_order_acquire)) {
                                throw std::runtime_error("end of stream");
                            }
                            if (event.t >= local_playback_t) {
                                while (accessing_play.test_and_set(std::memory_order_acquire)) {
                                }
                                play_t = event.t;
                                accessing_play.clear(std::memory_order_release);
                            }
                            while (accessing_playback.test_and_set(std::memory_order_acquire)) {
                            }
                            if (playback_speed_updated) {
                                playback_speed_updated = false;
                                local_playback_speed = playback_speed;
                                time_reference = std::chrono::high_resolution_clock::now();
                                local_playback_t = event.t;
                            }
                            if (playback_t_updated) {
                                playback_t_updated = false;
                                local_playback_t = playback_t;
                                accessing_playback.clear(std::memory_order_release);
                                while (accessing_play.test_and_set(std::memory_order_acquire)) {
                                }
                                play_t = local_playback_t;
                                slider->setProperty("locked", false);
                                accessing_play.clear(std::memory_order_release);
                                throw std::runtime_error("end of stream");
                            }
                            accessing_playback.clear(std::memory_order_release);
                            while (local_playback_speed == 0 && running.load(std::memory_order_acquire)) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                while (accessing_playback.test_and_set(std::memory_order_acquire)) {
                                }
                                if (playback_speed_updated) {
                                    playback_speed_updated = false;
                                    local_playback_speed = playback_speed;
                                    time_reference = std::chrono::high_resolution_clock::now();
                                    local_playback_t = event.t;
                                }
                                if (playback_t_updated) {
                                    playback_t_updated = false;
                                    local_playback_t = playback_t;
                                    accessing_playback.clear(std::memory_order_release);
                                    while (accessing_play.test_and_set(std::memory_order_acquire)) {
                                    }
                                    play_t = local_playback_t;
                                    slider->setProperty("locked", false);
                                    accessing_play.clear(std::memory_order_release);
                                    throw std::runtime_error("end of stream");
                                }
                                accessing_playback.clear(std::memory_order_release);
                            }
                            previous_t = event.t;
                        }
                        if (event.t >= local_playback_t) {
                            if (began) {
                                std::this_thread::sleep_until(
                                    time_reference
                                    + std::chrono::microseconds(
                                        static_cast<uint64_t>((event.t - local_playback_t) / local_playback_speed)));
                            } else {
                                began = true;
                                time_reference = std::chrono::high_resolution_clock::now();
                            }
                            split(event);
                        }
                    },
                    [](std::pair<std::size_t, uint64_t>) {},
                    std::ref(*capture_exception),
                    [&]() {
                        while (accessing_playback.test_and_set(std::memory_order_acquire)) {
                        }
                        playback_t = seeks_and_ts.front().second;
                        accessing_playback.clear(std::memory_order_release);
                        throw std::runtime_error("end of stream");
                        return false;
                    },
                    sepia::dispatch::as_fast_as_possible,
                    seek_and_t->first);
                capture_exception->wait();
                capture_exception->rethrow_unless<>();
            } catch (const std::runtime_error& exception) {
            }
            observable.reset();
            capture_exception.reset();
        }
    });
    auto result = app.exec();
    running.store(false, std::memory_order_release);
    play_loop.join();
    control_loop.join();
    return result;
}
