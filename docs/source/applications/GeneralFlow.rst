General Equivalence Checking Flow
=================================

QCEC implements the flow proposed in :cite:p:`burgholzer2021advanced`.

.. image:: /images/verification_flow.png
   :width: 80%
   :alt: Illustration of the QCEC verification flow
   :align: center

To this end, QCEC provides two complementary approaches based on the inherent reversibility of quantum computations:

1. *formal* equivalence checking of two circuits by proving that the composition of one circuit with the inverse of the other implements the identity---a structure that can be represented very efficiently. If conducted in a clever fashion, this efficient representation can be maintained throughout the entire verification process. Eventually, \emph{alternating} between applications of gates from either circuit allows to conduct verification in an efficient fashion (under the assumption that a suitable oracle can be derived).
2. *simulation-based* equivalence checking of both computations with a couple of arbitrary input states, i.e., considering only a small part of the whole functionality, provides an attractive alternative to formal verification as described above. Due to the inherent reversibility even small differences in quantum circuits to frequently affect the entire functional representation.

Efficiently proving the equivalence of two circuits
###################################################


Efficiently proving the non-equivalence of two circuits
#######################################################
