OTA Update Component
====================

With the OTA (Over The Air) update component you can upload your
firmware binaries to your node without having to use an USB cable for
uploads. esphomeyaml natively supports this through its ``run`` and
``upload`` helper scripts.

Optionally, you can also define a password to use for OTA updates so
that an intruder isn’t able to upload any firmware to the ESP without
having hardware access to it. This password is also hashed
automatically, so an intruder can’t extract the password from the
binary.

esphomelib also supports an “OTA safe mode”. If for some reason your
node gets into a boot loop, esphomelib will automatically try to detect
this and will go over into a safe mode after 10 unsuccessful boot
attempts. In that mode, all components are disabled and only Serial
Logging+WiFi+OTA are initialized, so that you can upload a new binary.

.. code:: yaml

    # Example configuration entry
    ota:
      safe_mode: True
      password: VERYSECURE

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **safe_mode** (*Optional*, boolean): Whether to enable safe mode.
   Defaults to ``True``.
-  **password** (*Optional*, string): The password to use for updates.
-  **port** (*Optional*, int): The port to use for OTA updates. Defaults
   to ``3232`` for the ESP32 and ``8266`` for the ESP8266.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
