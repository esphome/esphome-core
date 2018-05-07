Monochromatic Light
===================

The ``monochromatic`` light platform creates a simple light
brightness-only from an `output
component </esphomeyaml/components/output/index.html>`__.

|image0|

.. code:: yaml

    # Example configuration entry
    light:
      - platform: monochromatic
        name: "Kitchen Lights"
        output: output_component1

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): The name of the light.
-  **output** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   float `output component </esphomeyaml/components/output/index.html>`__
   to use for this light.
-  **gamma_correct** (*Optional*, float): The `gamma correction
   factor <https://en.wikipedia.org/wiki/Gamma_correction>`__ for the
   light. Defaults to ``2.8``.
-  **default_transition_length** (*Optional*,
   `time </esphomeyaml/configuration-types.html#time>`__): The length of
   the transition if no transition parameter is provided by Home
   Assistant. Defaults to ``1s``.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/light/kitchen-lights.png
   :class: align-center
   :width: 80.0%
