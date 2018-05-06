Shutdown Switch
===============

This platform allows you to put your ESP8266/ESP32 to sleep until it is manually restarted
by either pressing the reset button or toggling the power supply.

It is especially useful if you're using esphomelib with battery cells that must not
be discharged too much.

Example Usage
-------------

.. code-block:: cpp

    App.make_shutdown_switch("Livingroom Shutdown");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_shutdown_switch`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::switch_::ShutdownSwitch
    :members:
    :protected-members:
    :undoc-members:
