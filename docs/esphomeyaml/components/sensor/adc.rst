Analog To Digital Sensor
========================

The Analog To Digital (``adc``) Sensor allows you to use the built-in
ADC in your device to measure a voltage on certain pins. On the ESP8266
only pin A0 (GPIO17) supports this. On the ESP32 pins GPIO32 through
GPIO39 can be used.

|image0|

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: adc
        pin: A0
        name: "Living Room Brightness"
        update_interval: 15s

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**,
   `pin </esphomeyaml/configuration-types.html#pin>`__): The pin to
   measure the voltage on.
-  **name** (**Required**, string): The name of the voltage sensor.
-  **attenuation** (*Optional*): Only on ESP32. Specify the `ADC
   attenuation <http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html#_CPPv225adc1_config_channel_atten14adc1_channel_t11adc_atten_t>`__
   to use. One of ``0db``, ``2.5db``, ``6db``, ``11db``. Defaults to
   ``0db``.
-  **update_interval** (*Optional*,
   `time </esphomeyaml/configuration-types.html#time>`__): The interval
   to check the sensor. Defaults to ``15s``.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from
   `Sensor </esphomeyaml/components/sensor/index.html#base-sensor-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/sensor/adc.png
   :class: align-center
   :width: 80.0%
