Dallas Component
================

.. cpp:namespace:: esphomelib::input

This class allows using Dallas (DS18b20) devices with esphomelib. You first have to
create a hub or bus where all sensors are connected to (:cpp:class:`DallasComponent`).

.. cpp:namespace:: esphomelib::input::DallasComponent

Next, use :cpp:func:`get_sensor_by_address` and :cpp:func:`get_sensor_by_index` to get
individual sensors. You can get the addresses of dallas sensors by observing the log output
at startup time.

Example Usage
-------------

.. code-block:: cpp

    // Bus setup
    auto *dallas = App.make_dallas_component(15);

    // By address
    App.make_mqtt_sensor_for(dallas->get_sensor_by_address(0xfe0000031f1eaf29),
                             "Ambient Temperature");
    // With update interval, every 30 seconds
    App.make_mqtt_sensor_for(dallas->get_sensor_by_address(0x710000031f0e7e28, 30000),
                             "Heatpump Temperature");
    // By index
    App.make_mqtt_sensor_for(dallas->get_sensor_by_index(0),
                             "Ambient Temperature");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_dallas_component` and :cpp:func:`Application::make_mqt_sensor_for`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::input::DallasComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::input::DallasTemperatureSensor
    :members:
    :protected-members:
    :undoc-members:
