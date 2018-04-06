Power Supply Component
======================

The ``power_supply`` component allows you to have a high power mode for
certain outputs. For example, if you’re using an `ATX power
supply <https://en.wikipedia.org/wiki/ATX>`__ to power your LED strips,
you usually don’t want to have the power supply on all the time while
the output is not on. The power supply component can be attached to any
`Output Component </esphomeyaml/components/output/index.html>`__ and
will automatically switch on if any of the outputs are on. Furthermore,
it also has a cooldown time that keeps the power supply on for a while
after the last ouput has been disabled.

.. code:: yaml

    # Example configuration entry
    power_supply:
      - id: 'power_supply1'
        pin: 13

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **id** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the
   power supply so that it can be used by the outputs.
-  **pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The
   GPIO pin to control the power supply on.
-  **enable_time** (*Optional*,
   `time </esphomeyaml/configuration-types.html#time>`__): The time to
   that the power supply needs for startup. The output component will
   wait for this period of time after turning on the PSU and before
   switching the output on. Defaults to ``20ms``.
-  **keep_on_time** (*Optional*,
   `time </esphomeyaml/configuration-types.html#time>`__): The time the
   power supply should be kept enabled after the last output that used
   it has been switch off. Defaults to ``10s``.

See the `output component base
configuration </esphomeyaml/components/output/index.html#base-output-configuration>`__
for information on how to apply the power supply for a specific output.

ATX Power Supplies
~~~~~~~~~~~~~~~~~~

The power supply component will default to pulling the specified GPIO
pin up when high power mode is needed. Most ATX power supplies however
operate with an active-low configuration. Therefore their output needs
to be inverted.

.. code:: yaml

    power_supply:
      - id: 'atx_power_supply'
        pin:
          number: 13
          inverted: true

Then simply connect the green control wire from the ATX power supply to
your specified pin. It’s recommended to put a small resistor (about 1kΩ)
in between to protect the ESP board.
