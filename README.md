# USB Serial Logging

## Usage
Place the `20_usb_serial_logging.rpx` in the `[ENVIRONMENT]/modules/setup` folder and run the [EnvironmentLoader](https://github.com/wiiu-env/EnvironmentLoader).
Requires [MochaPayload](https://github.com/wiiu-env/MochaPayload) as a setup module in `[ENVIRONMENT]/modules/setup`.

Requires a USB serial cable with FTDI FT232 chipset (e.g. Digitus DA-70156) and [TeraTerm](https://ttssh2.osdn.jp/index.html.en), or similar serial terminal program.

Settings:
 - Speed: 57600
 - Data: 8 bit
 - Parity: none
 - Stop bits: 1 bit
 - Flow control: none
 - New-line Receive: CR+LF
 - New-line Transmit: CR+LF
    
The IOP-Shell is not supported.

## Building

For building you just need [wut](https://github.com/devkitPro/wut/) installed, then use the `make` command.

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t usb_serial_logging-builder

# make 
docker run -it --rm -v ${PWD}:/project usb_serial_logging-builder make

# make clean
docker run -it --rm -v ${PWD}:/project usb_serial_logging-builder make clean
```

## Credits
Maschell