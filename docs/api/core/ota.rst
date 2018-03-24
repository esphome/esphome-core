Over-The-Air Updates
====================

Example Usage
-------------

.. code-block:: cpp

    // Setup basic OTA
    App.init_ota();
    // Enable safe mode.
    App.init_ota()->start_safe_mode();
    // OTA password
    auto *ota = App.init_ota();
    ota->set_auth_plaintext_password("VERY_SECURE");
    ota->start_safe_mode();
    // OTA MD5 password
    auto *ota = App.init_ota();
    ota->set_auth_password_hash("761d3a8c46989f1d357842e8dedf7712");
    ota->start_safe_mode();


API Reference
-------------

.. cpp:namespace:: nullptr

OTAComponent
************

.. doxygenclass:: esphomelib::OTAComponent
    :members:
    :protected-members:
    :undoc-members:

