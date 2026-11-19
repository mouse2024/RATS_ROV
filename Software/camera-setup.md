# Camera Stream

The ROV is equipped with an Arducam OV5647.

The video streaming pipeline follows. Note that these processes run on the host (except for blueos-core, which is in Docker).

```
rpicam-vid -> ffmpeg -> mediamtx -> blueos-core (mavlink-camera-manager)
```

Breakdown:
- **rpicam-vid**: Process that directly accesses the camera sensor
- **ffmpeg**: Process that publishes the camera stream from *rpicam-vid* to an RTSP server.
- **mediamtx**: The media server which hosts the RTSP stream.
- **blueos-core**: BlueOS reads the RTSP stream and uses internal mavlink-camera-manager process to make the stream accessible to Cockpit Lite via WebRTC.

## Accessing the Stream

- In a GCS, use `rtsp://127.0.0.1:8554/rpiCamera` in video stream configuration.
- In a web browser, go to `http://192.168.2.2:8889/rpiCamera` (this is MediamTX's HTTP page)
- In BlueOS, Cockpit Lite should automatically capture the stream if the 'Redirect Source' is configured in the BlueOS Video Stream page (enable pirate mode to access).

## Underlying Services

#### Mediamtx Server

The mediamtx server is configured by `/usr/local/etc/mediamtx/mediamtx.yml`. Relevant configurations for the RTSP stream are at the bottom of the file.

A `systemd` service is configured at [`/etc/systemd/system/mediamtx.service`](./configurations/mediamtx.service) to run automatically on startup. It simply runs the mediamtx binary located at `/usr/local/bin/mediamtx`.

Use `systmctl status|start|stop|restart mediamtx.service` to interact with and troubleshoot the service.

#### rpicamera-streamer service

The `rpicam-vid` and `ffmpeg` processes are handled in another `systemd` service named `rpicamera-streamer.service`, configured at [`/etc/systemd/system/rpicamera-streamer.service`](./configurations/rpicamera-streamer.service). It waits for the `mediamtx` service to start, then executes the following command:

```
/bin/bash -o pipefail -c 'rpicam-vid -t 0 --width 1280 --height 720 --framerate 30 --codec h264 --inline -o - | ffmpeg -f h264 -i - -c:v copy -f rtsp rtsp://127.0.0.1:8554/rpiCamera'
```

Use `systmctl status|start|stop|restart rpicamera-streamer.service` to interact with and troubleshoot the service. Note that this service will NOT start until `mediamtx.service` is active. It will try to restart every 5 seconds.

#### BlueOS

To configure the stream in BlueOS, enable Pirate Mode (top right menu), go to the Camera Streams page, and add an RTSP source to 'Redirect Source' using the `rtsp://127.0.0.1:8554/rpiCamera` address.

### Manual Startup

If you want to run the stream without the automatic services, then make sure the services are stopped (`systemctl stop`) and run the commands manually:

```
MEDIAMTX:
/usr/local/bin/mediamtx

RPICAM AND FFMPEG:
rpicam-vid \
  -t 0 \
  --width 1280 \
  --height 720 \
  --framerate 30 \
  --codec h264 \
  --inline \
  -o - | \
ffmpeg \
  -f h264 \
  -i - \
  -c:v copy \
  -f rtsp \
  rtsp://127.0.0.1:8554/rpiCamera
```


## Niche Troubleshooting

If the camera is not detected at all (`rpicam-hello --list` shows nothing), check that `/boot/firmware/config.txt` has a line for `dtoverlay=ov5647` under the `pi4` section.