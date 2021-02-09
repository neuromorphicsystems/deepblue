# Apps

- __viewer__: display the output of a live ATIS camera (requires Qt)
- __recorder__: command-line application to record the output of an ATIS camera in [Event Stream](https://github.com/neuromorphic-paris/event_stream) (.es) format
- __player__: display the events from an [Event Stream](https://github.com/neuromorphic-paris/event_stream) (.es) file (requires Qt)

Other tools:
- https://github.com/neuromorphic-paris/command_line_tools
    - rainmaker: generate 3D plots from .es files
    - cut: extract a time rangee from a stream
    - es_to_csv: export events to CSV
- https://github.com/neuromorphic-paris/loris
    - `pip install loris`
    - read events as numpy arrays

# Raspberry Pi service

Create the file */lib/systemd/system/atis_recorder.service* with the following content:

```sh
[Unit]
Description=Manager for the ATIS recorder program
After=multi-user.target

[Service]
Type=idle
ExecStartPre=mkdir -p /recordings
ExecStartPre=/bin/bash -c 'if [ ! -f /recordings/biases.json ]; then printf "\x7b\x7d\n" > /recordings/biases.json; fi'
ExecStart=/home/pi/atis_recorder/build/release/recorder /recordings /recordings/biases.json
Restart=on-failure
RestartSec=3s
StandardOutput=file:/recordings/atis_recorder_info.log
StandardError=file:/recordings/atis_recorder_error.log

[Install]
WantedBy=multi-user.target
```

To enable the service on boot, run:
```sh
systemctl enable atis_recorder.service
```
