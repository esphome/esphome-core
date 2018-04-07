esphomeyaml
===========

Hi there! This is the documentation for esphomeyaml, a project that aims to make using ESP8266/ESP32 boards with Home Assistant very easy with no programming experience required.

To get started, please see the `Getting Started Guide </esphomeyaml/getting-started.html>`__.

You can find the source for this project `on Github <https://github.com/OttoWinter/esphomeyaml>`__, and the core of this project, `esphomelib is also hosted on Github <https://github.com/OttoWinter/esphomelib>`__.


Core Components
~~~~~~~~~~~~~~~

==================  ==================  ==================
|Core|_             |WiFi|_             |MQTT|_
------------------  ------------------  ------------------
`Core`_             `WiFi`_             `MQTT`_
------------------  ------------------  ------------------
|I²C Bus|_          |OTA Updates|_      |Power Supply|_
------------------  ------------------  ------------------
`I²C Bus`_          `OTA Updates`_      `Power Supply`_
==================  ==================  ==================

.. |Core| image:: /esphomeyaml/cloud-circle.svg
    :class: component-image
.. _Core: /esphomeyaml/components/esphomeyaml.html

.. |WiFi| image:: /esphomeyaml/network-wifi.svg
    :class: component-image
.. _WiFi: /esphomeyaml/components/wifi.html

.. |MQTT| image:: /esphomeyaml/mqtt.png
    :class: component-image
.. _MQTT: /esphomeyaml/components/mqtt.html

.. |I²C Bus| image:: /esphomeyaml/i2c.svg
    :class: component-image
.. _I²C Bus: /esphomeyaml/components/i2c.html

.. |OTA Updates| image:: /esphomeyaml/system-update.svg
    :class: component-image
.. _OTA Updates: /esphomeyaml/components/ota.html

.. |Power Supply| image:: /esphomeyaml/power.svg
    :class: component-image
.. _Power Supply: /esphomeyaml/components/power_supply.html


Sensor Components
~~~~~~~~~~~~~~~~~

======================  ======================  ======================
|Sensor Core|_          |ADC|_                  |ADS1115|_
----------------------  ----------------------  ----------------------
`Sensor Core`_          `ADC`_                  `ADS1115`_
----------------------  ----------------------  ----------------------
|BMP085|_               |Dallas|_               |DHT|_
----------------------  ----------------------  ----------------------
`BMP085`_               `Dallas`_               `DHT`_
----------------------  ----------------------  ----------------------
|HDC1080|_              |HTU21D|_               |Pulse Counter|_
----------------------  ----------------------  ----------------------
`HDC1080`_              `HTU21D`_               `Pulse Counter`_
----------------------  ----------------------  ----------------------
|Ultrasonic Sensor|_
----------------------  ----------------------  ----------------------
`Ultrasonic Sensor`_
======================  ======================  ======================

.. |Sensor Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Sensor Core: /esphomeyaml/components/sensor/index.html

.. |ADC| image:: /esphomeyaml/flash.svg
    :class: component-image
.. _ADC: /esphomeyaml/components/sensor/adc.html

.. |ADS1115| image:: /esphomeyaml/ads1115.jpg
    :class: component-image
.. _ADS1115: /esphomeyaml/components/sensor/ads1115.html

.. |BMP085| image:: /esphomeyaml/bmp180.jpg
    :class: component-image
.. _BMP085: /esphomeyaml/components/sensor/bmp085.html

.. |Dallas| image:: /esphomeyaml/ds18b20.jpg
    :class: component-image
.. _Dallas: /esphomeyaml/components/sensor/dallas.html

.. |DHT| image:: /esphomeyaml/dht22.jpg
    :class: component-image
.. _DHT: /esphomeyaml/components/sensor/dht.html

.. |HDC1080| image:: /esphomeyaml/HDC1080.jpg
    :class: component-image
.. _HDC1080: /esphomeyaml/components/sensor/hdc1080.html

.. |HTU21D| image:: /esphomeyaml/htu21d.jpg
    :class: component-image
.. _HTU21D: /esphomeyaml/components/sensor/htu21d.html

.. |Pulse Counter| image:: /esphomeyaml/pulse.svg
    :class: component-image
.. _Pulse Counter: /esphomeyaml/components/sensor/pulse_counter.html

.. |Ultrasonic Sensor| image:: /esphomeyaml/hc-sr04.png
    :class: component-image
.. _Ultrasonic Sensor: /esphomeyaml/components/sensor/ultrasonic.html


Binary Sensor Components
~~~~~~~~~~~~~~~~~~~~~~~~

======================  ======================  ======================
|Binary Sensor Core|_   |GPIO|_                 |Status|_
----------------------  ----------------------  ----------------------
`Binary Sensor Core`_   `GPIO`_                 `Status`_
======================  ======================  ======================

.. |Binary Sensor Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Binary Sensor Core: /esphomeyaml/components/binary_sensor/index.html

.. |GPIO| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO: /esphomeyaml/components/binary_sensor/gpio.html

.. |Status| image:: /esphomeyaml/server-network.svg
    :class: component-image
.. _Status: /esphomeyaml/components/binary_sensor/status.html

Output Components
~~~~~~~~~~~~~~~~~

========================  ========================  ========================
|Output Core|_            |ESP8266 Software PWM|_   |GPIO Output|_
------------------------  ------------------------  ------------------------
`Output Core`_            `ESP8266 Software PWM`_   `GPIO Output`_
------------------------  ------------------------  ------------------------
|ESP32 LEDC|_             |PCA9685|_
------------------------  ------------------------  ------------------------
`ESP32 LEDC`_             `PCA9685`_
========================  ========================  ========================

.. |Output Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Output Core: /esphomeyaml/components/output/index.html

.. |ESP8266 Software PWM| image:: /esphomeyaml/pwm.png
    :class: component-image
.. _ESP8266 Software PWM: /esphomeyaml/components/output/esp8266_pwm.html

.. |GPIO Output| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO Output: /esphomeyaml/components/output/gpio.html

.. |ESP32 LEDC| image:: /esphomeyaml/pwm.png
    :class: component-image
.. _ESP32 LEDC: /esphomeyaml/components/output/ledc.html

.. |PCA9685| image:: /esphomeyaml/pca9685.jpg
    :class: component-image
.. _PCA9685: /esphomeyaml/components/output/pca9685.html


Light Components
~~~~~~~~~~~~~~~~

========================  ========================  ========================
|Light Core|_             |Binary Light|_           |Monochromatic Light|_
------------------------  ------------------------  ------------------------
`Light Core`_             `Binary Light`_           `Monochromatic Light`_
------------------------  ------------------------  ------------------------
|RGB Light|_              |RGBW Light|_
------------------------  ------------------------  ------------------------
`RGB Light`_              `RGBW Light`_
========================  ========================  ========================


.. |Light Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Light Core: /esphomeyaml/components/light/index.html

.. |Binary Light| image:: /esphomeyaml/lightbulb.svg
    :class: component-image
.. _Binary Light: /esphomeyaml/components/light/binary.html

.. |Monochromatic Light| image:: /esphomeyaml/brightness-medium.svg
    :class: component-image
.. _Monochromatic Light: /esphomeyaml/components/light/monochromatic.html

.. |RGB Light| image:: /esphomeyaml/rgb.png
    :class: component-image
.. _RGB Light: /esphomeyaml/components/light/rgb.html

.. |RGBW Light| image:: /esphomeyaml/rgbw.png
    :class: component-image
.. _RGBW Light: /esphomeyaml/components/light/rgbw.html


Switch Components
~~~~~~~~~~~~~~~~~

========================  ========================  ========================
|Switch Core|_            |GPIO Switch|_            |IR Transmitter|_
------------------------  ------------------------  ------------------------
`Switch Core`_            `GPIO Switch`_            `IR Transmitter`_
------------------------  ------------------------  ------------------------
|Restart Switch|_
------------------------  ------------------------  ------------------------
`Restart Switch`_
========================  ========================  ========================

.. |Switch Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Switch Core: /esphomeyaml/components/switch/index.html

.. |GPIO Switch| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO Switch: /esphomeyaml/components/switch/gpio.html

.. |IR Transmitter| image:: /esphomeyaml/remote.svg
    :class: component-image
.. _IR Transmitter: /esphomeyaml/components/switch/ir_transmitter.html

.. |Restart Switch| image:: /esphomeyaml/restart.svg
    :class: component-image
.. _Restart Switch: /esphomeyaml/components/switch/restart.html


Fan Components
~~~~~~~~~~~~~~

========================  ========================  ========================
|Fan Core|_               |Binary Fan|_             |Speed Fan|_
------------------------  ------------------------  ------------------------
`Fan Core`_               `Binary Fan`_             `Speed Fan`_
========================  ========================  ========================

.. |Fan Core| image:: /esphomeyaml/folder-open.svg
    :class: component-image
.. _Fan Core: /esphomeyaml/components/fan/index.html

.. |Binary Fan| image:: /esphomeyaml/fan.svg
    :class: component-image
.. _Binary Fan: /esphomeyaml/components/fan/binary.html

.. |Speed Fan| image:: /esphomeyaml/fan.svg
    :class: component-image
.. _Speed Fan: /esphomeyaml/components/fan/speed.html


Misc Components
~~~~~~~~~~~~~~~

========================  ========================  ========================
|Dallas Hub|_             |IR Transmitter Hub|_     |PCA9685 Hub|_
------------------------  ------------------------  ------------------------
`Dallas Hub`_             `IR Transmitter Hub`_     `PCA9685 Hub`_
------------------------  ------------------------  ------------------------
|ADS1115 Hub|_
------------------------  ------------------------  ------------------------
`ADS1115 Hub`_
========================  ========================  ========================

.. |Dallas Hub| image:: /esphomeyaml/ds18b20.jpg
    :class: component-image
.. _Dallas Hub: /esphomeyaml/components/dallas.html

.. |IR Transmitter Hub| image:: /esphomeyaml/remote.svg
    :class: component-image
.. _IR Transmitter Hub: /esphomeyaml/components/ir_transmitter.html

.. |PCA9685 Hub| image:: /esphomeyaml/pca9685.jpg
    :class: component-image
.. _PCA9685 Hub: /esphomeyaml/components/pca9685.html

.. |ADS1115 Hub| image:: /esphomeyaml/ads1115.jpg
    :class: component-image
.. _ADS1115 Hub: /esphomeyaml/components/ads1115.html


Full Index
~~~~~~~~~~

.. toctree::
    :maxdepth: 1

    getting-started.rst
    configuration-types.rst
    components/esphomeyaml.rst
    components/dallas.rst
    components/sensor/dallas.rst
    components/i2c.rst
    components/logger.rst
    components/wifi.rst
    components/mqtt.rst
    components/ota.rst
    components/ads1115.rst
    components/binary_sensor/index.rst
    components/binary_sensor/gpio.rst
    components/binary_sensor/status.rst
    components/sensor/index.rst
    components/sensor/adc.rst
    components/sensor/ads1115.rst
    components/sensor/bmp085.rst
    components/sensor/dht.rst
    components/sensor/hdc1080.rst
    components/sensor/htu21d.rst
    components/sensor/pulse_counter.rst
    components/sensor/ultrasonic.rst
    components/output/index.rst
    components/power_supply.rst
    components/output/esp8266_pwm.rst
    components/output/gpio.rst
    components/output/ledc.rst
    components/pca9685.rst
    components/output/pca9685.rst
    components/light/index.rst
    components/light/binary.rst
    components/light/monochromatic.rst
    components/light/rgb.rst
    components/light/rgbw.rst
    components/switch/index.rst
    components/switch/gpio.rst
    components/ir_transmitter.rst
    components/switch/ir_transmitter.rst
    components/switch/restart.rst
    components/fan/index.rst
    components/fan/binary.rst
    components/fan/speed.rst
