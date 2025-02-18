from __future__ import annotations

import sys

if sys.version_info >= (3, 11):
    from typing import Unpack
else:
    from typing_extensions import Unpack

__all__ = ["Unpack"]


def __dir__() -> list[str]:
    return __all__
