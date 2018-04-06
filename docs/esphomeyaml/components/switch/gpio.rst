GPIO Switch
===========

The ``gpio`` switch platform allows you to use any pin on your node as a
switch. You can for example hook up a relay to a GPIO pin and use it
through this platform.

|image0|

.. code:: yaml

    # Example configuration entry
    switch:
      - platform: gpio
        pin: 25
        name: "Living Room Dehumidifier"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The
   GPIO pin to use for the switch.
-  **name** (**Required**, string): The name for the switch.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from
   `Switch </esphomeyaml/components/switch/index.html#base-switch-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/switch/gpio.png
   :class: align-center
   :width: 80.0%
