Ultrasonic Distance Sensor
==========================

The ultrasonic distance sensor allows you to use simple ultrasonic
sensors like the HC-SR04
(`datasheet <https://www.electroschematics.com/wp-content/uploads/2013/07/HC-SR04-datasheet-version-2.pdf>`__,
`sparkfun <https://www.sparkfun.com/products/13959>`__) with esphomelib
to measure distances. These sensors usually can’t measure anything more
than about two meters and may sometimes make some annoying clicking
sounds.

This sensor platform expects a sensor that can be sent a **trigger
pulse** on a specific pin and will send out a **echo pulse** once a
measurement has been taken. Because sometimes (for example if no object
is detected) the echo pulse is never returned, this sensor also has a
timeout option which specifies how long to wait for values. During this
timeout period the whole core will be blocked and therefore shouldn’t be
set too high.

.. figure:: /esphomeyaml/components/sensor/hc-sr04-full.jpg
   :align: center
   :target: `www.dx.com`_
   :width: 50.0%

   HC-SR04 Ultrasonic Distance Sensor. Image by `www.dx.com`_.

.. _www.dx.com: http://www.dx.com/p/new-hc-sr04-ultrasonic-sensor-distance-measuring-module-3-3v-5v-compatible-for-arduino-416860

|image0|

.. |image0| image:: /esphomeyaml/components/sensor/ultrasonic.png
   :class: align-center
   :width: 80.0%

.. code:: yaml

    # Example configuration entry
    sensor:
      - platform: ultrasonic
        trigger_pin: D1
        echo_pin: D2
        name: "Ultrasonic Sensor"

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **trigger_pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The output pin to
   periodically send the trigger pulse to.
-  **echo_pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The input pin on which to
   wait for the echo.
-  **name** (**Required**, string): The name of the sensor.
-  **timeout_meter** (*Optional*, float): The number of meters for the
   timeout. Use either this or ``timeout_time``. Defaults to 2 meters.
-  **timeout_time** (*Optional*, int): The number of microseconds for
   the timeout. Use either this or ``timeout_meter``. Defaults to
   11662µs.
-  **update_interval** (*Optional*, `time </esphomeyaml/configuration-types.html#time>`__): The interval to check the
   sensor. Defaults to ``15s``.
-  **id** (*Optional*, `id </esphomeyaml/configuration-types.html#id>`__): Manually specify the ID used for code
   generation.
-  All other options from
   `Sensor </esphomeyaml/components/sensor/index.html#base-sensor-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.
