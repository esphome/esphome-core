HTU21D Temperature+Humidity Sensor
==================================

The HTU21D Temperature+Humidity sensor allows you to use your HTU21D
(`datasheet <http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FHPC199_6%7FA6%7Fpdf%7FEnglish%7FENG_DS_HPC199_6_A6.pdf%7FCAT-HSC0004>`__,
`adafruit <https://www.adafruit.com/product/1899>`__) sensors with
esphomelib. The `I²C bus </esphomeyaml/components/i2c.html>`__ is
required to be set up in your configuration for this sensor to work.

.. figure:: /esphomeyaml/components/sensor/htu21d-full.jpg
   :align: center
   :target: `Adafruit`_
   :width: 50.0%

   HTU21D Temperature & Humidity Sensor. Image by `Adafruit`_.

.. _Adafruit: https://learn.adafruit.com/adafruit-htu21d-f-temperature-humidity-sensor/overview

|image0|

.. |image0| image:: /esphomeyaml/components/sensor/temperature-humidity.png
   :class: align-center
   :width: 80.0%

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: htu21d
        temperature:
          name: "Living Room Temperature"
        humidity:
          name: "Living Room Humidity"
        update_interval: 15s

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

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

-  **update_interval** (*Optional*, `time </esphomeyaml/configuration-types.html#time>`__): The interval to check the
   sensor. Defaults to ``15s``.
-  **id** (*Optional*, `id </esphomeyaml/configuration-types.html#id>`__): Manually specify the ID used for code
   generation.
