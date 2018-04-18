esphomeyaml Core Configuration
==============================

Here you specify some core information that esphomeyaml needs to create
firmwares. Most importantly, this is the section of the configuration
where you specify the **name** of the node, the **platform** and
**board** you’re using.

.. code:: yaml

    # Example configuration entry
    esphomeyaml:
        name: livingroom
        platform: ESP32
        board: nodemcu-32s

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): This is the name of the node. It
   should always be unique to the node and no other node in your system
   can use the same name. It can also only contain upper/lowercase
   characters, digits and underscores.
-  **platform** (**Required**, string): The platform your board is on,
   either ``ESP32`` or ``ESP8266``.
-  **board** (**Required**, string): The board esphomeyaml should
   specify for platformio. For the ESP32, choose the appropriate one
   from `this
   list <http://docs.platformio.org/en/latest/platforms/espressif32.html#boards>`__
   and use `this
   list <http://docs.platformio.org/en/latest/platforms/espressif8266.html#boards>`__
   for ESP8266-based boards.
-  **library_uri** (*Optional*, string): You can manually specify the
   `version of
   esphomelib <https://github.com/OttoWinter/esphomelib/releases>`__ to
   use here. Accepts all parameters of `platformio lib
   install <http://docs.platformio.org/en/latest/userguide/lib/cmd_install.html#id2>`__.
   Use ``https://github.com/OttoWinter/esphomelib.git`` for the latest
   (unstable) build. Defaults to ``esphomelib``.
-  **simplify** (*Optional*, boolean): Whether to simplify the
   auto-generated code, i.e. whether to remove unused variables, use
   ``auto`` types and so on. Defaults to ``true``.
-  **use_build_flags** (*Optional*, boolean): If esphomeyaml should manually set
   build flags that specifically set what should be included in the binary. Most of
   this is already done automatically by the linker but this option can help with
   shrinking the firmware size while slowing down compilation. Defaults to ``false``.
