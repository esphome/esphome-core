Helpers
=======

esphomelib uses a bunch of helpers to make the library easier to use.

API Reference
-------------

helpers.h
*********

.. doxygenfile:: esphomelib/helpers.h

ESPPreferences
**************

.. doxygenclass:: esphomelib::ESPPreferences
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::global_preferences

esphal.h
********

This header should be used whenever you want to access some `digitalRead`, `digitalWrite`, ... methods.

.. doxygenclass:: esphomelib::GPIOPin
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::GPIOOutputPin
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::GPIOInputPin
    :members:
    :protected-members:
    :undoc-members:


ESPOneWire
**********

esphomelib has its own implementation of OneWire, because the implementation in the Arduino libraries
seems to have lots of timing issues with the ESP8266/ESP32. That's why ESPOneWire was created.

.. doxygenclass:: esphomelib::ESPOneWire
    :members:
    :protected-members:
    :undoc-members:
