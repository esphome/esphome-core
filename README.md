# esphomelib [![Build Status](https://travis-ci.org/OttoWinter/esphomelib.svg?branch=master)](https://travis-ci.org/OttoWinter/esphomelib) [![Discord Chat](https://img.shields.io/discord/429907082951524364.svg)](https://discord.gg/KhAMKrd) [![GitHub release](https://img.shields.io/github/release/OttoWinter/esphomelib.svg)](https://GitHub.com/OttoWinter/esphomelib/releases/)


**esphomelib** is a library designed to greatly simplify your firmware code for ESP32/ESP8266-based devices with full 
seamless [Home Assistant](https://www.home-assistant.io) integration (with automatic MQTT discovery!) so that you can focus on creating the hardware, 
not the software.

## See [esphomeyaml](https://github.com/OttoWinter/esphomeyaml) for an easy way to use esphomelib

## Example
For example, the software for a device with an RGB light using the internal PWM and a 
[DHT22](https://www.adafruit.com/product/385) temperature sensor can be as simple as this:

```cpp
#include "esphomelib.h"

using namespace esphomelib;

void setup() {
    App.set_name("livingroom");
    App.init_log();

    App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    App.init_ota()->start_safe_mode();
    App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
    App.init_web_server();

    auto *red = App.make_ledc_output(32); // on pin 32, only available with ESP32
    auto *green = App.make_ledc_output(33);
    auto *blue = App.make_ledc_output(34);
    App.make_rgb_light("Livingroom Light", red, green, blue);
    
    App.make_dht_sensor("Livingroom Temperature", "Livingroom Humidity", 12);

    App.setup();
}

void loop() {
    App.loop();
}
```

And voilà 🎉 - esphomelib will now automatically manage everything such as logging to MQTT, Home Assistant MQTT 
discovery, OTA updates, light effects/transitions, WiFi reconnects, etc etc. for you - amazing, right?

If you have [MQTT Discovery](#home-assistant-configuration) enabled in Home Assistant, all the components defined here
will even show up in the front end without adding anything to your configuration file.

## Powerful Core

It doesn't stop there. If you, for some reason, want to customize the default options for a component, just call the
corresponding methods, almost all options are customizable. And if there's an unsupported device, simply create
a `Component` for it and esphomelib will take care of the MQTT stuff. You will even be able to use all of your 
existing Arduino libraries! If you create a component, please then also consider creating a pull request so that
others won't have to re-implement everything again.

esphomelib is built on a powerful core. Every object in esphomelib that interacts with a device, peripheral, or MQTT,
is a `Component` - and every one of those has its own independent lifecycle with the `setup()` and `loop()` options
you may know from Arduino. All the components are managed by the `Application` instance, which also has many
helper methods to easily create and add components.

Moreover, esphomelib is very good at abstracting different components, so that the front-end parts that report values,
can easily be used with different back-end components/sensors. For example, an esphomelib light doesn't care how
RGB values are written to hardware and you can easily implement your own PWM output while reusing the entire
front-end code.

If esphomelib doesn't have a sensor or device you'd really like, creating a custom component is very
easy: [Custom Sensor Component](https://github.com/OttoWinter/esphomelib/wiki/Custom-Sensor-Component).

## API documentation

You can find a documentation for the API of this project at [esphomelib.com](http://esphomelib.com/api/).

## Getting Started

### Setting Up Development Environment

#### PlatformIO

esphomelib is made for use with [platformio](http://platformio.org/), an advanced ecosystem for microcontroller
development. To get started with coding esphomelib applications, you first have to 
[install the atom-based platformio IDE](http://platformio.org/platformio-ide) or for advanced users, 
[install the command line version of platformio](http://docs.platformio.org/en/latest/installation.html).

Then create a new project for an [ESP32-based board](http://docs.platformio.org/en/latest/platforms/espressif32.html#boards)
(for example, `nodemcu-32s`). Then open up the newly created `platformio.ini` file and insert

```ini
; ...
platform = espressif32
board = nodemcu-32s
framework = arduino
lib_deps = esphomelib
```

... or for [ESP8266-based boards](http://docs.platformio.org/en/latest/platforms/espressif8266.html#boards):

```ini
; ...
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps = esphomelib
```

Finally, create a new source file in the `src/` folder (for example `main.cpp`) and start coding with esphomelib.

#### Arduino IDE


##### Installing the esphomelib library

1. Download the latest release from https://github.com/OttoWinter/esphomelib/releases
2. (In the Arduino IDE) **Sketch > Include Library > Add .ZIP Library... >** select the downloaded file **> Open**

##### Installing library dependencies

Repeat the above steps with the following libraries:
- https://github.com/marvinroger/async-mqtt-client/archive/master.zip
- https://github.com/OttoWinter/ArduinoJson/releases
- https://github.com/FastLED/FastLED/releases
- https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
- https://github.com/me-no-dev/AsyncTCP/archive/master.zip (only needed for ESP32)
- https://github.com/me-no-dev/ESPAsyncTCP/archive/master.zip (only needed for ESP8266)

After installing esphomelib, you will find a variety of example sketches under **File > Examples > esphomelib**.

### Bare Bones

Before adding all the desired components to your code, there are a few things you need to set up first.

Begin with including the library and setting the C++ namespace.

```cpp
#include "esphomelib.h"

using namespace esphomelib;
```

Next, set the name of your node (here "livingroom") with `App.set_name()`. This is required if you plan on using MQTT. 
Then initialize the log so you can debug your code on the serial port with baud rate 115200.
Additionally, important log messages will also be published on MQTT with the topic `<NAME>/debug`
(here `livingroom/debug`) by default.

Following after that you'll want to setup all the connectivity stuff, like WiFi, MQTT, and OTA with the respective 
methods in your `Application` instance.

Note: MQTT will automatically determine the topics it publishes/subscribes to. All state/command topics will, 
by default, be set to `livingroom/...` (e.g. `lightroom/light/livingroom_light/state`), birth messages/last will 
testaments are sent to `livingroom/state`, and discovery will automatically happen with the base topic `homeassistant/`.

```cpp
void setup() {
    App.set_name("livingroom");
    App.init_log();

    App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    App.init_ota()->start_safe_mode();
    App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
    // ...

```

Last, all you need to do is add your components. Then call `App.setup()` **at the end of** `setup()` - do the same 
with `loop()`

```cpp
    App.setup();
}

void loop() {
    App.loop();
}
```

### Adding Components

In order to create a clear separation between front-end (like MQTT) and back-end (peripherals), every time you add 
a device, you'll actually want to add two components: a base component (such as `SensorComponent`) and the 
corresponding `MQTTComponent`. Note: many methods in `Application` do this automatically, so you won't have to worry 
about this unless you use custom components.

In the following section, we'll go through how you would go about adding/initializing the main built-in components 
of esphomelib.

#### Light

To add lights to your device, you'll only really need to do two things: Create the channels for output and setup 
the light component.

First, create a `OutputComponent`. This is basically just a component that the light will write brightness values to. 
Each channel, i.e. the RGB channels, will have its own output and you can easily switch between different types of 
outputs. For example, to use the internal LEDC PWM peripheral, just use the following code to create an output 
component on pin 32:

```cpp
auto *red = App.make_ledc_output(32);
```

Additionally, you can use `make_atx()` to automatically switch on a power supply when a channel is switched high.

Next, use the `make_binary_light()`, `make_monochromatic_light()`, and `make_rgb_light()` methods with a friendly name 
(which will be displayed in Home Assistant) and the channels, to create the light component.

```cpp
App.make_rgb_light("Livingroom Light", red, green, blue);
App.make_monochromatic_light("Table Lamp", table_lamp);
App.make_binary_light("Livingroom Standing Lamp", standing_lamp);
```

> Lights will automatically store their state in non-volatile memory so that lights can restore the color and 
> brightness if the board is restarted 🎉

#### Sensor

Adding a sensor is quite easy in esphomelib. For example, to add a DHT22, just use the `make_dht_sensor()` with 
the GPIO pin and the friendly names for the temperature and humidity.

Dallas ds18b20 sensors are almost equally easy. Just setup the sensor hub on a pin with `make_dallas_component()` and 
then use that to get sensors with `get_sensor_by_address()` or `get_sensor_by_index()` and register them with `make_mqtt_sensor_for()`.

Every sensor (or technically every `MQTTSensorComponent`) can have a smoothing filter and a value offset. By default, 
all sensors created with the methods in the `Application` instance automatically take the average of the last few 
values and report that to the outside world - to disable this, use the `disable_filter()` method.

#### Switch

To create a simple GPIO switch that can control a high/low device on a pin, just use the following code with your 
pin and friendly name.

```cpp
App.make_simple_gpio_switch(32, "Livingroom Dehumidifier");
```

Another feature esphomelib offers is infrared transmitter support. This way, you can control all devices using remote 
controls yourself. First, you'll need to find the IR codes of the remote - maybe try the enormous 
[LIRC database](http://lirc.sourceforge.net/remotes/) if you quickly want to find the codes for your remote. Then adapt
the following code to your needs (and repeat from the second line for each channel):

```cpp
auto *ir = App.make_ir_transmitter(32); // switch out 32 for your IR pin
auto *channel = ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)); // use the other functions in SendData for other codes.
App.make_mqtt_switch_for("Panasonic TV On", channel);
```

That's it.

#### Binary Sensor

To create a simple GPIO binary sensor, that reports the state of a GPIO pin, use `make_gpio_binary_sensor()` 
with the friendly name of the binary sensor, a [device class](https://www.home-assistant.io/components/binary_sensor/), 
and the GPIO pin like this:

```cpp
App.make_gpio_binary_sensor(36, "Cabinet Motion", binary_sensor::device_class::MOTION);
```

#### Fan

Fans can be created by first calling `App.make_fan("Friendly Name")` and then using the return value
to set the output channels. See [`examples/fan-example.cpp`](examples/fan-example.cpp) for an example.

## Home Assistant Configuration

To use an **esphomelib** component with Home Assistant, [MQTT discovery](https://www.home-assistant.io/docs/mqtt/discovery/)
must be enabled with the topic `homeassistant/` (the default).

```yaml
mqtt:
  broker: 192.168.0.2  # Change this to your broker
  discovery: True
```

## Current Features

* Powerful core that allows for easy creation of new, custom components
* Automatic WiFi handling (reconnects, etc.)
* Automatic MQTT handling (birth messages, last will testaments, reconnects, etc.)
* Powerful, (colored - yay 🌈) logging to Serial **and** MQTT.
* Over the Air (OTA) updates
   * OTA "safe mode". esphomelib will automatically detect boot loops and automatically
     switch to a safe mode where only OTA is enabled if needed.
* Home Assistant automatic MQTT discovery
* Binary Sensors
* Switches
* Fans
* Dallas DS18b20 temperature sensors
* DHT11/DHT22 temperature/humidity sensors (with "DHT" library)
* DHT12 over the I²C bus
* Lights
    - Binary/Brightness-only/RGB/RGBW
    - Transitions/Flashes
    - Effects (easy to add custom ones)
* IR Transmitters (with ESP32 remote control peripheral)
* ESP32: LEDC peripheral PWM output
* ESP8266 Software PWM
* PCA9685 PWM output (with "PCA9685" library)
* Power supply Mode - Automatically switch a power supply on/off when it's needed.
* ADS1115 I²C ADC sensor (experimental)
* BMP085 temperature and pressure I²C sensor
* BMP180/BME280 temperature, pressure and humidity I²C sensor
* BME680 temperature, pressure, humidity and gas I²C sensor
* HTU21D, HDC1080 and HDC1080 temperature and humidity I²C sensor
* BH1750 brightness I²C sensor 

## Planned features

* Improve documentation
* Refine default options (like sensor update intervals)
* FastLED support
* Covers
* Status LED
* More light effects
* Multiple WiFi Networks
* **Suggestions?** Feel free to create an issue and tag it with feature request.

## Advanced Options

### Disable MQTT Logging

The second argument to `init_log()` denotes the MQTT topic that logs will be written to, providing a disabled Optional
disables MQTT Logging.

```cpp
App.init_log(115200, optional<std::string>());
```

### Logging

esphomelib features a powerful logging engine that automatically pushes logs to Serial and MQTT.
To use this in your own code, simply define a TAG, and use `ESP_LOGx` 
(from [esp-idf](https://esp-idf.readthedocs.io/en/v2.1.1/api-reference/system/log.html), this has been back-ported 
to ESP8266 too):

```cpp
static const char *TAG = "main";

// in your code:
ESP_LOGV(TAG, "This is a verbose message.");
ESP_LOGD(TAG, "This is a debug message.");
ESP_LOGI(TAG, "This is an informational message.");
ESP_LOGW(TAG, "This is a warning message.");
``` 

There are several log levels available:
 * **Verbose** (This is mostly for esphomelib development)
 * **Debug (Default)** (Includes useful information when setting up your project)
 * **Info** (Only important messages are published here; nothing periodic)
 * **Warning** (Warnings about invalid sensor values, ...)
 * **Error** (Only error messages that stop esphomelib from working correctly)

To change the global log level, include the following in your `platform.ini` (and change verbose to the log level 
you want):

```ini
build_flags =
    -DESPHOMELIB_LOG_LEVEL=ESPHOMELIB_LOG_LEVEL_VERBOSE
```

Next, if you're using MQTT logging, simply subscribe to the debug topic and see all the beautifully color-coded log
messages scroll by:

```bash
# for example if using mosquitto and name is livingroom
mosquitto_sub -h 192.168.178.42 -u USERNAME -P PASSWORD -t livingroom/debug
```

> Note: use `set_global_log_level()` and `set_log_level` in `LogComponent` to adjust the global and 
> tag-specific log levels, respectively, for more fine-grained control. But also make sure to update the `build_flags`
> to a low enough log level because `build_flags` defines which log messages are even included in your App.

When using the `platformio device monitor [...]` command, try adding the `--raw` argument - this will apply color to
log messages in your terminal.

### Setting custom MQTT topics

If esphomelib's default MQTT topics don't suit your needs, you can override them. For this, there are two options: 

#### 1. Set the global MQTT topic prefix.

By default, all MQTT topics are named with the application name (see `set_name()`), for example `livingroom/...`.
However, if you want to use your own MQTT topic prefixes like `home/livingroom/node1/...`, this is possible:

```cpp
auto *mqtt = App.init_mqtt(...);
// topic prefix should *not* include trailing "/"
mqtt->set_topic_prefix("home/livingroom/node1");
```

#### 2. Customize the MQTT topics of a MQTTComponent
 
Customizing the MQTT state/command topics of a single `MQTTComponent` is also possible. Simply call 
`set_custom_*_topic()` on your `MQTTComponent` like this:

```cpp
auto dht = App.make_dht_sensor(12, "Livingroom Temperature", "Livingroom Humidity");
dht.mqtt_temperature->set_custom_state_topic("home/livingroom/node1/temperature/state");
```

### OTA Updates

If you call `App.init_ota()` during `setup()`, esphomelib will continuously listen for over-the-air updates. 
To push an OTA update to your device, simply add `upload_port = `*`IP_OF_YOUR_ESP`* to your `platform.ini`.
Then do the upload as you would do normally via serial. You might want to [set a static IP](#static-ips) for your ESP32.

> Note: OTA is, by default, enabled without any authentication. If you're on a public WiFi network, it's highly
> encouraged to set a passphrase using the `set_auth_*()` methods on the object returned by `init_ota()`. Then also
> include `upload_flags = -a `*`PASSPHRASE`* in your `platformio.ini`.
