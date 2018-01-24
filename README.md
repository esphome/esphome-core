# esphomelib [![Build Status](https://travis-ci.org/OttoWinter/esphomelib.svg?branch=master)](https://travis-ci.org/OttoWinter/esphomelib)

**esphomelib** is a library designed to greatly simplify your firmware code for ESP32-based devices with full MQTT
support so that you can focus on creating the hardware, not the software. It seamlessly integrates with Home Assistant
by providing many of its components out-of-the-box with reasonable defaults set automatically.

## Example
For example, the software for a device with an RGB light using the internal PWM and a 
[DHT22](https://www.adafruit.com/product/385) temperature sensor can be as simple as this:

```cpp
#include "esphomelib/application.h"

using namespace esphomelib;

Application app;

void setup() {
    app.set_name("livingroom");
    app.init_log();

    app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
    app.init_ota();

    auto *red = app.make_ledc_component(32); // on pin 32
    auto *green = app.make_ledc_component(33);
    auto *blue = app.make_ledc_component(34);
    app.make_rgb_light("Livingroom Light", red, green, blue);
    
    app.make_dht_component(12, "Livingroom Temperature", "Livingroom Humidity");

    app.setup();
}

void loop() {
    app.loop();
}
```

And voilà - esphomelib will now automatically manage everything such as logging to MQTT, Home Assistant MQTT-discovery, 
OTA updates, light effects/transitions, WiFi reconnects, etc. for you - amazing, right?

## Powerful Core
It doesn't stop there. If you, for some reason, want to customize the default options for a component, just call the
corresponding methods, almost all options are customizable. And if there's an unsupported device, simply create
a `Component` for it and esphomelib will take care of the MQTT stuff. You will even be able to use all of your 
existing Arduino libraries! If you create a component, please then also consider creating a pull request so that
others won't have to reimplement everything again.

esphomelib is built on a powerful core. Every object in esphomelib that interacts with a device, peripheral, or MQTT,
is a `Component` - and every one of those has its own independent lifecycle with the `setup()` and `loop()` options
you may know from Arduino. All the components are managed by the `Application` instance, which also has many
helper methods to easily create and add components.

Moreover, esphomelib is very good at abstracting different components, so that the front-end parts that report values,
can easily be used with different back-end components/sensors. For example, an esphomelib light doesn't care how
RGB values are written to hardware and you can easily implement your own PWM output while reusing the entire
front-end code.

## Why not ESPEasy?

[ESPEasy](https://www.letscontrolit.com/wiki/index.php/ESPEasy), an excellent library with a similar objective, has a 
big community supporting development and lots of supported hardware. esphomelib, however, does have some advantages. 
Firstly, esphomelib is made for the ESP32, which is the successor of the ESP8266 chip that ESPEasy is made for.
The ESP32 has many addtional features/peripherals. Just to name a few:

* It's faster and has more flash space (in most configurations)
* The ESP32 has an integrated high-precision high-speed PWM peripheral with up to 16 channels - this is very handy for 
RGB lights!
* Many features that would otherwise be required to be implemented in software, have hardware peripherals on the ESP32, 
which can be very useful - for example integrated IR transmitting, pulse counting, etc.

Moreover this library is especially useful for Home Assistant users because of its tight integration with discovery
and pre-implemented components. And if there's a new piece of hardware you want to support, that's going to be much
easier with esphomelib and its powerful core.

## Getting Started

### Setting Up Development Environment

>  Note: Developing in the Arduino IDE is currently not possible, but support for this is being looked into. However, 
> platformio will remain the preferred method.

esphomelib is made for use with [platformio](http://platformio.org/), an advanced ecosystem for microcontroller
development. To get started with coding esphomelib applications, you first have to 
[install the atom-based platformio IDE](http://platformio.org/platformio-ide) or for advanced users, 
[install the command line version of platformio](http://docs.platformio.org/en/latest/installation.html).

Then create a new project for an [ESP32-based board](http://docs.platformio.org/en/latest/platforms/espressif32.html#boards)
(for example, `nodemcu-32s`). Then open up the newly created `platformio.ini` file and insert

```ini
; ...
framework = arduino
lib_deps = esphomelib
```

Finally, create a new source file in the `src/` folder (for example `main.cpp`) and start coding with esphomelib.

### Bare Bones

Before adding all the desired components to your code, there are a few things you need to set up first.

Begin with including the library and setting up the `Application` instance, which will handle all of your components.
```cpp
#include "esphomelib/application.h"

using namespace esphomelib;

Application app;
```

Next, set the name of your node (here "livingroom") with `app.set_name()`. This is required if you plan on using MQTT. 
Then initialize the log so you can debug your code on the serial port of the ESP32 with baud rate 115200.
Additionally, important log messages will also be published on MQTT with the topic `NAME/debug`
(here `livingroom/debug`) by default.

Following after that you'll want to setup all the connectivity stuff, like WiFi, MQTT, and OTA with the respective 
methods in your `Application` instance.

Note: MQTT will automatically determine the topics it publishes/subscribes to. All state/command topics will, 
by default, be set to `livingroom/...` (e.g. `lightroom/light/livingroom_light/state`), birth messages/last will 
testaments are sent to `livingroom/state`, and discovery will automatically happen with the base topic `discovery/`.

```cpp
void setup() {
    app.set_name("livingroom");
    app.init_log();

    app.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    app.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
    app.init_ota();
    // ...

```

Last, all you need to do is add your components. Then call `app.setup()` **at the end of** `setup()` - do the same 
with `loop()`

```cpp
    app.setup();
}

void loop() {
    app.loop();
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
Each channel, i.e. the RGB channels, will have its own output and you can easily switch between different types of outputs. For example, to use the internal LEDC PWM peripheral, just use the following code to create an output component on pin 32:

```cpp
auto *red = app.make_ledc_component(32);
```

Aditionnaly, you can use `make_atx()` to automatically switch on a power supply when a channel is switched high.

Next, use the `make_binary_light()`, `make_monochromatic_light()`, and `make_rgb_light()` methods with a friendly name 
(which will be displayed in Home Assistant) and the channels, to create the light component.

```cpp
app.make_rgb_light("Livingroom Light", red, green, blue);
app.make_monochromatic_light("Table Lamp", table_lamp);
app.make_binary_light("Livingroom Standing Lamp", standing_lamp);
```

> Lights will automatically store their state in non-volatile memory so that lights can restore the color and brightness if the ESP32 were restarted.

#### Sensor

Adding a sensor is quite easy in esphomelib. For example, to add a DHT22, just use the `make_dht_component()` with 
the GPIO pin and the friendly names for the temperature and humidity.

Dallas ds18b20 sensors are almost equally easy. Just setup the sensor hub on a pin with `make_dallas_component()` and 
then use that to get sensors with `get_sensor_by_address()` or `get_sensor_by_index()` and register them with `make_mqtt_sensor_for()`.

Every sensor (or technically every `MQTTSensorComponent`) can have a smoothing filter and a value offset. By default, 
all sensors create with the methods in the `Application` instance, automatically take the average of the last few 
values and report that to the outside world - to disable this, use the `disable_filter()` method.

#### Switch

To create a simple GPIO switch that can control a high/low device on a pin, just use the following code with your 
pin and friendly name.

```cpp
app.make_simple_gpio_switch(32, "Livingroom Dehumidifier");
```

Another feature esphomelib offers is infrared transmitter support. This way, you can control all devices using remote 
controls yourself. First, you'll need to find the IR codes of the remote - maybe try the enormous 
[LIRC database](http://lirc.sourceforge.net/remotes/) if you quickly want to find the codes for your remote. Then adapt
the following code to your needs (and repeat from the second line for each channel):

```cpp
auto *ir = app.make_ir_transmitter(32); // switch out 32 for your IR pin
auto *channel = ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25)); // use the other functions in SendData for other codes.
app.make_mqtt_switch_for("Panasonic TV On", channel);
```

That's it.

#### Binary Sensor

To create a simple GPIO binary sensor, that reports the state of a GPIO pin, use `make_simple_gpio_binary_sensor()` 
with the friendly name of the binary sensor, a [device class](https://home-assistant.io/components/binary_sensor/), 
and the GPIO pin like this:

```cpp
app.make_simple_gpio_binary_sensor("Cabinet Motion", binary_sensor::device_class::MOTION, 36);
```

#### Fan

Fans can be created by first calling `app.make_fan("Friendly Name")` and then using the return value
to set the output channels. See [`examples/fan-example.cpp`](examples/fan-example.cpp) for an example.

## Current Features

* Powerful core that allows for easy creation of new, custom components
* Automatic WiFi handling (reconnects, etc.)
* Automatic MQTT handling (birth messages, last will testaments, reconnects, etc.)
* Powerful, (colored - yay) logging to Serial **and** MQTT
* Over the Air updates
* Home Assistant automatic MQTT discovery
* Binary Sensors
* Switches
* Fans
    - ON/OFF
    - Speed
    - Oscillation
* Dallas ds18b20 temperature sensors (with "DallasTemperature" library)
* DHT11/DHT22 temperature/humidity sensors (with "DHT" library)
* Lights
    - Binary/Brightness-only/RGB/RGBW
    - Transitions/Flashes
    - Effects (easy to add custom ones)
* IR Transmitters (with remote control peripheral)
* LEDC peripheral PWM output
* PCA9685 PWM output (with "PCA9685" library)
* ATX Mode - Automatically switch a power supply on/off when it's needed.

## Planned features

* Arduino IDE support (if possible)
* ESP8266 support
* Multiple WiFi Networks
* internal ADC
* IR Receiver via remote control peripheral
* Improve documentation
* Testing
* FastLED support
* More light effects
* Home Assistant covers
* Refine default options
* Pulse Counter
* Status LED
* Covers
* **Suggestions?** Feel free to create an issue and tag it with feature-request.

## Advanced Options

### Where to Find Code for Initializing Components?

A good place to start is always to go look in the `Application` class. There, all possible use-cases should have a 
corresponding helper method. In the future, guides for each component+component type will be set up, so that getting 
started is a lot easier for beginners. Next, if you want to override the default options for a component, you should 
look in the component, most options should already have appropriate setters for customization.

### Static IPs

After `init_wifi()`, call:

```cpp
wifi->set_manual_ip(ManualIP{
    .static_ip = IPAddress(192, 168, 178, 204),
    .gateway = IPAddress(192, 168, 178, 1),
    .subnet = IPAddress(255, 255, 255, 0)
});
```

### Disable MQTT Logging

The second argument to `init_log()` denotes the MQTT topic that logs will be written to, providing a disabled Optional
disables MQTT Logging.

```cpp
app.init_log(115200, Optional());
```

### Logging

esphomelib features a powerful logging engine that automatically pushes logs to Serial and MQTT.
To use this in your own code, simply include `esp_log.h`, define a TAG, and use `ESP_LOGx` 
(from [esp-idf](https://esp-idf.readthedocs.io/en/v2.1.1/api-reference/system/log.html)):

```cpp
#include <esp_log.h>

static const char *TAG = "main";

// in your code:
ESP_LOGV(TAG, "This is a verbose message.");
ESP_LOGD(TAG, "This is a debug message.");
ESP_LOGI(TAG, "This is an informational message.");
ESP_LOGW(TAG, "This is a warning message.");
``` 

> Note: use `set_global_log_level()` and `set_log_level` in `LogComponent` to adjust the global and 
tag-specific log levels, respectively.

When using the `platformio device monitor [...]` command, try adding the `--raw` argument - this will apply color to
log messages in your terminal.

### Setting custom MQTT topics

If esphomelib's default MQTT topics don't suit your needs, you can override them. For this, there are two options: 

#### 1. Set the global MQTT topic prefix.

By default, all MQTT topics are named with the application name (see `set_name()`), for example `livingroom/...`.
However, if you want to use your own MQTT topic prefixes like `home/livingroom/node1/...`, this is possible:

```cpp
auto *mqtt = app.init_mqtt(...);
mqtt->set_topic_prefix("home/livingroom/node1");
```

#### 2. Customize the MQTT topics of a MQTTComponent
 
Customizing the MQTT state/command topics of a single MQTTComponent is also possible. Simple call the 
`set_custom_*_topic()` on your MQTTComponent like this:

```cpp
auto dht = app.make_dht_component(12, "Livingroom Temperature", "Livingroom Humidity");
dht.mqtt_temperature->set_custom_state_topic("home/livingroom/node1/temperature/state");
```

### OTA Updates

If you call `app.init_ota()` during `setup()`, esphomelib will continuously listen for over-the-air updates. 
To push an OTA update to your device, simply add `upload_port = `*`IP_OF_YOUR_ESP32`* to your `platform.ini`.
Then do the upload as you would do normally via serial. You might want to [set a static IP](#static-ips) for your ESP32.

> Note: OTA is, by default, enabled without any authentication. If you're on a public WiFi network, it's highly
> encouraged to set a paraphrase using the `set_auth_*()` methods on the object returned by `init_ota()`. Then also
> include `upload_flags = -a `*`PASSPHRASE`* in your `platformio.ini`.

### Help! I'm getting lots of `PubSubClient::publish() failed` warnings.

This due to this libraries MQTT client library ([PubSubClient](https://github.com/knolleary/pubsubclient)) having a
fixed-sized buffer for all MQTT messages. And if your messages are very long, they may not fit inside the buffer, so
PubSubClient won't send the message. To fix this, increase the buffer size, by adding this to your `platformio.ini`:

```ini
build_flags =
    -DMQTT_MAX_PACKET_SIZE=1024
```

Increase the number if it still doesn't fit.
