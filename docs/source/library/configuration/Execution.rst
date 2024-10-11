Execution
=========

    .. autoclass:: mqt.qcec::Configuration.Execution
        :members:
        :undoc-members:

Timeout Handling
----------------

Timeouts in QCEC work by checking an atomic flag in between the application of gates (for DD-based checkers) or rewrite rules (for the ZX-based checkers).
Unfortunately, this means that an operation needs to be fully applied before a timeout can set in.
If a certain operation during the equivalence check takes a very long time (e.g., because the DD is becoming very large), the timeout will not be triggered until that operation is finished.
Thus, it is possible that the timeout is not triggered at the expected time, and it might seem like the timeout is being ignored.

Unfortunately, there is no clean way to kill a thread without letting it finish its computation.
That's something that could be made possible by switching from multi-threading to multi-processing, but the overhead of processes versus threads is huge on certain platforms and that would not be a good trade-off.
In addition, more fine-grained abortion checks would significantly decrease the overall performance due to all the branching that would be necessary.

Consequently, timeouts in QCEC are a best-effort feature, and they should not be relied upon to always work as expected.
From experience, they tend to work reliably well for the ZX-based checkers, but they are less reliable for the DD-based checkers.
