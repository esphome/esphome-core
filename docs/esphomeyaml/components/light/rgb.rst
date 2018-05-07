RGB Light
=========

The ``rgb`` light platform creates an RGB light from 3 `output
components </esphomeyaml/components/output/index.html>`__ (one for each
color channel).

|image0|

.. code:: yaml

    # Example configuration entry
    light:
      - platform: rgb
        name: "Living Room Lights"
        red: output_component1
        green: output_component2
        blue: output_component3

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): The name of the light.
-  **red** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   float `output component </esphomeyaml/components/output/index.html>`__
   to use for the red channel.
-  **green** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   float `output component </esphomeyaml/components/output/index.html>`__
   to use for the green channel.
-  **blue** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   float `output component </esphomeyaml/components/output/index.html>`__
   to use for the blue channel.
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

.. |image0| image:: /esphomeyaml/components/light/rgb-light.png
   :class: align-center
   :width: 40.0%
