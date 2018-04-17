IR Transmitter
==============

Example Usage
-------------

.. code-block:: cpp

    // at the top of your file:
    using namespace esphomelib::switch_::ir_;

    // Create the hub
    auto *ir = App.make_ir_transmitter(32);
    // Create switches
    auto *panasonic_on = ir->create_transmitter("Panasonic TV On", SendData::from_panasonic(0x4004, 0x100BCBD).repeat(25));
    App.register_switch(panasonic_on);
    App.register_switch(ir->create_transmitter("Panasonic TV Volume Up", SendData::from_panasonic(0x4004, 0x1000405)));

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_ir_transmitter` and :cpp:func:`Application::register_switch`.

API Reference
-------------

.. cpp:namespace:: nullptr

IRTransmitterComponent
**********************

.. doxygenclass:: esphomelib::switch_::IRTransmitterComponent
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::switch_::next_rmt_channel

SendData
********

.. doxygennamespace:: esphomelib::switch_::ir
    :members:
    :protected-members:
    :undoc-members:
