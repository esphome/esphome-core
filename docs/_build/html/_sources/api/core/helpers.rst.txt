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
