Ultrasonic Sensor
=================

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.make_ultrasonic(12, 13); // trigger pin: 12, echo pin: 13
    // Filter out timeouts
    auto ultrasonic = App.make_ultrasonic(12, 13);
    ultrasonic.mqtt->set_filters({
        new sensor::FilterOutNANFilter(), // filter out timeouts
    });
    // Set timeout, 4 meters
    ultrasonic.sensor->set_timeout_m(4.0f);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_ultrasonic_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::input::UltrasonicSensorComponent
    :members:
    :protected-members:
    :undoc-members:
