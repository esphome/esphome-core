Logger Component
================

The logger component automatically logs all log messages through the
serial port and through MQTT topics. By default, all logs with a
severity higher than ``DEBUG`` will be shown. Decreasing the log level
can help with the performance of the application and memory size.

.. code:: yaml

    # Example configuration entry
    logger:
      level: DEBUG

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **baud_rate** (*Optional*, int): The baud rate to use for the serial
   UART port. Defaults to 115200.
-  **log_topic** (*Optional*, string): The MQTT topic to write log
   messages to. Defaults to ``<MQTT_TOPIC_PREFIX>/debug``. Empty string
   disables MQTT logging.
-  **tx_buffer_size** (*Optional*, string): The size of the buffer used
   for log messages. Decrease this if you’re having memory problems.
   Defaults to 512.
-  **level** (*Optional*, string): The global log level. Any log message
   with a lower severity will not be shown. Defaults to DEBUG.
-  **logs** (*Optional*, mapping): Manually set the log level for a
   specific component or tag. See `Manual Log Levels for more
   information <#manual-tag-specific-log-levels>`__.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.

Log Levels
~~~~~~~~~~

Possible log levels are (sorted by severity):

-  ``NONE``
-  ``ERROR``
-  ``WARN``
-  ``INFO``
-  ``DEBUG``
-  ``VERBOSE``

Manual Tag-Specific Log Levels
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If some component is spamming the logs and you want to manually set the
log level for it, first identify the tag of the log messages in question
and then disable them in your configuration.

Suppose we want to have verbose log messages globally, but the MQTT
client spams too much. In the following example, we’d first see that the
tag of the MQTT client is ``mqtt.client`` (before the first colon) and
the tag for MQTT components is ``mqtt.component``.

|image0|

Next, we can manually set the log levels in the configuration like this:

.. code:: yaml

    logger:
      level: VERBOSE
      logs:
        mqtt.component: DEBUG
        mqtt.client: ERROR

Please note that the global log level determines what log messages are
saved in the binary. So for example a ``INFO`` global log message will
purge all ``DEBUG`` log statements from the binary in order to conserve
space. This however means that you cannot set tag-specific log levels
that have a lower severity than the global log level.

.. |image0| image:: /esphomeyaml/components/logger-manual-log-level.png

