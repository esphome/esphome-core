Helpers
=======

esphomelib uses a bunch of helpers to make the library easier to use.

API Reference
-------------

helpers.h
*********

.. doxygentypedef:: esphomelib::json_parse_t

.. doxygentypedef:: esphomelib::json_build_t

.. doxygenvariable:: esphomelib::HOSTNAME_CHARACTER_WHITELIST

.. doxygenfunction:: esphomelib::get_mac_address

.. doxygenfunction:: esphomelib::generate_hostname

.. doxygenfunction:: esphomelib::sanitize_hostname

.. doxygenfunction:: esphomelib::truncate_string

.. doxygenfunction:: esphomelib::is_empty

.. doxygenfunction:: esphomelib::shutdown

.. doxygenfunction:: esphomelib::add_shutdown_hook

.. doxygenfunction:: esphomelib::safe_shutdown

.. doxygenfunction:: esphomelib::add_safe_shutdown_hook

.. doxygenfunction:: esphomelib::to_lowercase_underscore

.. doxygenfunction:: esphomelib::build_json

.. doxygenfunction:: esphomelib::parse_json

.. doxygenfunction:: esphomelib::clamp

.. doxygenfunction:: esphomelib::lerp

.. doxygenfunction:: esphomelib::make_unique

.. doxygenfunction:: esphomelib::random_uint32

.. doxygenfunction:: esphomelib::random_double

.. doxygenfunction:: esphomelib::random_float

.. doxygenfunction:: esphomelib::gamma_correct

.. doxygenfunction:: esphomelib::value_accuracy_to_string

.. doxygenfunction:: esphomelib::uint64_to_string

.. doxygenfunction:: esphomelib::sanitize_string_whitelist

.. doxygenfunction:: esphomelib::run_without_interrupts

.. doxygenclass:: esphomelib::Optional
    :members:
    :protected-members:
    :undoc-members:

.. doxygenfunction:: esphomelib::parse_on_off

.. doxygenclass:: esphomelib::SlidingWindowMovingAverage
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::ExponentialMovingAverage
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::CallbackManager
    :members:
    :protected-members:
    :undoc-members:

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

defines.h
*********

.. doxygenfile:: esphomelib/defines.h
