WiFi Component
==============

This core esphomelib component sets up WiFi connections to access points
for you. It needs to be in your configuration or otherwise esphomeyaml
will fail in the config validation stage.

It’s recommended to provide a static IP for your node, as it can
dramatically improve connection times.

.. code:: yaml

    # Example configuration entry
    wifi:
      ssid: MyHomeNetwork
      password: VerySafePassword

      # Optional manual IP
      manual_ip:
        static_ip: 10.0.0.42
        gateway: 10.0.0.1
        subnet: 255.255.255.0

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **ssid** (**Required**, string): The name (or `service set
   identifier <https://www.lifewire.com/definition-of-service-set-identifier-816547>`__)
   of the WiFi access point your device should connect to.
-  **password** (**Required**, string): The password (or PSK) for your
   WiFi network.
-  **manual_ip** (*Optional*): Manually configure the static IP of the
   node.

   -  **static_ip** (*Required*, IPv4 address): The static IP of your
      node.
   -  **gateway** (*Required*, IPv4 address): The gateway of the local
      network.
   -  **subnet** (*Required*, IPv4 address): The subnet of the local
      network.
   -  **dns1** (*Optional*, IPv4 address): The main DNS server to use.
   -  **dns2** (*Optional*, IPv4 address): The backup DNS server to use.

-  **hostname** (*Optional*, string): Manually set the hostname of the
   node. Can only be 63 long at max and must only contain alphanumeric
   characters plus dashes and underscores.
-  **id** (*Optional*,
   `id </esphomeyaml/configuration-types.html#id>`__): Manually specify
   the ID used for code generation.
