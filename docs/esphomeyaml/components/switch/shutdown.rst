Shutdown Switch
===============

The ``shutdown`` switch platform allows you to shutdown your node remotely
through Home Assistant. It does this by putting the node into deep sleep mode with no
wakeup source selected. After enabling, the only way to startup the ESP again is by
pressing the reset button or restarting the power supply.

|image0|

.. code:: yaml

    # Example configuration entry
    switch:
      - platform: shutdown
        name: "Living Room Shutdown"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): The name for the switch.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from
   `Switch </esphomeyaml/components/switch/index.html#base-switch-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/switch/shutdown.png
   :class: align-center
   :width: 80.0%
