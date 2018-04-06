ESP32 LEDC Output
=================

The LEDC output component exposes a `LEDC PWM
channel <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/ledc.html>`__
of the ESP32 as an output component.

.. code:: yaml

    # Example configuration entry
    output:
      - platform: ledc
        pin: 19
        id: gpio-d1

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**,
   `pin </esphomeyaml/configuration-types.html#pin>`__): The pin to use
   LEDC on. Can only be GPIO0-GPIO33.
-  **id** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id to use for
   this output component.
-  **frequency** (*Optional*, float): At which frequency to run the LEDC
   channel’s timer. Two LEDC channels always share the same timer and
   therefore also the same frequency. Defaults to 1000Hz.
-  **bit_depth** (*Optional*, int): The bit depth to use for the LEDC
   channel. Defaults to 12.
-  **channel** (*Optional*, int): Manually set the `LEDC
   channel <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/ledc.html#configure-channel>`__
   to use. Two adjacent channels share the same timer.
-  All other options from
   `Output </esphomeyaml/components/output/index.html#base-output-configuration>`__.
