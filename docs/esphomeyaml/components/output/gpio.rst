GPIO Output
===========

The GPIO output component is quite simple: It exposes a single GPIO pin
as an output component. Note that output components are **not** switches and
will not show up in Home Assistant. See `GPIO Switch </esphomeyaml/components/switch/gpio.html>`__.

.. code:: yaml

    # Example configuration entry
    output:
      - platform: gpio
        pin: D1
        id: gpio-d1

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The
   pin to use PWM on.
-  **id** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id to use for
   this output component.
-  All other options from
   `Output </esphomeyaml/components/output/index.html#base-output-configuration>`__.
