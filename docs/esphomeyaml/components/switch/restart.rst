Restart Switch
==============

The ``restart`` switch platform allows you to restart your node remotely
through Home Assistant.

|image0|

.. code:: yaml

    # Example configuration entry
    switch:
      - platform: restart
        name: "Living Room Restart"

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

.. |image0| image:: /esphomeyaml/components/switch/restart.png
   :class: align-center
   :width: 80.0%
