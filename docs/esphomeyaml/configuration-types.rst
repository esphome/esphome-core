Configuration Types
===================

esphomeyaml’s configuration files have several configuration types. This
page describes them.

ID
~~

Quite an important aspect of esphomeyaml are “ids”. They are used to
connect components from different domains. For example, you define an
output component together with an id and then later specify that same id
in the light component. IDs should always be unique within a
configuration and esphomeyaml will warn you if you try to use the same
ID twice.

Because esphomeyaml converts your configuration into C++ code and the
ids are in reality just C++ variable names, they must also adhere to
C++’s naming conventions. `C++ Variable
names <http://venus.cs.qc.cuny.edu/~krishna/cs111/lectures/D3_C++_Variables.pdf>`__
…

-  … must start with a letter and can end with numbers.
-  … must not have a space in the name.
-  … can not have special characters except the underscore (“_“).
-  … must not be a keyword.

Pin
~~~

esphomeyaml always uses the **chip-internal GPIO numbers**. These
internal numbers are always integers like ``16`` and can be prefixed by
``GPIO``. For example to use the pin with the **internal** GPIO number 16,
you could type ``GPIO16`` or just ``16``.

Most boards however have aliases for certain pins. For example the NodeMCU
ESP8266 uses pin names ``D0`` through ``D8`` as aliases for the internal GPIO
pin numbers. Each board (defined in `esphomeyaml
section </esphomeyaml/components/esphomeyaml.html>`__)
has their own aliases and so not all of them are supported yet. For example,
for the ``D0`` (as printed on the PCB silkscreen) pin on the NodeMCU ESP8266
has the internal GPIO name ``GPIO16``, but also has an alias ``D0``. So using
either one of these names in your configuration will lead to the same result.

.. code:: yaml

    some_config_option:
      pin: GPIO16

    some_config_option:
      # alias on the NodeMCU ESP8266:
      pin: D0

Pin Schema
~~~~~~~~~~

In some places, esphomeyaml also supports a more advanced “pin schema”.

.. code:: yaml

    some_config_option:
      # Basic:
      pin: D0

      # Advanced:
      pin:
        number: D0
        inverted: True
        mode: INPUT_PULLUP

Configuration variables:

-  **number** (**Required**, pin): The pin number.
-  **inverted** (*Optional*, boolean): If all read and written values
   should be treated as inverted. Defaults to ``False``.
-  **mode** (*Optional*, string): A pin mode to set for the pin at
   startup, corresponds to Arduino’s ``pinMode`` call.

Available Pin Modes:

-  ``INPUT``
-  ``OUTPUT``
-  ``INPUT_PULLDOWN`` (only on ESP32)
-  ``INPUT_PULLUP``
-  ``OUTPUT_OPEN_DRAIN``
-  ``WAKEUP_PULLUP`` (only on ESP8266)
-  ``WAKEUP_PULLDOWN`` (only on ESP8266)
-  ``INPUT_PULLDOWN_16`` (only on ESP8266 and only on GPIO16)
-  ``OPEN_DRAIN`` (only on ESP32)
-  ``PULLUP`` (only on ESP32)
-  ``PULLDOWN`` (only on ESP32)
-  ``ANALOG`` (only on ESP32)
-  ``SPECIAL``
-  ``FUNCTION_0`` (only on ESP8266)
-  ``FUNCTION_1``
-  ``FUNCTION_2``
-  ``FUNCTION_3``
-  ``FUNCTION_4``
-  ``FUNCTION_5`` (only on ESP32)
-  ``FUNCTION_6`` (only on ESP32)

Time
~~~~

In lots of places in esphomeyaml you need to define time lengths, most
notably of these is an ``update_interval`` in milliseconds. Internally,
epshomelib always tries to work with millisecond values. But as
esphomeyaml is trying to make your lifes as easy as possible, there are
lots of helpers for defining times.

.. code:: yaml

    some_config_option:
      some_time_option: 1000ms  # 1000 milliseconds
      some_time_option: 1.5s  # 1.5 seconds
      some_time_option: 0.5min  # half a minute
      some_time_option: 2h  # 2 hours
      some_time_option:  # 10ms + 30s + 25min + 3h
        milliseconds: 10
        seconds: 30
        minutes: 25
        hours: 3
        days: 0
