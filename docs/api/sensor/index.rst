Sensor
======

The `esphomelib::sensor` namespace contains all sensor helpers.

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_mqtt_sensor_for`.

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
.. doxygenvariable:: esphomelib::sensor::UNIT_OF_MEASUREMENT_CELSIUS
.. doxygenvariable:: esphomelib::sensor::UNIT_OF_MEASUREMENT_PERCENT
.. doxygenvariable:: esphomelib::sensor::UNIT_OF_MEASUREMENT_VOLT
.. doxygenvariable:: esphomelib::sensor::UNIT_OF_MEASUREMENT_METER
.. doxygenvariable:: esphomelib::sensor::ICON_TEMPERATURE
.. doxygenvariable:: esphomelib::sensor::ICON_HUMIDITY
.. doxygenvariable:: esphomelib::sensor::ICON_VOLTAGE
.. doxygenvariable:: esphomelib::sensor::ICON_DISTANCE

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
