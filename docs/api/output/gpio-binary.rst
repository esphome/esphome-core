GPIO Binary Output
==================

Example Usage
-------------

.. code-block:: cpp

    // Create a binary output, *not a switch*
    App.make_gpio_output(33);
    // Custom pinMode
    App.make_gpio_output(GPIOOutputPin(33, OUTPUT_OPEN_DRAIN));

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_gpio_output` (and :cpp:func:`Application::make_gpio_switch`).

API Reference
-------------

.. cpp:namespace:: nullptr

GPIOBinaryOutputComponent
*************************

.. doxygenclass:: esphomelib::output::GPIOBinaryOutputComponent
    :members:
    :protected-members:
    :undoc-members:
