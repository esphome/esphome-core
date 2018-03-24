Logging Engine
==============

esphomelib will by default log to both Serial (with baudrate 115200) and MQTT (with topic <name>/debug).

API Reference
-------------

log.h
*****

.. doxygenenum:: ESPLogLevel
.. doxygendefine:: ESPHOMELIB_LOG_LEVEL

LogComponent
************

.. doxygenclass:: esphomelib::LogComponent
    :members:
    :protected-members:
    :undoc-members:
