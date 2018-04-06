DHT Temperature+Humidity Sensor
===============================

The DHT Temperature+Humidity sensor allows you to use your DHT11
(`datasheet <https://akizukidenshi.com/download/ds/aosong/DHT11.pdf>`__,
`adafruit <https://www.adafruit.com/product/386>`__), DHT22
(`datasheet <https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf>`__,
`adafruit <https://www.adafruit.com/product/385>`__), AM2302
(`datasheet <https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf>`__,
`adafruit <https://www.adafruit.com/product/393>`__) and RHT03
(`datashet <https://cdn.sparkfun.com/datasheets/Sensors/Weather/RHT03.pdf>`__,
`sparkfun <https://cdn.sparkfun.com/datasheets/Sensors/Weather/RHT03.pdf>`__)
sensors with esphomelib.

.. figure:: /esphomeyaml/components/sensor/dht22-full.jpg
   :align: center
   :target: `Adafruit`_
   :width: 50.0%

   DHT22 Temperature & Humidity Sensor. Image by `Adafruit`_.

.. _Adafruit: https://www.adafruit.com/product/385

|image0|

.. |image0| image:: /esphomeyaml/components/sensor/temperature-humidity.png
   :class: align-center
   :width: 80.0%

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: dht
        pin: D2
        temperature:
          name: "Living Room Temperature"
        humidity:
          name: "Living Room Humidity"
        update_interval: 15s

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, `pin </esphomeyaml/configuration-types.html#pin>`__): The pin where the DHT bus is connected.
-  **temperature** (**Required**): The information for the temperature
   sensor

   -  **name** (**Required**, string): The name for the temperature
      sensor.
   -  All other options from
      `Sensor </esphomeyaml/components/sensor/index.html#base-sensor-configuration>`__
      and `MQTT
      Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

-  **humidity** (**Required**): The information for the humidity sensor

   -  **name** (**Required**, string): The name for the humidity sensor.
   -  All other options from
      `Sensor </esphomeyaml/components/sensor/index.html#base-sensor-configuration>`__
      and `MQTT
      Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

-  **model** (*Optional*, int): Manually specify the DHT model, can be
   one of ``AUTO_DETECT``, ``DHT11``, ``DHT22``, ``AM2303``, ``RHT03``
   and helps with some connection issues. Defaults to ``AUTO_DETECT``.
-  **update_interval** (*Optional*, `time </esphomeyaml/configuration-types.html#time>`__): The interval to check the
   sensor. Defaults to ``15s``.
-  **id** (*Optional*, `id </esphomeyaml/configuration-types.html#id>`__): Manually specify the ID used for code
   generation.
