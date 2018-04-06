Dallas Temperature Component
============================

The ``dallas`` component allows you to use your
`DS18b20 <https://www.adafruit.com/product/374>`__
(`datasheet <https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf>`__)
and similar temperature sensors.

To use your dallas sensor, first define a dallas “hub” with a pin and
id, which you will later use to create the sensors. The 1-Wire bus the
sensors are connected to should have an external pullup resistor of
about 4.7KΩ.

.. code:: yaml

    # Example configuration entry
    dallas:
      - id: dallas_hub1
        pin: 23

    # Individual sensors
    sensor:
      - platform: dallas
        dallas_id: "dallas_hub1"
        address: 0x1c0000031edd2a28
        name: "Livingroom Temperature"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, number): The pin the sensor bus is connected
   to.
-  **update_interval** (*Optional*,
   `time </esphomeyaml/configuration-types.html#time>`__): The interval
   that the sensors should be checked. Defaults to 15 seconds.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
