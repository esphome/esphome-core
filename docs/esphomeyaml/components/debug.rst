Debug Component
===============

The ``debug`` component can be used to debug problems with esphomelib. At startup, it prints
a bunch of useful information like reset reason, free heap size, esphomelib version and so on.

.. figure:: /esphomeyaml/components/debug.png
    :align: center

    Example debug component output.

.. code:: yaml

    # Example configuration entry
    debug:

    # Logger must be at least debug (default)
    logger:
      level: debug

There are no configuration variables for this component.
