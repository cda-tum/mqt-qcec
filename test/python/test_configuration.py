import datetime
import unittest

from jkq import qcec


class QCECConfigurationTests(unittest.TestCase):
    def setUp(self):
        self.config = qcec.Configuration()

    def test_timeout(self):
        """Test setting a timeout"""
        self.config.execution.timeout = 60.
        self.config.execution.timeout = datetime.timedelta(seconds=60)

    def test_application_scheme(self):
        self.config.application.alternating_scheme = "sequential"
        self.config.application.alternating_scheme = qcec.ApplicationScheme.sequential

        self.config.application.alternating_scheme = "one_to_one"
        self.config.application.alternating_scheme = qcec.ApplicationScheme.one_to_one

        self.config.application.alternating_scheme = "lookahead"
        self.config.application.alternating_scheme = qcec.ApplicationScheme.lookahead

        self.config.application.alternating_scheme = "gate_cost"
        self.config.application.alternating_scheme = qcec.ApplicationScheme.gate_cost

        self.config.application.alternating_scheme = "proportional"
        self.config.application.alternating_scheme = qcec.ApplicationScheme.proportional

    def test_state_type(self):
        self.config.simulation.state_type = "computational_basis"
        self.config.simulation.state_type = qcec.StateType.computational_basis

        self.config.simulation.state_type = "random_1Q_basis"
        self.config.simulation.state_type = qcec.StateType.random_1Q_basis

        self.config.simulation.state_type = "stabilizer"
        self.config.simulation.state_type = qcec.StateType.stabilizer
