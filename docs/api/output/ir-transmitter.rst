IR Transmitter
==============

Example Usage
-------------

.. code-block:: cpp

    // Create the hub
    auto *ir = App.make_ir_transmitter_component(32);
    // Create switches
    auto *panasonic_on = ir->create_transmitter(SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25));
    App.make_mqtt_switch_for("Panasonic TV On", panasonic_on);
    App.make_mqtt_switch_for("Panasonic TV Volume Up", ir->create_transmitter(SendData::from_panasonic(0x4004, 0x1000405)));

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_ir_transmitter_component` and :cpp:func:`Application::make_mqtt_switch_for`.

API Reference
-------------

.. cpp:namespace:: nullptr

IRTransmitterComponent
**********************

.. doxygenclass:: esphomelib::output::IRTransmitterComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::output::next_rmt_channel

SendData
********

.. doxygennamespace:: esphomelib::output::ir
    :members:
    :protected-members:
    :undoc-members:
