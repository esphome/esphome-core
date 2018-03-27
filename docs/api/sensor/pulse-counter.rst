ESP32 Pulse Counter Sensor
==========================

This component allows you to count pulses on a PIN using the internal
`pulse counter <http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/pcnt.html>`_ peripheral of
the ESP32. By default, the values by this sensor are reported in "pulses/min". You can convert
that to your own unit like in below example.

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

.. doxygenclass:: esphomelib::sensor::PulseCounterSensorComponent
    :members:
    :protected-members:
    :undoc-members:
