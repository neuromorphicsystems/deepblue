R""(
{
    "ini300": {
        "name": "INI-300",
        "size": [346, 260],
        "streams": ["dvs", "aps", "imu", "trigger"],
        "slots": [
            {
                "key": "reset_timestamp",
                "name": "Reset timestamp",
                "parameters": []
            }
        ],
        "parameters": [
            {
                "key": "biases",
                "name": "Biases",
                "type": "collection",
                "children": [
                    {
                        "key": "off_threshold",
                        "name": "OFF threshold",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1025
                    },
                    {
                        "key": "differential",
                        "name": "Differential",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1063
                    },
                    {
                        "key": "on_threshold",
                        "name": "ON threshold",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1535
                    },
                    {
                        "key": "refractory_period",
                        "name": "Refractory period",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1049
                    },
                    {
                        "key": "local_buffer",
                        "name": "Local buffer",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1444
                    },
                    {
                        "key": "pad_follower",
                        "name": "Pad follower",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 2007
                    },
                    {
                        "key": "passive_load_inverter",
                        "name": "Passive load inverter",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1680
                    },
                    {
                        "key": "photoreceptor",
                        "name": "Photoreceptor",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 570
                    },
                    {
                        "key": "follower",
                        "name": "Follower",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 272
                    },
                    {
                        "key": "readout_buffer",
                        "name": "Readout buffer",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1556
                    },
                    {
                        "key": "aps_follower",
                        "name": "APS follower",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1755
                    },
                    {
                        "key": "adc_comparator",
                        "name": "ADC comparator",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1300
                    },
                    {
                        "key": "column_selection_low",
                        "name": "Column selection low",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1
                    },
                    {
                        "key": "dac_buffer",
                        "name": "DAC buffer",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1596
                    },
                    {
                        "key": "column_timeout",
                        "name": "Column timeout",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1329
                    },
                    {
                        "key": "arbiter_pulldown",
                        "name": "Arbiter pulldown",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1627
                    },
                    {
                        "key": "arbiter_pullup_x",
                        "name": "Arbiter pullup X",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1104
                    },
                    {
                        "key": "arbiter_pullup_y",
                        "name": "Arbiter pullup Y",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1944
                    },
                    {
                        "key": "if_refractory_period",
                        "name": "IF refractory period",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1535
                    },
                    {
                        "key": "if_threshold",
                        "name": "IF threshold",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1535
                    },
                    {
                        "key": "bias_buffer",
                        "name": "Bias buffer",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 2047,
                        "default": 1534
                    },
                    {
                        "key": "aps_overflow_level_voltage",
                        "name": "APS overflow level voltage",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 63,
                        "default": 27
                    },
                    {
                        "key": "aps_overflow_level_current",
                        "name": "APS overflow level current",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 7,
                        "default": 6
                    },
                    {
                        "key": "aps_cascode_voltage",
                        "name": "APS cascode voltage",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 63,
                        "default": 21
                    },
                    {
                        "key": "aps_cascode_current",
                        "name": "APS cascode current",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 7,
                        "default": 6
                    },
                    {
                        "key": "adc_reference_high_voltage",
                        "name": "ADC reference high voltage",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 63,
                        "default": 32
                    },
                    {
                        "key": "adc_reference_high_current",
                        "name": "ADC reference high current",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 7,
                        "default": 7
                    },
                    {
                        "key": "adc_reference_low_voltage",
                        "name": "ADC reference low voltage",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 63,
                        "default": 1
                    },
                    {
                        "key": "adc_reference_low_current",
                        "name": "ADC reference low current",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 7,
                        "default": 7
                    },
                    {
                        "key": "adc_test_voltage_voltage",
                        "name": "ADC test voltage voltage",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 63,
                        "default": 21
                    },
                    {
                        "key": "adc_test_voltage_current",
                        "name": "ADC test voltage current",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 7,
                        "default": 7
                    }
                ]
            },
            {
                "key": "dvs",
                "name": "DVS",
                "type": "collection",
                "children": [
                    {
                        "key": "enable",
                        "name": "Enable",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "drop_on_stall",
                        "name": "Drop on stall",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "roi",
                        "name": "ROI",
                        "type": "rectangle",
                        "minimum": [0, 0],
                        "maximum": [345, 259],
                        "default": [[0, 0], [345, 259]]
                    },
                    {
                        "key": "set_all_polarities_to_off",
                        "name": "Set all polarities to OFF",
                        "type": "bool",
                        "default": false
                    },
                    {
                        "key": "enable_external_aer",
                        "name": "Enable external AER",
                        "type": "bool",
                        "default": false
                    },
                    {
                        "key": "filter_polarity",
                        "name": "Filter polarity",
                        "type": "enum",
                        "values": ["None", "OFF", "ON"],
                        "default": "None"
                    },
                    {
                        "key": "filter_background_activity",
                        "name": "Filter background activity",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 4095,
                        "default": 0,
                        "unit": [0.25, "ms"]
                    },
                    {
                        "key": "filter_refractory_period",
                        "name": "Filter refractory period",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 4095,
                        "default": 0,
                        "unit": [0.25, "ms"]
                    },
                    {
                        "key": "filter_nth_event",
                        "name": "Filter nth event",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 256,
                        "default": 256
                    },
                    {
                        "key": "filtered_pixel_0",
                        "name": "Filtered pixel 0",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_1",
                        "name": "Filtered pixel 1",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_2",
                        "name": "Filtered pixel 2",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_3",
                        "name": "Filtered pixel 3",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_4",
                        "name": "Filtered pixel 4",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_5",
                        "name": "Filtered pixel 5",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_6",
                        "name": "Filtered pixel 6",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    },
                    {
                        "key": "filtered_pixel_7",
                        "name": "Filtered pixel 7",
                        "type": "pixel",
                        "minimum": [0, 0],
                        "maximum": [346, 260],
                        "default": [346, 260]
                    }
                ]
            },
            {
                "key": "aps",
                "name": "APS",
                "type": "collection",
                "children": [
                    {
                        "key": "enable",
                        "name": "Enable",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "drop_on_stall",
                        "name": "Drop on stall",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "roi",
                        "name": "ROI",
                        "type": "rectangle",
                        "minimum": [0, 0],
                        "maximum": [345, 259],
                        "default": [[0, 0], [345, 259]]
                    },
                    {
                        "key": "exposure",
                        "name": "Exposure",
                        "type": "int",
                        "minimum": 1,
                        "maximum": 4294967295,
                        "default": 624000,
                        "unit": [0.000016025641025641026, "ms"]
                    },
                    {
                        "key": "frame_duration",
                        "name": "Frame duration",
                        "type": "int",
                        "minimum": 1,
                        "maximum": 4294967295,
                        "default": 6240000,
                        "unit": [0.000016025641025641026, "ms"]
                    }
                ]
            },
            {
                "key": "imu",
                "name": "IMU",
                "type": "collection",
                "children": [
                    {
                        "key": "enable_gyroscope",
                        "name": "Enable gyroscope",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "enable_thermometer",
                        "name": "Enable thermometer",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "rate_divider",
                        "name": "Rate divider",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "accelerometer_low_pass_filter",
                        "name": "Accelerometer low pass filter",
                        "type": "enum",
                        "values": [
                            "𝑓₀ = 250 Hz, G = 0.97 ms, 𝑓 = 8 kHz",
                            "𝑓₀ = 184 Hz, G = 2.9 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 92 Hz, G = 3.9 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 41 Hz, G = 5.9 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 20 Hz, G = 9.9 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 10 Hz, G = 17.85 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 5 Hz, G = 33.48 ms, 𝑓 = 1 kHz",
                            "𝑓₀ = 3600 Hz, G = 0.17 ms, 𝑓 = 8 kHz"
                        ],
                        "default": "𝑓₀ = 184 Hz, G = 2.9 ms, 𝑓 = 1 kHz"
                    },
                    {
                        "key": "gyroscope_range",
                        "name": "Gyroscope range",
                        "type": "enum",
                        "values": [
                            "±250 °/s",
                            "±500 °/s",
                            "±1000 °/s",
                            "±2000 °/s"
                        ],
                        "default": "±500 °/s"
                    }
                ]
            },
            {
                "key": "trigger",
                "name": "Trigger",
                "type": "collection",
                "children": [
                    {
                        "key": "enable",
                        "name": "Enable",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "drop_on_stall",
                        "name": "Drop on stall",
                        "type": "bool",
                        "default": true
                    },
                    {
                        "key": "pulse_length",
                        "name": "Pulse length",
                        "type": "int",
                        "minimum": 1,
                        "maximum": 4294967295,
                        "default": 1040,
                        "unit": [0.000009615384615384616, "ms"]
                    },
                    {
                        "key": "pulse_polarity",
                        "name": "Pulse polarity",
                        "type": "enum",
                        "values": ["None", "OFF", "ON"],
                        "default": "ON"
                    },
                    {
                        "key": "detect_rising_edges",
                        "name": "Detect rising edges",
                        "type": "bool",
                        "default": false
                    },
                    {
                        "key": "detect_falling_edges",
                        "name": "Detect falling edges",
                        "type": "bool",
                        "default": false
                    }
                ]
            },
            {
                "key": "usb",
                "name": "USB",
                "type": "collection",
                "children": [
                    {
                        "key": "early_packet_delay",
                        "name": "Early packet delay",
                        "type": "int",
                        "minimum": 1,
                        "maximum": 4294967295,
                        "default": 83200,
                        "unit": [0.00001201923076923077, "ms"]
                    }
                ]
            }
        ]
    },
    "psee300": {
        "name": "PSEE-300",
        "size": [640, 480],
        "streams": ["dvs"],
        "parameters": [
            {
                "key": "biases",
                "name": "Biases",
                "type": "collection",
                "children": [
                    {
                        "key": "diff_off",
                        "name": "Differential OFF",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 30
                    },
                    {
                        "key": "diff_on",
                        "name": "Differential ON",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 51
                    },
                    {
                        "key": "diff",
                        "name": "Differential",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 41
                    },
                    {
                        "key": "latchout_or_pu",
                        "name": "Latch out",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 13
                    },
                    {
                        "key": "reqx_or_pu",
                        "name": "Request x",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 13
                    },
                    {
                        "key": "req_pux",
                        "name": "Request PUx",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 169
                    },
                    {
                        "key": "req_puy",
                        "name": "Request PUy",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 169
                    },
                    {
                        "key": "del_reqx_or",
                        "name": "Delta request x",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 140
                    },
                    {
                        "key": "sendreq_pdx",
                        "name": "Send request PDx",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 112
                    },
                    {
                        "key": "sendreq_pdy",
                        "name": "Send request PDy",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 140
                    },
                    {
                        "key": "del_ack_array",
                        "name": "Delta acknowledge",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 140
                    },
                    {
                        "key": "del_timeout",
                        "name": "Delta timeout",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 62
                    },
                    {
                        "key": "inv",
                        "name": "Inverter",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 69
                    },
                    {
                        "key": "refr",
                        "name": "Refractory",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 211
                    },
                    {
                        "key": "clk",
                        "name": "Clock",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 83
                    },
                    {
                        "key": "overflow",
                        "name": "Overflow",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "tail",
                        "name": "Tail",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "out",
                        "name": "Out",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "hyst",
                        "name": "Hysteresis",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "vrefl",
                        "name": "Vref Low",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "vrefh",
                        "name": "Vref High",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "cas",
                        "name": "Cascode",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 140
                    },
                    {
                        "key": "fo",
                        "name": "Follower",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 243
                    },
                    {
                        "key": "pr",
                        "name": "PR",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 211
                    },
                    {
                        "key": "bulk",
                        "name": "Bulk",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 211
                    },
                    {
                        "key": "hpf",
                        "name": "HPF",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 211
                    },
                    {
                        "key": "buf",
                        "name": "Buffer",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 83
                    }

                ]
            }
        ]
    },
    "psee400": {
        "name": "PSEE-400",
        "size": [1280, 720],
        "streams": ["dvs", "trigger"],
        "parameters": [
            {
                "key": "biases",
                "name": "Biases",
                "type": "collection",
                "children": [
                    {
                        "key": "diff_on",
                        "name": "Differential ON",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 117
                    },
                    {
                        "key": "diff",
                        "name": "Differential",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 84
                    },
                    {
                        "key": "diff_off",
                        "name": "Differential OFF",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 67
                    },
                    {
                        "key": "refr",
                        "name": "Refractory",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 61
                    },
                    {
                        "key": "vrefh",
                        "name": "Vref High",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "vrefl",
                        "name": "Vref Low",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "pr",
                        "name": "PR",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 151
                    },
                    {
                        "key": "fo_p",
                        "name": "Follower p",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "fo_n",
                        "name": "Follower n",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 16
                    },
                    {
                        "key": "hpf",
                        "name": "HPF",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 48
                    },
                    {
                        "key": "inv",
                        "name": "Inverter",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 76
                    },
                    {
                        "key": "cas",
                        "name": "Cascode",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "overflow",
                        "name": "Overflow",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "tail",
                        "name": "Tail",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "hyst",
                        "name": "Hysteresis",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "out",
                        "name": "Output",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 0
                    },
                    {
                        "key": "req_puy",
                        "name": "Request PUy",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 124
                    },
                    {
                        "key": "reqpux",
                        "name": "Request PUx",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 124
                    },
                    {
                        "key": "sendreqpdy",
                        "name": "Send request PDy",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 148
                    },
                    {
                        "key": "blk",
                        "name": "Bulk",
                        "type": "int",
                        "minimum": 0,
                        "maximum": 255,
                        "default": 120
                    }
                ]
            }
        ]
    }
}
)""
