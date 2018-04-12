Getting Started with esphomeyaml
================================

esphomeyaml is the perfect solution for creating custom firmwares for
your ESP8266/ESP32 boards. In this guide we’ll go through how to setup a
basic “node” in a few simple steps.

Installation
~~~~~~~~~~~~

Installing epshomeyaml is very easy. All you need to do is have `Python
2.7 <https://www.python.org/download/releases/2.7/>`__ installed
(because of platformio 😕) and install the console script script through
``pip``.

.. code:: bash

    pip install esphomeyaml

Alternatively, there’s also a docker image available for easy
installation:

.. code:: bash

    docker pull ottowinter/esphomeyaml

Creating A Project
~~~~~~~~~~~~~~~~~~

Now let’s setup a configuration file. Fortunately, esphomeyaml has a
friendly setup wizard that will guide you through creating your first
configuration file. For example, if you want to create a configuration
file called ``livingroom.yaml``:

.. code:: bash

    esphomeyaml livingroom.yaml wizard
    # On Docker:
    docker run --rm -v "`pwd`":/config -it ottowinter/esphomeyaml livingroom.yaml wizard

At the end of this step, you will have your first YAML configuration
file ready. It doesn’t do much yet and only makes your device connect to
the WiFi network and MQTT broker, but still it’s a first step.

Adding some features
~~~~~~~~~~~~~~~~~~~~

So now you should have a file called ``livingroom.yaml`` (or similar).
Go open that file in an editor of your choice and let’s add a `simple
GPIO switch </esphomeyaml/components/switch/gpio.html>`__ to our app.

.. code:: yaml

    switch:
      - platform: gpio
        name: "Living Room Dehumidifer"
        pin: 5

The configuration format should hopefully immediately seem similar to
you. esphomeyaml has tried to keep it as close to Home Assistant’s
``configuration.yaml`` schema as possible. In above example, we’re
simply adding a switch that’s called “Living Room Relay” (could control
anything really, for example lights) and is connected to pin ``GPIO5``.
The nice thing about esphomeyaml is that it will automatically also try
to translate pin numbers for you based on the board. For example in
above configuration, if using a NodeMCU board, you could have just as
well set ``D1`` as the ``pin:`` option.

First Uploading
~~~~~~~~~~~~~~~

Now you can go ahead and add some more components. Once you feel like
you have something you want to upload to your ESP board, simply plug in
the device via USB and type the following command (replacing
``livingroom.yaml`` with your configuration file):

.. code:: bash

    esphomeyaml livingroom.yaml run

You should see esphomeyaml validating the configuration and telling you
about potential problems. Then esphomeyaml will proceed to compile and
upload the custom firmware. You will also see that esphomeyaml created a
new folder with the name of your node. This is a new platformio project
that you can modify afterwards and play around with.

On docker, the first upload is a bit more complicated, either you manage
to map the serial device into docker with the ``-v`` option, or you just
call ``compile`` within the container and let platformio do the
uploading on the host system:

.. code:: bash

    docker run --rm -v "`pwd`":/config -it ottowinter/esphomeyaml livingroom.yaml compile
    platformio run -d livingroom -t upload

Now if you have `MQTT
Discovery <https://www.home-assistant.io/docs/mqtt/discovery/>`__
enabled in your Home Assistant configuration, the switch should already
be automatically be added 🎉 (Make sure you’ve added it to a view too.)

|image0|

After the first upload, you will probably never need to use the USB
cable again, as all features of esphomelib are enabled remotely as well.
No more opening hidden boxes stowed in places hard to reach. Yay!

Adding A Binary Sensor
~~~~~~~~~~~~~~~~~~~~~~

Next, we’re going to add a very simple binary sensor that periodically
checks a GPIO pin whether it’s pulled high or low - the `GPIO Binary
Sensor </esphomeyaml/components/binary_sensor/gpio.html>`__.

.. code:: yaml

    binary_sensor:
      - platform: gpio
        name: "Living Room Window"
        pin:
          number: 16
          inverted: True
          mode: INPUT_PULLUP

This is an advanced feature of esphomeyaml, almost all pins can
optionally have a more complicated configuration schema with options for
inversion and pinMode - the `Pin
Schema </esphomeyaml/configuration-types.html#pin-schema>`__.

This time when uploading, you don’t need to have the device plugged in
through USB again. The upload will magically happen “over the air”.
Using esphomeyaml directly, this is the same as from a USB cable, but
for docker you need to supply an additional parameter:

.. code:: bash

    esphomeyaml livingroom.yaml run
    # On docker
    docker run --rm -p 6123:6123 -v "`pwd`":/config -it ottowinter/esphomeyaml livingroom.yaml run

|image1|

Where To Go Next
~~~~~~~~~~~~~~~~

Great 🎉! You’ve now successfully setup your first esphomeyaml project
and uploaded your first esphomelib custom firmware to your node. You’ve
also learned how to enable some basic components via the configuration
file.

So now is a great time to go take a look at the `Components
Index </esphomeyaml/index.html>`__, hopefully you’ll find all
sensors/outputs/… you’ll need in there. If you’re having any problems or
want new features, please either create a new issue on the `GitHub issue
tracker <https://github.com/OttoWinter/esphomeyaml/issues>`__ or contact
me via the `Discord chat <https://discord.gg/KhAMKrd>`__.

Using Custom components
~~~~~~~~~~~~~~~~~~~~~~~

esphomelib’s powerful core makes it easy to create own custom sensors.
Please first follow the `Custom Sensor Component
Guide <https://github.com/OttoWinter/esphomelib/wiki/Custom-Sensor-Component>`__
to see how this can be done. For using custom components with
esphomeyaml you only need to open up the auto-generated ``src/main.cpp``
file in the platformio project folder. The lines in between
``AUTO GENERATED CODE BEGIN`` and ``AUTO GENERATED CODE END`` should not
be edited and all changes in there will be overriden, but outside of
those comments you can safely create custom sensors while still using
esphomeyaml’s great configuration options.

.. code:: cpp

    // Auto generated code by esphomeyaml
    #include "esphomelib/application.h"

    using namespace esphomelib;

    void setup() {
      // ===== DO NOT EDIT ANYTHING BELOW THIS LINE =====
      // ========== AUTO GENERATED CODE BEGIN ===========
      App.set_name("cabinet");
      // ...
      // =========== AUTO GENERATED CODE END ============
      // ========= YOU CAN EDIT AFTER THIS LINE =========
      App.setup();
    }

    void loop() {
      App.loop();
      delay(20);
    }

.. |image0| image:: /esphomeyaml/components/switch/gpio.png
   :class: align-center
.. |image1| image:: /esphomeyaml/components/binary_sensor/gpio.png

