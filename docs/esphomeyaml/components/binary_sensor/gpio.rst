GPIO Binary Sensor
==================

The GPIO Binary Sensor platform allows you to use any input pin on your
device as a binary sensor.

|image0|

.. code:: yaml

    # Example configuration entry
    binary_sensor:
      - platform: gpio
        pin: D2
        name: "Living Room Window"
        device_class: window

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The
   pin to periodically check.
-  **name** (**Required**, string): The name of the binary sensor.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from `Binary
   Sensor </esphomeyaml/components/binary_sensor/index.html#base-binary-sensor-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/binary_sensor/gpio.png
   :class: align-center
   :width: 80.0%
