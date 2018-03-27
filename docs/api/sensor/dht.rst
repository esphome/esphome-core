DHT Temperature/Humidity Sensor
===============================

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_dht_sensor(12, "Outside Temperature", "Outside Humidity");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_dht_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::sensor::DHTComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::DHTTemperatureSensor
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::DHTHumiditySensor
    :members:
    :protected-members:
    :undoc-members:
