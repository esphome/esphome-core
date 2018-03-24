Power Supply
============

Example Usage
-------------

.. code-block:: cpp

    // Basic
    auto *power_supply = App.make_power_supply(12);
    // Inverted, for ATX
    auto *atx = App.make_power_supply(GPIOOutputPin(12, OUTPUT, true));

API Reference
-------------

.. cpp:namespace:: nullptr

PowerSupplyComponent
********************

.. doxygenclass:: esphomelib::PowerSupplyComponent
    :members:
    :protected-members:
    :undoc-members:



