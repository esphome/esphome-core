esphomelib API Reference
========================

Welcome to the esphomelib API docs!
-----------------------------------

Here you will find all of esphomelib's API documentation together with some examples.

Eventually, guides will also be setup here on how to create your own custom components
and some basic stuff.

Supported First Order Integrations
----------------------------------

esphomelib is using many layers of abstraction so that you can use all of these 1st
order integrations with all 2nd order integrations (like lights or fans). For example,
a light is a combination of output components.

.. toctree::
    :maxdepth: 1

    api/binary_sensor/gpio.rst
    api/output/esp8266-pwm.rst
    api/output/gpio-binary.rst
    api/output/ledc.rst
    api/output/pca9685.rst
    api/sensor/adc-sensor.rst
    api/sensor/ads1115.rst
    api/sensor/bmp085.rst
    api/sensor/dallas.rst
    api/sensor/dht.rst
    api/sensor/htu21d.rst
    api/sensor/hdc1080.rst
    api/sensor/pulse-counter.rst
    api/sensor/ultrasonic.rst
    api/switch/ir-transmitter.rst

API Reference
-------------

.. toctree::
    :maxdepth: 2

   	API Reference <api/index>


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
