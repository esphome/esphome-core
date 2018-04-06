Binary Sensor Component
=======================

With esphomelib you can use different types of binary sensors. They will
automatically appear in the Home Assistant front-end and have several
configuration options.

Currently supported binary sensor platforms:

======================  ======================  ======================
|GPIO|_                 |Status|_
----------------------  ----------------------  ----------------------
`GPIO`_                 `Status`_
======================  ======================  ======================

.. |GPIO| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO: /esphomeyaml/components/binary_sensor/gpio.html

.. |Status| image:: /esphomeyaml/server-network.svg
    :class: component-image
.. _Status: /esphomeyaml/components/binary_sensor/status.html


Base Binary Sensor Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All binary sensors have a platform and an optional device class. By
default, the binary will chose the appropriate device class itself, but
you can always override it.

.. code:: yaml

    binary_sensor:
      - platform: ...
        device_class: Device Class

Configuration variables:

-  **device_class** (*Optional*, string): The device class for the
   sensor. See https://www.home-assistant.io/components/binary_sensor/
   for a list of available options.
-  **inverted** (*Optional*, boolean): Whether to invert the binary
   sensor output, i.e. report ON states as OFF and vice versa. Defaults
   to ``False``.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.
