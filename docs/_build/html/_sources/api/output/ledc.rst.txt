ESP32 LEDC Output
=================

Example Usage
-------------

.. code-block:: cpp

    // Create a binary output, *not a switch*
    App.make_ledc_output(33);
    // Custom Frequency
    App.make_ledc_output(33, 2000.0);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_ledc_output`.

API Reference
-------------

.. cpp:namespace:: nullptr

LEDCOutputComponent
*******************

.. doxygenclass:: esphomelib::output::LEDCOutputComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::output::next_ledc_channel
