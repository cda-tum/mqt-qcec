# noqa: A005
from __future__ import annotations

import sys

if sys.version_info < (3, 11):
    from typing_extensions import Unpack

else:
    from typing import Unpack

__all__ = ["Unpack"]


def __dir__() -> list[str]:
    return __all__
