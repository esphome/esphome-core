ESP32 Pulse Counter Sensor
==========================

The pulse counter sensor allows you to count the number of pulses on a
specific pin using the `pulse counter
peripheral <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/pcnt.html>`__
on the ESP32.

|image0|

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: pulse_counter
        pin: 12
        name: "Pulse Counter"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**,
   `pin </esphomeyaml/configuration-types.html#pin>`__): The pin to
   count pulses on.
-  **name** (**Required**, string): The name of the sensor.
-  **pull_mode** (*Optional*): The `pull
   mode <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/gpio.html#_CPPv218gpio_set_pull_mode10gpio_num_t16gpio_pull_mode_t>`__
   of the pin. One of ``PULLUP``, ``PULLDOWN``, ``PULLUP_PULLDOWN``,
   ``FLOATING``. Defaults to ``FLOATING``.
-  **count_mode** (*Optional*): Configure how the counter should behave
   on a detected rising edge/falling edge.

   -  **rising_edge** (*Optional*): What to do when a rising edge is
      detected. One of ``DISABLE``, ``INCREMENT`` and ``DECREMENT``.
      Defaults to ``INCREMENT``.
   -  **falling_edge** (*Optional*): What to do when a falling edge is
      detected. One of ``DISABLE``, ``INCREMENT`` and ``DECREMENT``.
      Defaults to ``DISABLE``.

-  **internal_filter** (*Optional*, int): If a pulse shorter than this
   number of APB clock pulses (each 12.5 ns) is detected, it’s
   discarded. See `esp-idf Filtering
   Pulses <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/pcnt.html#filtering-pulses>`__
   for more information. Defaults to the max value 1023 or about 13 µs.
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

Converting units
~~~~~~~~~~~~~~~~

The sensor defaults to measuring its values using a unit of measurement
of “pulses/min”. You can change this by using `Sensor
Filters </esphomeyaml/components/sensor/index.html#sensor-filters>`__.
For example, if you’re using the pulse counter with a photodiode to
count the light pulses on a power meter, you can do the following:

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: pulse_counter
        pin: 12
        unit_of_measurement: 'kW'
        name: 'Power Meter House'
        filters:
          - multiply: 0.06

.. |image0| image:: /esphomeyaml/components/sensor/pulse-counter.png
   :class: align-center
   :width: 80.0%
