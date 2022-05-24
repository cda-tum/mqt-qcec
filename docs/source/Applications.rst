Quantum Circuit Equivalence Checking
====================================

QCEC can be used for checking the equivalence of arbitrary quantum circuits.
It implements the flow proposed in :cite:p:`burgholzer2021advanced`.

.. image:: /images/verification_flow.png
   :width: 80%
   :alt: Illustration of the QCEC verification flow
   :align: center

To this end, QCEC provides two complementary approaches based on the inherent reversibility of quantum computations:

1. *formal* equivalence checking of two circuits by proving that the composition of one circuit with the inverse of the other implements the identity---a structure that can be represented very efficiently. If conducted in a clever fashion, this efficient representation can be maintained throughout the entire verification process. Eventually, *alternating* between applications of gates from either circuit allows to conduct verification in an efficient fashion (under the assumption that a suitable oracle can be derived).
2. *simulation-based* equivalence checking of both computations with a couple of arbitrary input states, i.e., considering only a small part of the whole functionality, provides an attractive alternative to formal verification as described above. Due to the inherent reversibility even small differences in quantum circuits to frequently affect the entire functional representation.

Both methods complement each other in many different ways.
Trying to keep :math:`G \rightarrow \mathbb{I} \leftarrow G'` close to the identity proves very efficient in case two circuits are indeed equivalent---provided a “good” strategy can be employed.
Conducting simulations with randomly chosen stimuli on the other hand allows to quickly detect non-equivalence even in cases where both circuits only differ slightly.

The following pages outline some specific application scenarios and how to best configure QCEC to efficiently verify the respective circuits.

.. toctree::
   :maxdepth: 4

   applications/CompilationFlowVerification
   applications/RandomStimuliGeneration
   applications/DynamicCircuitVerification
