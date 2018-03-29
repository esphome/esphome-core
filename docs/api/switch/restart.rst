Restart Switch
==============

This platform allows you to restart your ESP8266/ESP32 with a simple MQTT message.

Example Usage
-------------

.. code-block:: cpp

    App.make_restart_switch("Livingroom Restart");

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_restart_switch`.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::switch_::RestartSwitch
    :members:
    :protected-members:
    :undoc-members:
