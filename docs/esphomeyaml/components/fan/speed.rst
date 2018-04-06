Speed Fan
=========

The ``speed`` fan platform lets you represent any float `ouput
component </esphomeyaml/components/output/index.html>`__ as a fan that
supports speed settings.

|image0|

.. code:: yaml

    # Example configuration entry
    fan:
      - platform: speed
        output: myoutput_1
        name: "Living Room Fan"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **output** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   binary output component to use for this fan.
-  **name** (**Required**, string): The name for this fan.
-  **oscillation_output** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): The output to use
   for the oscillation state of this fan. Default is empty.
-  **speed** (*Optional*): Set the float values for each speed setting:

   -  **low** (*Required*, float): Set the value for the low speed
      setting. Must be in range 0 to 1. Defaults to 0.33.
   -  **medium** (*Required*, float): Set the value for the medium speed
      setting. Must be in range 0 to 1. Defaults to 0.66.
   -  **high** (*Required*, float): Set the value for the high speed
      setting. Must be in range 0 to 1. Defaults to 1.

-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__
   and `Fan
   Component </esphomeyaml/components/fan/index.html#base-fan-configuration>`__.

.. |image0| image:: /esphomeyaml/components/fan/fan.png
   :class: align-center
   :width: 80.0%
