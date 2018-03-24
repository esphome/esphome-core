Pulse Counter Sensor
====================

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_pulse_counter_sensor(13, "Stromverbrauch Wärmepumpe");
    // Unit conversion
    auto strom_warme = App.make_pulse_counter_sensor(13, "Stromverbrauch Wärmepumpe");
    strom_warme.mqtt->set_unit_of_measurement("kW");
    strom_warme.mqtt->clear_filters();
    strom_warme.mqtt->add_multiply_filter(0.06f); // convert from Wh pulse to kW

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_pulse_counter_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::input::PulseCounterSensorComponent
    :members:
    :protected-members:
    :undoc-members:
