I²C Bus
=======

This component sets up the i²c bus for your ESP32 or ESP8266. It’s used
for several other components:

=======================  =======================  =======================
|PCA9685 PWM|_           |ADS1115|_               |BMP085|_
-----------------------  -----------------------  -----------------------
`PCA9685 PWM`_           `ADS1115`_               `BMP085`_
-----------------------  -----------------------  -----------------------
|HDC1080|_               |HTU21D|_                |MPU6050|_
-----------------------  -----------------------  -----------------------
`HDC1080`_               `HTU21D`_                `MPU6050`_
=======================  =======================  =======================

.. |PCA9685 PWM| image:: /esphomeyaml/pca9685.jpg
    :class: component-image
.. _PCA9685 PWM: /esphomeyaml/components/output/pca9685.html

.. |ADS1115| image:: /esphomeyaml/ads1115.jpg
    :class: component-image
.. _ADS1115: /esphomeyaml/components/sensor/ads1115.html

.. |BMP085| image:: /esphomeyaml/bmp180.jpg
    :class: component-image
.. _BMP085: /esphomeyaml/components/sensor/bmp085.html

.. |HDC1080| image:: /esphomeyaml/hdc1080.jpg
    :class: component-image
.. _HDC1080: /esphomeyaml/components/sensor/hdc1080.html

.. |HTU21D| image:: /esphomeyaml/htu21d.jpg
    :class: component-image
.. _HTU21D: /esphomeyaml/components/sensor/htu21d.html

.. |MPU6050| image:: /esphomeyaml/mpu6050.jpg
    :class: component-image
.. _MPU6050: /esphomeyaml/components/sensor/mpu6050.html

In order for those components to work correctly, you need to define the
i²c bus in your configuration.

.. code:: yaml

    # Example configuration entry
    i2c:
      sda: 21
      scl: 22
      scan: False

Configuration variables:
~~~~~~~~~~~~~~~~~~~~~~~~

-  **sda** (*Optional*, `pin </esphomeyaml/configuration-types.html#pin>`__): The pin for the data line of the i²c bus.
   Defaults to the default of your board (usually GPIO21 for ESP32 and
   GPIO4 for ESP8266).
-  **sdc** (*Optional*, `pin </esphomeyaml/configuration-types.html#pin>`__): The pin for the clock line of the i²c bus.
   Defaults to the default of your board (usually GPIO22 for ESP32 and
   GPIO5 for ESP8266).
-  **scan** (*Optional*, boolean): If esphomelib should do a search of the i2c address space on startup.
   Note that this can slow down startup and is only recommended for when setting up new sensors. Defaults to
   ``False``.
-  **frequency** (*Optional*, float): Only on ESP32. Set the frequency
   the i²c bus should operate on. Defaults to “100kHz”. Accepts most
   metric suffixes.
