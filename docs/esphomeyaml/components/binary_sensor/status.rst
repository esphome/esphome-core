Status Binary Sensor
====================

The Status Binary Sensor exposes the node state (if it’s connected to
MQTT or not) for Home Assistant. It uses the `MQTT birth and last will
messages </esphomeyaml/components/mqtt.html#last-will-and-birth-messages>`__
to do this.

|image0|

.. code:: yaml

    # Example configuration entry
    binary_sensor:
      - platform: status
        name: "Living Room Status"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **name** (**Required**, string): The name of the binary sensor.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from `Binary
   Sensor </esphomeyaml/components/binary_sensor/index.html#base-binary-sensor-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.
   Inverted is not supported.

.. |image0| image:: /esphomeyaml/components/binary_sensor/status.png
   :class: align-center
   :width: 80.0%
