Light
=====

.. cpp:namespace:: esphomelib::light

Lights in esphomelib are implemented like fans. Both the hardware and the MQTT frontend
access a combined :cpp:class:`LightState` object and use only that to set state and receive
state updates.

Example Usage
-------------

.. code-block:: cpp

    // Binary
    App.make_binary_light("Desk Lamp", App.make_gpio_output(15));
    // Brightness-only
    App.make_monochromatic_light("Kitchen Lights", App.make_ledc_output(16));
    // RGB, see esphomelib::output for information how to setup individual channels.
    App.make_rgb_light("RGB Lights", red, green, blue);
    App.make_rgbw_light("RGBW Lights", red, green, blue, white);

.. cpp:namespace:: esphomelib

See :cpp:func:`Application::make_binary_light`, :cpp:func:`Application::make_monochromatic_light`,
:cpp:func:`Application::make_rgb_light`, :cpp:func:`Application::make_rgbw_light`.

API Reference
-------------

.. cpp:namespace:: nullptr

LightColorValues
****************

.. doxygenclass:: esphomelib::light::LightColorValues
    :members:
    :protected-members:
    :undoc-members:

LightEffect
***********

.. doxygenclass:: esphomelib::light::LightEffect
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::light::NoneLightEffect
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::light::RandomLightEffect
    :members:
    :protected-members:
    :undoc-members:

LightOutput
***********

.. doxygenclass:: esphomelib::light::LightOutput
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::light::LinearLightOutputComponent
    :members:
    :protected-members:
    :undoc-members:

LightState
**********

.. doxygenclass:: esphomelib::light::LightState
    :members:
    :protected-members:
    :undoc-members:

LightTraits
***********

.. doxygenclass:: esphomelib::light::LightTraits
    :members:
    :protected-members:
    :undoc-members:


LightTransformer
****************

.. doxygenclass:: esphomelib::light::LightTransformer
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::light::LightTransitionTransformer
    :members:
    :protected-members:
    :undoc-members:

.. doxygenclass:: esphomelib::light::LightFlashTransformer
    :members:
    :protected-members:
    :undoc-members:

MQTTJSONLightComponent
**********************

.. doxygenclass:: esphomelib::light::MQTTJSONLightComponent
    :members:
    :protected-members:
    :undoc-members:
