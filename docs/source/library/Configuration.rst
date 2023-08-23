Configuration
=============

    .. currentmodule:: mqt.qcec
    .. autoclass:: Configuration

The :class:`~mqt.qcec.Configuration` class provides all the means to configure QCEC. All of the options are split into the following categories:

.. toctree::
   :maxdepth: 2

   configuration/Execution
   configuration/Optimizations
   configuration/Application
   configuration/Functionality
   configuration/Simulation
   configuration/Parameterized

All of these options can be passed to the :meth:`~mqt.qcec.verify` and :meth:`~mqt.qcec.verify_compilation` methods as keyword arguments.

    .. currentmodule:: mqt.qcec.configuration
    .. autoclass:: ConfigurationOptions
       :members:
       :undoc-members:

There, they are incorporated into the :class:`~mqt.qcec.Configuration` using the :func:`~mqt.qcec.configuration.augment_config_from_kwargs` function.

    .. autofunction:: augment_config_from_kwargs
