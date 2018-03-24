Binary Sensor
=============

.. cpp:namespace:: esphomelib::binary_sensor

In esphomelib, every component that exposes a binary state, is a :cpp:class:`BinarySensor`.

To create your own binary sensor, simply subclass :cpp:class:`BinarySensor` and call
:cpp:func:`BinarySensor::publish_state` to tell the frontend that you have a new state.
Inversion is automatically done for you when publishing state and can be changed by the
user with :cpp:func:`BinarySensor::set_inverted`.

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_mqtt_binary_sensor_for("Custom Binary Sensor",
                                    binary_sensor::device_class::MOTION,
                                    custom_binary_sensor);
    // GPIO Binary Sensor
    App.make_gpio_binary_sensor(36, "Window Open",
                                binary_sensor::device_class::WINDOW);


.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_mqtt_binary_sensor_for` and :cpp:func:`Application::make_gpio_binary_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

BinarySensor
************

.. doxygenclass:: esphomelib::binary_sensor::BinarySensor
    :members:
    :protected-members:
    :undoc-members:

MQTTBinarySensorComponent
*************************

.. doxygennamespace:: esphomelib::binary_sensor::device_class
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::binary_sensor::MQTTBinarySensorComponent
    :members:
    :protected-members:
    :undoc-members:
