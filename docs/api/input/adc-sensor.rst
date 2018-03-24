ADC Sensor
==========

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_adc_sensor(13, "Analog Voltage");
    // Custom update interval 30 seconds
    App.make_adc_sensor(13, "Analog Voltage", 30000);
    // Custom pinMode
    App.make_adc_sensor(GPIOInputPin(13, INPUT_PULLUP), "Analog Voltage");
    // ESP32: Attenuation
    auto adc = App.make_adc_sensor(13, "Analog Voltage");
    adc.adc.set_attenuation(ADC_11db);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_adc_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::input::ADCSensorComponent
    :members:
    :protected-members:
    :undoc-members:
