GPIO Binary Sensor
==================

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_gpio_binary_sensor(36, "Window Open",
                                binary_sensor::device_class::WINDOW);
    // Custom pinMode
    App.make_gpio_binary_sensor(GPIOInputPin(36, INPUT_PULLUP), "Window Open",
                                binary_sensor::device_class::WINDOW);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_gpio_binary_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::input::GPIOBinarySensorComponent
    :members:
    :protected-members:
    :undoc-members:
