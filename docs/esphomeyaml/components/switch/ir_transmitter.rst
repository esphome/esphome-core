ESP32 IR Transmitter Switch
===========================

The ``ir_transmitter`` switch platform allows you to create switches
that send an IR code using the `IR Transmitter
Component </esphomeyaml/components/ir_transmitter.html>`__. Every time
the switch is turned on, the IR signal with the provided IR code is sent
out.

Theoretically this platform can also be used to create arbitrary output
signals on any pin by using the ``raw:`` option.

|image0|

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
        repeat: 25

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **ir_transmitter_id** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id of the `IR
   Transmitter
   Component </esphomeyaml/components/ir_transmitter.html>`__.
-  **name** (**Required**, string): The name for the switch.
-  The IR code, see `Defining IR Codes <#defining-ir-codes>`__. Only one
   of them can be specified per switch.
-  **repeat** (*Optional*, int): How often the command should be
   repeated. Additionally, an ``wait_time_us`` option can be specified
   to set how long to wait in between repeats. Defaults to 1 (code is
   sent once).
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
-  All other options from
   `Switch </esphomeyaml/components/switch/index.html#base-switch-configuration>`__
   and `MQTT
   Component </esphomeyaml/components/mqtt.html#mqtt-component-base-configuration>`__.

Defining IR Codes
~~~~~~~~~~~~~~~~~

To get the IR codes, first use an Arduino or similar device with an IR
Receiver and upload the `Aruino-IRRemote IRRecvDump
sketch <https://github.com/z3t0/Arduino-IRremote/blob/master/examples/IRrecvDumpV2/IRrecvDumpV2.ino>`__.
Then press the buttons on the remote and observe the serial monitor. It
should include all the information needed to setup these IR codes.

.. code:: yaml

    nec:
      address: 0x4242
      command: 0x8484
    lg:
      data: 0x01234567890ABC
      nbits: 28
    sony:
      data: 0xABCDEF
      nbits: 12
    panasonic:
      address: 0x4004
      command: 0x1000BCD
    raw:
      carrier_frequency: 35kHz
      data:
        - 1000
        - -1000

Configuration variables:

-  **nec**: Send a NEC IR code.

   -  **address**: The address of the device.
   -  **command**: The command to send.

-  **lg**: Send an LG IR code.

   -  **data**: The data bytes to send.
   -  **nbits**: The number of bits to send, defaults to 28.

-  **sony**: Send an Sony IR code.

   -  **data**: The data bytes to send.
   -  **nbits**: The number of bits to send, defaults to 12.

-  **panasonic**: Send a Panasonic IR code.

   -  **address**: The address of the device.
   -  **command**: The command to send.

-  **raw**: Send an arbitrary signal.

   -  **carrier_frequency**: The frequency to use for the carrier. A lot
      of IR sensors only respond to a very specific frequency.
   -  **data**: List containing integers describing the signal to send.
      Each value is a time in µs declaring how long the carrier should
      be switched on or off. Positive values mean ON, negative values
      mean OFF.

.. |image0| image:: /esphomeyaml/components/switch/ir-transmitter.png
   :class: align-center
   :width: 80.0%
