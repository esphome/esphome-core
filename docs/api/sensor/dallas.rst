Dallas Component
================

.. cpp:namespace:: esphomelib::input

This class allows using Dallas (DS18b20) devices with esphomelib. You first have to
create a hub or bus where all sensors are connected to (:cpp:class:`DallasComponent`).

.. cpp:namespace:: esphomelib::sensor::DallasComponent

Next, use :cpp:func:`get_sensor_by_address` and :cpp:func:`get_sensor_by_index` to get
individual sensors. You can get the addresses of dallas sensors by observing the log output
at startup time.

Example Usage
-------------

.. code-block:: cpp

    // Bus setup
    auto *dallas = App.make_dallas_component(15);

    // By address
    App.register_sensor(dallas->get_sensor_by_address("Ambient Temperature", 0xfe0000031f1eaf29));
    // By index
    App.register_sensor(dallas->get_sensor_by_index("Ambient Temperature", 0));

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_dallas_component` and :cpp:func:`Application::register_sensor`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::sensor::DallasComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::sensor::DallasTemperatureSensor
    :members:
    :protected-members:
    :undoc-members:
