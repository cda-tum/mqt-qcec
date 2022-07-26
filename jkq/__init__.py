#
# This file is part of the MQT QCEC library released under the MIT license.
# See README.md or go to https://github.com/cda-tum/qcec for more information.
#

import warnings

from mqt import qcec  # noqa: F401

warnings.simplefilter("always", DeprecationWarning)
warnings.warn(
    "Usage via `import jkq` is deprecated in favor of the new prefix. Please use `import mqt` instead.",
    DeprecationWarning,
)
