Output Component
================

Each platform of the ``output`` domain exposes some output to
esphomelib. These are grouped into two categories: ``binary`` outputs
(that can only be ON/OFF) and ``float`` outputs (like PWM, can output
any rational value between 0 and 1).

========================  ========================  ========================
|ESP8266 Software PWM|_   |GPIO Output|_            |ESP32 LEDC|_
------------------------  ------------------------  ------------------------
`ESP8266 Software PWM`_   `GPIO Output`_            `ESP32 LEDC`_
------------------------  ------------------------  ------------------------
|PCA9685|_
------------------------  ------------------------  ------------------------
`PCA9685`_
========================  ========================  ========================

.. |ESP8266 Software PWM| image:: /esphomeyaml/pwm.png
    :class: component-image
.. _ESP8266 Software PWM: /esphomeyaml/components/output/esp8266_pwm.html

.. |GPIO Output| image:: /esphomeyaml/pin.svg
    :class: component-image
.. _GPIO Output: /esphomeyaml/components/output/gpio.html

.. |ESP32 LEDC| image:: /esphomeyaml/pwm.png
    :class: component-image
.. _ESP32 LEDC: /esphomeyaml/components/output/ledc.html

.. |PCA9685| image:: /esphomeyaml/pca9685.jpg
    :class: component-image
.. _PCA9685: /esphomeyaml/components/output/pca9685.html


Base Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

Each output platform extends this configuration schema.

.. code:: yaml

    # Example configuration entry
    output:
      - platform: ...
        id: myoutput_id
        power_supply: power_supply_id
        inverted: False
        max_power: 0.75

Configuration variables:

-  **id** (**Required**, `id </esphomeyaml/configuration-types.html#id>`__): The id to use for this output component.
-  **power_supply** (*Optional*, `id </esphomeyaml/configuration-types.html#id>`__): The `power
   supply </esphomeyaml/components/power_supply.html>`__ to connect to
   this output. When the output is enabled, the power supply will
   automatically be switched on too.
-  **inverted** (*Optional*, boolean): If the output should be treated
   as inverted. Defaults to ``False``.
-  **max_power** (*Optional*, float): Only for float outputs. Sets the
   maximum output value of this output platform. Each value will be
   multiplied by this. Must be in range from 0 to 1. Defaults to 1.
