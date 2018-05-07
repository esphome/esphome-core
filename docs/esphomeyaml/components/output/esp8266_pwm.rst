ESP8266 Software PWM Output
===========================

The ESP8266 Software PWM platform allows you to use a software PWM on
the pins GPIO0-GPIO16 on your ESP8266. As this is only a software PWM
and not a hardware PWM (like the `ESP32 LEDC
PWM </esphomeyaml/components/output/ledc.html>`__) and has a few
limitations.

-  There can be a noticeable amount of flickering with increased WiFi
   activity.
-  The output range only goes up to about 80%.
-  It’s mostly fixed to a frequency of 1kHz, you can `increase this a
   bit manually </api/output/esp8266-pwm.html#example-usage>`__ in code
   though.

If you need a stable PWM signal, it’s definitely recommended to use the
successor of the ESP8266, the ESP32, and its `LEDC
peripheral </esphomeyaml/components/output/ledc.html>`__ instead.

.. code:: yaml

    # Example configuration entry
    output:
      - platform: esp8266_pwm
        pin: D1
        id: pwm-output

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **pin** (**Required**, `Pin
   Schema </esphomeyaml/configuration-types.html#pin-schema>`__): The
   pin to use PWM on.
-  **id** (**Required**,
   `id </esphomeyaml/configuration-types.html#id>`__): The id to use for
   this output component.
-  All other options from
   `Output </esphomeyaml/components/output/index.html#base-output-configuration>`__.
