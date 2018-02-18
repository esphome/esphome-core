Application
===========

.. cpp:namespace:: esphomelib

The :cpp:class:`Application` class has two objectives: 1. handle all of
your :cpp:class:`Component`\  s and 2. provide helper methods to simplify creating a component.

.. cpp:namespace:: esphomelib::Application

In itself, an Application instance doesn't do and store very much. It mostly only keeps track of all Components using an internal :cpp:func:`std::vector` that contains pointers, so that when the user calls the :cpp:func:`setup` or :cpp:func:`loop` methods, it can order the components based on their loop/setup priorities and runs their code in sequence. In order for this to work, of course the Application needs to know about those Components - do so by calling the :cpp:func:`register_component` / :cpp:func:`register_mqtt_component` methods.

Apart from the helpers, only one function is really necessary for setting up the app - :cpp:func:`set_name`. The name provided by this is used throughout the code base to construct various strings such as MQTT topics or WiFi hostnames. Next, the Application instance also keeps track of the inizialized WiFiComponent and MQTTClientComponent so that they can easily be accessed later.

All methods beginning with :cpp:func:`init_` are supposed to be called at most once, whereas all methods beginning with :cpp:func:`make_` can be called as often as required.

API Reference
-------------

.. cpp:namespace:: nullptr

.. doxygenclass:: esphomelib::Application
	:members:

