Component
=========

.. cpp:namespace:: esphomelib::Component

Every object that should be handled by the Application instance and receive :cpp:func:`setup` and :cpp:func:`loop` calls must be a subclass of :cpp:class:`Component`.

API Reference
-------------

.. cpp:namespace:: nullptr

Component
*********

.. doxygenclass:: esphomelib::Component
    :members:
    :protected-members:
    :undoc-members:

PollingComponent
****************

.. doxygenclass:: esphomelib::PollingComponent
    :members:
    :protected-members:
    :undoc-members:


Setup Priorities
****************

.. doxygennamespace:: esphomelib::setup_priority

Assertions
**********

.. doxygendefine:: assert_is_pin
.. doxygendefine:: assert_setup
.. doxygendefine:: assert_construction_state
