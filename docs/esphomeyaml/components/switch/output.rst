Generic Output Switch
=====================

The ``output`` switch platform allows you to use any output component as a switch.


|image0|

.. code:: yaml

    # Example configuration entry
    output:
      - platform: gpio
        pin: 25
        id: 'generic_out'
    switch:
      - platform: output
        name: "Generic Output"
        output: 'generic_out'

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **output** (**Required**, `id </esphomeyaml/configuration-types.html#id>`__): The
   ID of the output component to use.
-  **name** (**Required**, string): The name for the switch.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from
   `Switch </esphomeyaml/components/switch/index.html#base-switch-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/switch/output.png
   :class: align-center
   :width: 80.0%
