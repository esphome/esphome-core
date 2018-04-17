ADC Sensor
==========

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_adc_sensor("Analog Voltage", 13);
    // Custom update interval 30 seconds
    App.make_adc_sensor("Analog Voltage", 13, 30000);
    // Custom pinMode
    App.make_adc_sensor("Analog Voltage", GPIOInputPin(13, INPUT_PULLUP));
    // ESP32: Attenuation
    auto adc = App.make_adc_sensor("Analog Voltage", 13);
    adc.adc.set_attenuation(ADC_11db);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_adc_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::sensor::ADCSensorComponent
    :members:
    :protected-members:
    :undoc-members:
