Application
===========

.. cpp:namespace:: esphomelib

The :cpp:class:`Application` class has two objectives: 1. handle all of
your :cpp:class:`Component`\  s and 2. provide helper methods to simplify creating a component.

.. cpp:namespace:: esphomelib::Application

In itself, an Application instance doesn't do and store very much. It mostly only keeps track of all Components
(using an internal :cpp:func:`std::vector` containing pointers), so that when the user calls the :cpp:func:`setup` or
:cpp:func:`loop` methods, it can relay those calls to the components.

In order for the Application to know about your components, each one of them should be registered using the
:cpp:func:`register_component` call. And for subclasses of MQTTComponent, :cpp:func:`register_mqtt_component`
shall be used.

Apart from the helpers, only one function is really necessary for setting up the Application instance:
:cpp:func:`set_name`. The name provided by this is used throughout the code base to construct various strings
such as MQTT topics or WiFi hostnames. Additionally, some one-time-setup components such as MQTT or WiFi can be
initialized with the `init_` calls. Next, this class provides a bunch of helper methods to create and
register components easily using the `make_` calls.

Each component can tell the Application with which *priority* it wishes to be called using the `get_loop_priority()`
and `get_setup_priority()` overrides. The Application will then automatically order the components before execution.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::Application
    :members:
    :protected-members:
    :undoc-members:

.. doxygenvariable:: esphomelib::App
