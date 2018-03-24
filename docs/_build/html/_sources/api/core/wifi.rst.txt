WiFi
====

Example Usage
-------------

.. code-block:: cpp

    // Basic
    App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    // Manual IP
    auto *wifi = App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
    wifi->set_manual_ip(ManualIP{
        .static_ip = IPAddress(192, 168, 178, 42),
        .gateway = IPAddress(192, 168, 178, 1),
        .subnet = IPAddress(255, 255, 255, 0)
    });

API Reference
-------------

.. cpp:namespace:: nullptr

WiFiComponent
*************

.. doxygenclass:: esphomelib::WiFiComponent
    :members:
    :protected-members:
    :undoc-members:


