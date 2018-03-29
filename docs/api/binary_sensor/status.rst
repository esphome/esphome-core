Node Status Binary Sensor
=========================

This binary sensor platform allows you to create a switch that uses the birth and last will
messages by the MQTT client to show an ON/OFF state of the node.

Example Usage
-------------

.. code-block:: cpp

    App.make_status_binary_sensor("Livingroom Node Status");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_status_binary_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::binary_sensor::StatusBinarySensor
    :members:
    :protected-members:
    :undoc-members:
