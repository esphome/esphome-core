HTU21D Temperature/Humidity Sensor
==================================

.. warning:: This sensor is experimental has not been tested yet. If you can verify it works, notify me on Gitter.

The HTU21D component allows you get accurate temperature and humidity readings from your HTU21D
(`datasheet <https://cdn-shop.adafruit.com/datasheets/1899_HTU21D.pdf>`_,
`adafruit <https://www.adafruit.com/product/1899>`_) sensors with esphomelib. To use these
i2c-based devices, first initialize the i2c bus using the pins you have for SDA and SCL and
then create the sensors themselves as in below example.

Example Usage
-------------

.. code-block:: cpp

    // inside setup()
    App.init_i2c(SDA_PIN, SCL_PIN); // change these values for your pins.
    // create sensors
    App.make_htu21d_sensor("Outside Temperature", "Outside Humidity");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_htu21d_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::sensor::HTU21DComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::HTU21DTemperatureSensor
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::HTU21DHumiditySensor
    :members:
    :protected-members:
    :undoc-members:
