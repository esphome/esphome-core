Sensor
======

The `esphomelib::sensor` namespace contains all sensors.

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_mqtt_sensor_for`.

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
