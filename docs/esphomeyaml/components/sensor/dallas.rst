Dallas Temperature Sensor
=========================

The ``dallas`` sensor allows you to define sensors for you `dallas
sensor hub </esphomeyaml/components/dallas.html>`__.

To initialize a sensor, first supply the ``dallas_id`` attribute
pointing to the dallas hub. Then supply either ``address`` **or**
``index`` to identify the sensor.

.. figure:: /esphomeyaml/components/sensor/ds18b20-full.jpg
   :align: center
   :target: `Adafruit`_
   :width: 50.0%

   DS18b20 One-Wire Temperature Sensor. Image by `Adafruit`_.

.. _Adafruit: https://www.adafruit.com/product/374

|image0|

.. |image0| image:: /esphomeyaml/components/sensor/temperature.png
   :class: align-center
   :width: 80.0%

.. code:: yaml

    # Example configuration entry
    dallas:
      - id: dallas_hub1
        pin: 23

    # Individual sensors
    sensor:
      - platform: dallas
        dallas_id: "dallas_hub1"
        address: 0x1C0000031EDD2A28
        index: 0
        name: "Living Room Temperature"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **dallas_id** (**Required**, `id </esphomeyaml/configuration-types.html#id>`__): The ID of the dallas hub.
-  **address** (**Required**, int): The address of the sensor. Use either
   this option or index.
-  **index** (**Required**, int): The index of the sensor starting with 0.
   So the first sensor will for example have index 0. `It’s recommended
   to use address instead <#getting-sensor-ids>`__.
-  **resolution** (*Optional*, int): An optional resolution from 8 to
   12. Higher means more accurate. Defaults to the default for most
   sensors: 12.
-  **id** (*Optional*, `id </esphomeyaml/configuration-types.html#id>`__): Manually specify the ID used for code
   generation.
-  All other options from `Sensor </esphomeyaml/components/sensor/index.html#base-sensor-configuration>`__
   and `MQTT Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

Getting Sensor IDs
~~~~~~~~~~~~~~~~~~

It is highly recommended to use the ``address`` attribute for creating
dallas sensors, because if you have multiple sensors on a bus and the
automatic sensor discovery fails, all sensors indices will be shifted by
one. In order to get the address, simply start the firmware on your
device with a configured dallas hub and observe the log output (the `log
level </esphomeyaml/components/logger>`__ must be set to at least
``debug``!). You will find something like this:

|image1|

Next, individually warm up or cool down the sensors and observe the log
output to determine which address points to which sensor.

.. |image1| image:: /esphomeyaml/components/sensor/dallas-log.png

