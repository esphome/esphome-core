Switch Component
================

The ``switch`` domain includes all platforms that should show up like a
switch and can only be turned ON or OFF.

Currently supported switch platforms:

========================  ========================  ========================
|GPIO Switch|_            |IR Transmitter|_         |Restart Switch|_
------------------------  ------------------------  ------------------------
`GPIO Switch`_            `IR Transmitter`_         `Restart Switch`_
------------------------  ------------------------  ------------------------
|Shutdown Switch|_        |Output Switch|_
------------------------  ------------------------  ------------------------
`Shutdown Switch`_        `Output Switch`_
========================  ========================  ========================

.. |GPIO Switch| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO Switch: /esphomeyaml/components/switch/gpio.html

.. |IR Transmitter| image:: /esphomeyaml/remote.svg
    :class: component-image
.. _IR Transmitter: /esphomeyaml/components/switch/ir_transmitter.html

.. |Restart Switch| image:: /esphomeyaml/restart.svg
    :class: component-image
.. _Restart Switch: /esphomeyaml/components/switch/restart.html

.. |Shutdown Switch| image:: /esphomeyaml/power.svg
    :class: component-image
.. _Shutdown Switch: /esphomeyaml/components/switch/shutdown.html

.. |Output Switch| image:: /esphomeyaml/upload.svg
    :class: component-image
.. _Output Switch: /esphomeyaml/components/switch/output.html

Base Switch Configuration
-------------------------

.. code:: yaml

    switch:
      - platform: ...
        name: "Switch Name"
        icon: "mdi:restart"

Configuration variables:

-  **name** (**Required**, string): The name of the switch.
-  **icon** (*Optional*, icon): Manually set the icon to use for the
   sensor in the frontend.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.
