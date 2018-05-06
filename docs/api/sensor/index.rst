Sensor
======

The `esphomelib::sensor` namespace contains all sensors.

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::register_sensor`.

.. toctree::
    :maxdepth: 1

    ADC Sensor Component <adc-sensor>
    Dallas Component <dallas>
    DHT Component <dht>
    ESP32 Pulse Counter Sensor <pulse-counter>
    Ultrasonic Sensor Component <ultrasonic>
    ADS1115 ADC Component <ads1115>
    BMP085 Pressure/Temperature Sensor <bmp085>
    HTU21D Temperature/Humidity Sensor <htu21d>
    HDC1080 Temperature/Humidity Sensor <hdc1080>


API Reference
-------------

.. cpp:namespace:: nullptr

Sensor
******

.. doxygenclass:: esphomelib::sensor::Sensor
    :members:
    :protected-members:
    :undoc-members:

.. doxygentypedef:: esphomelib::sensor::sensor_callback_t

.. doxygenclass:: esphomelib::sensor::PollingSensorComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::EmptySensor
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::EmptyPollingParentSensor
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::sensor::ICON_EMPTY
.. doxygenvariable:: esphomelib::sensor::ICON_WATER_PERCENT
.. doxygenvariable:: esphomelib::sensor::ICON_GAUGE
.. doxygenvariable:: esphomelib::sensor::ICON_FLASH
.. doxygenvariable:: esphomelib::sensor::ICON_SCREEN_ROTATION
.. doxygenvariable:: esphomelib::sensor::ICON_BRIEFCASE_DOWNLOAD

.. doxygenvariable:: esphomelib::sensor::UNIT_C
.. doxygenvariable:: esphomelib::sensor::UNIT_PERCENT
.. doxygenvariable:: esphomelib::sensor::UNIT_HPA
.. doxygenvariable:: esphomelib::sensor::UNIT_V
.. doxygenvariable:: esphomelib::sensor::UNIT_DEGREES_PER_SECOND
.. doxygenvariable:: esphomelib::sensor::UNIT_M_PER_S_SQAURED


Filter
******

.. doxygenclass:: esphomelib::sensor::Filter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::SlidingWindowMovingAverageFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::ExponentialMovingAverageFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygentypedef:: esphomelib::sensor::lambda_filter_t

.. doxygenclass:: esphomelib::sensor::LambdaFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::OffsetFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::MultiplyFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::FilterOutValueFilter
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::FilterOutNANFilter
    :members:
    :protected-members:
    :undoc-members:

MQTTSensorComponent
*******************

.. doxygenclass:: esphomelib::sensor::MQTTSensorComponent
    :members:
    :protected-members:
    :undoc-members:
