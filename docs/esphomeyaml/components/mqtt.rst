MQTT Client Component
=====================

The MQTT Client Component sets up the MQTT connection to your broker and
is currently required for esphomelib to work. In most cases, you will
just be able to copy over the `MQTT
section <https://www.home-assistant.io/components/mqtt/>`__ of your Home
Assistant configuration.

.. code:: yaml

    # Example configuration entry
    mqtt:
      broker: 10.0.0.2
      username: livingroom
      password: MyMQTTPassword

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **broker** (**Required**, string): The host of your MQTT broker.
-  **port** (*Optional*, int): The port to connect to. Defaults to 1883.
-  **username** (*Optional*, string): The username to use for
   authentication. Empty (the default) means no authentication.
-  **password** (*Optional*, string): The password to use for
   authentication. Empty (the default) means no authentication.
-  **client_id** (*Optional*, string): The client id to use for opening
   connections. See `Defaults <#defaults>`__ for more information.
-  **discovery** (*Optional*, boolean): If Home Assistant automatic
   discovery should be enabled. Defaults to ``True``.
-  **discovery_retain** (*Optional*, boolean): Whether to retain MQTT
   discovery messages so that entities are added automatically on Home
   Assistant restart. Defaults to ``True``.
-  **discovery_prefix** (*Optional*, string): The prefix to use for Home
   Assistant’s MQTT discovery. Should not contain trailing slash.
   Defaults to ``homeassistant``.
-  **topic_prefix** (*Optional*, string): The prefix used for all MQTT
   messages. Should not contain trailing slash. Defaults to
   ``<APP_NAME>``.
-  **log_topic** (*Optional*, `MQTTMessage <#mqttmessage>`__) The topic to send MQTT log
   messages to.
-  **birth_message** (*Optional*, `MQTTMessage <#mqttmessage>`__): The message to send when
   a connection to the broker is established. See `Last Will And Birth
   Messages <#last-will-and-birth-messages>`__ for more information.
-  **will_message** (*Optional*, `MQTTMessage <#mqttmessage>`__): The message to send when
   the MQTT connection is dropped. See `Last Will And Birth
   Messages <#last-will-and-birth-messages>`__ for more information.
-  **ssl_fingerprints** (*Optional*, list): Only on ESP8266. A list of SHA1 hashes used
   for verifying SSL connections. See `SSL Fingerprints <#ssl-fingerprints>`__
   for more information.
-  **keepalive** (*Optional*, `time </esphomeyaml/configuration-types.html#time>`__): The time
   to keep the MQTT socket alive, decreasing this can help with overall stability due to more
   WiFi traffic with more pings. Defaults to 15 seconds.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.

MQTTMessage
~~~~~~~~~~~

With the MQTT Message schema you can tell esphomeyaml how a specific MQTT message should be sent.
It is used in several places like last will and birth messages or MQTT log options.

.. code:: yaml

    # Simple:
    some_option: topic/to/send/to

    # Disable:
    some_option:

    # Advanced:
    some_option:
      topic: topic/to/send/to
      payload: online
      qos: 0
      retain: True


Configuration options:

-  **topic** (*Required*, string): The MQTT topic to publish the message.
-  **payload** (*Required*, string): The message content. Will be filled by the actual payload with some
   options, like log_topic.
-  **qos** (*Optional*, int): The `Quality of
   Service <https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels>`__
   level of the topic. Defaults to 0.
-  **retain** (*Optional*, boolean): If the published message should
   have a retain flag on or not. Defaults to ``True``.


Using with Home Assistant
~~~~~~~~~~~~~~~~~~~~~~~~~

Using esphomelib with Home Assistant is easy, simply setup an MQTT
broker (like `mosquitto <https://mosquitto.org/>`__) and point both your
Home Assistant installation and esphomelib to that broker. Next, enable
discovery in your Home Assistant configuration with the following:

.. code:: yaml

    # Example Home Assistant configuration.yaml entry
    mqtt:
      broker: ...
      discovery: True

And that should already be it 🎉 All devices defined through
esphomelib/esphomeyaml should show up automatically in the entities
section of Home Assistant.

When adding new entities, you might run into trouble with old entities
still appearing in Home Assistant’s front-end. This is because in order
to have Home Assistant “discover” your devices on restart, all discovery
MQTT messages need to be retained. Therefore the old entities will also
re-appear on every Home Assistant restart even though they’re in
eshomeyaml anymore.

To fix this, esphomeyaml has a simple helper script that purges stale
retained messages for you:

.. code:: bash

    esphomeyaml configuration.yaml clean-mqtt

This will remove all retained messages with the topic
``<DISCOVERY_PREFIX>/+/NODE_NAME/#``. If you want to purge on another
topic, simply add ``--topic <your_topic>`` to the command.

Defaults
~~~~~~~~

By default, esphomelib will prefix all messages with your node name or
``topic_prefix`` if you have specified it manually. The client id will
automatically be generated by using your node name and adding the MAC
address of your device to it. Next, discovery is enabled by default with
Home Assistant’s default prefix ``homeassistant``.

If you want to prefix all MQTT messages with a different prefix, like
``home/living_room``, you can specify a custom ``topic_prefix`` in the
configuration. That way, you can use your existing wildcards like
``home/+/#`` together with esphomelib. All other features of esphomelib
(like availabilty) should still work correctly.

Last Will And Birth Messages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

esphomelib (and esphomeyaml) uses the `last will
testament <https://www.hivemq.com/blog/mqtt-essentials-part-9-last-will-and-testament>`__
and birth message feature of MQTT to achieve availabilty reporting for
Home Assistant. If the node is not connected to MQTT, Home Assistant
will show all its entities as unavailable (a feature 😉).

|image0|

By default, esphomelib will send a retained MQTT message to
``<TOPIC_PREFIX>/status`` with payload ``online``, and will tell the
broker to send a message ``<TOPIC_PREFIX>/status`` with payload
``offline`` if the connection drops.

You can change these messages by overriding the ``birth_message`` and
``will_message`` with the following options.

.. code:: yaml

    mqtt:
      # ...
      birth_message:
        topic: myavailability/topic
        payload: online
      will_message:
        topic: myavailability/topic
        payload: offline

-  **birth_message** (*Optional*, `MQTTMessage <#mqttmessage>`__)
-  **will_message** (*Optional*, `MQTTMessage <#mqttmessage>`__)

If the birth message and last will message have empty topics or topics
that are different from each other, availabilty reporting will be
disabled.

SSL Fingerprints
~~~~~~~~~~~~~~~~

On the ESP8266 you have the option to use SSL connections for MQTT. This feature
will get expanded to the ESP32 once the base library, AsyncTCP, supports it. Please
note that the SSL feature only checks the SHA1 hash of the SSL certificate to verify
the integrity of the connection, so every time the certificate changes, you'll have to
update the fingerprints variable. Additionally, SHA1 is known to be partially insecure
and with some computing power the fingerprint can be faked.

To get this fingerprint, first put the broker and port options in the configuration and
then run the ``mqtt-fingerprint`` script of esphomeyaml to get the certificate:

.. code:: bash

    esphomeyaml livingroom.yaml mqtt-fingerprint
    > SHA1 Fingerprint: a502ff13999f8b398ef1834f1123650b3236fc07
    > Copy above string into mqtt.ssl_fingerprints section of livingroom.yaml

.. code:: yaml

    mqtt:
      # ...
      ssl_fingerprints:
        - a502ff13999f8b398ef1834f1123650b3236fc07

MQTT Component Base Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All components in esphomelib that do some sort of communication through
MQTT can have some overrides for specific options.

.. code:: yaml

    name: "Component Name"
    # Optional variables:
    retain: True
    discovery: True
    availabilty:
      topic: livingroom/status
      payload_available: online
      payload_not_available: offline
    state_topic: livingroom/custom_state_topic
    command_topic: livingroom/custom_command_topic

Configuration variables:

-  **name** (**Required**, string): The name to use for the MQTT
   Component.
-  **retain** (*Optional*, boolean): If all MQTT state messages should
   be retained. Defaults to ``True``.
-  **discovery** (*Optional*, boolean): Manually enable/disable
   discovery for a component. Defaults to the global default.
-  **availabilty** (*Optional*): Manually set what should be sent to
   Home Assistant for showing entity availabilty. Default derived from
   `global birth/last will message <#last-will-and-birth-messages>`__.
-  **state_topic** (*Optional*, string): The topic to publish state
   updates to. Defaults to
   ``<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/state``
   (non-alphanumeric characters from the name are removed).
-  **command_topic** (*Optional*, string): The topic to subscribe to for
   commands from the remote. Defaults to
   ``<TOPIC_PREFIX>/<COMPONENT_TYPE>/<COMPONENT_NAME>/command``
   (non-alphanumeric characters from the name are removed).

.. |image0| image:: /esphomeyaml/components/mqtt-availability.png
   :class: align-center
   :width: 50.0%
