Binary Light
============

The ``binary`` light platform creates a simple light from an `output
component </esphomeyaml/components/output/index.html>`__ and can only be
turned ON or OFF.

|image0|

.. code:: yaml

    # Example configuration entry
    light:
      - platform: binary
        name: "Desk Lamp"
        output: output_component1

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): The name of the light.
-  **output** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   binary `output
   component </esphomeyaml/components/output/index.html>`__ to use for
   this light.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/light/binary-light.png
   :class: align-center
   :width: 80.0%
