ESP32 IR Transmitter Component
==============================

The IR transmitter component lets you use the `remote
peripheral <https://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/rmt.html>`__
on your ESP32 to send infrared messages to control devices in your home.
First, you need to setup a global hub that specifies which pin your IR
led is connected to. Afterwards you can create `individual
switches </esphomeyaml/components/switch/ir_transmitter.html>`__ that
each send a pre-defined IR code to a device.

.. code:: yaml

    # Example configuration entry
    ir_transmitter:
      - id: 'ir_hub1'
        pin: 32

    # Individual switches
    switch:
      - platform: ir_transmitter
        ir_transmitter_id: 'ir_hub1'
        name: "Panasonic TV Off"
        panasonic:
          address: 0x4004
          command: 0x100BCBD

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**,
   `pin </esphomeyaml/configuration-types.html#pin>`__): The pin of the
   IR LED.
-  **carrier_duty_percent** (*Optional*, int): The duty percentage of
   the carrier. 50 for example means that the LED will be on 50% of the
   time. Must be in range from 0 to 100. Defaults to 50.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation. Use this if you have multiple IR
   transmitters.
