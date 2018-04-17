GPIO Binary Sensor
==================

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_gpio_binary_sensor("Window Open", 36);
    // Custom pinMode
    App.make_gpio_binary_sensor("Window Open", GPIOInputPin(36, INPUT_PULLUP));

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_gpio_binary_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::binary_sensor::GPIOBinarySensorComponent
    :members:
    :protected-members:
    :undoc-members:
