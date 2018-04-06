Binary Fan
==========

The ``binary`` fan platform lets you represent any binary `ouput
component </esphomeyaml/components/output/index.html>`__ as a fan.

|image0|

.. code:: yaml

    # Example configuration entry
    fan:
      - platform: binary
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
