Fan Component
=============

With the ``fan`` domain you can create components that appear as fans in
the Home Assistant frontend. A fan can be switched ON or OFF, optionally
has a speed setting (``LOW``, ``MEDIUM``, ``HIGH``) and can have an
oscillate output.

|image0|

Currently supported fan platforms:

========================  ========================  ========================
|Binary Fan|_             |Speed Fan|_
------------------------  ------------------------  ------------------------
`Binary Fan`_             `Speed Fan`_
========================  ========================  ========================

.. |Binary Fan| image:: /esphomeyaml/fan.svg
    :class: component-image
.. _Binary Fan: /esphomeyaml/components/fan/binary.html

.. |Speed Fan| image:: /esphomeyaml/fan.svg
    :class: component-image
.. _Speed Fan: /esphomeyaml/components/fan/speed.html

Base Fan Configuration
~~~~~~~~~~~~~~~~~~~~~~

.. code:: yaml

    fan:
      - platform: ...
        name: ...

Configuration variables:

-  **name** (**Required**, string): The name of the fan.
-  **oscillation_state_topic** (*Optional*, string): The topic to
   publish fan oscillaiton state changes to.
-  **oscillation_command_topic** (*Optional*, string): The topic to
   receive oscillation commands on.
-  **speed_state_topic** (*Optional*, string): The topic to publish fan
   speed state changes to.
-  **speed_command_topic** (*Optional*, string): The topic to receive
   speedcommands on.
-  All other options from `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

.. |image0| image:: /esphomeyaml/components/fan/fan.png
   :class: align-center
   :width: 70.0%
