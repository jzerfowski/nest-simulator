# -*- coding: utf-8 -*-
#
# test_iaf_ps_psp_poisson_generator_accuracy.py
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.


"""
Tests for correct voltage of precise timing neuron receiving input from precise timing poisson_generator

The tests generates a poisson spike train using the poisson generator
for precise spike times. In a second step this spike train is supplied
to a neuron model and the resulting subthreshold membrane potential
fluctuations are compared to the analytical solution.  Thus, in
contrast to the more advanced test_iaf_ps_psp_poisson_accuracy, this
test does not require the interaction of the generator and the neuron
model to work and does not require the availability of a parrot
neuron.  In contrast to test_iaf_ps_psp_poisson_accuracy the DC
required to maintain a subthreshold membrane potential is generated by
a dc generator not a property of the neuron model.  The
spike_generator used to supply the neuron model with spikes,
constrains spike times to the tic grid of the simulation kernel. This
is the temporal resolution in which the computation step size and
simulation times are expressed. Therefore, the results of simulations
at different computation step sizes only differ because of limited
machine precision.  The difference between the analytical result and
the simulation, however, is dictated by the number of tics per
millisecond.

Author:  May 2005, February 2008, March 2009; Diesmann
References:
 [1] Morrison A, Straube S, Plesser H E, & Diesmann M (2007) Exact Subthreshold
     Integration with Continuous Spike Times in Discrete Time Neural Network
     Simulations. Neural Computation 19:47--79
SeeAlso: testsuite::test_iaf_ps_psp_accuracy, testsuite::test_iaf_ps_dc_accuracy
"""


import math
from math import exp

import nest
import pytest

DEBUG = False

# Global parameters
T = 6.0
tau_syn = 0.3
tau_m = 10.0
C_m = 250.0
weight = 65.0
delay = 1.0

min_exponent = -10
max_exponent = 2
poisson_rate = 16000.0


neuron_params = {
    "E_L": 0.0,
    "V_m": 0.0,
    "V_th": 1500.0,
    "I_e": 0.0,
    "tau_m": tau_m,
    "tau_syn_ex": tau_syn,
    "tau_syn_in": tau_syn,
    "C_m": C_m,
}


def V_m_response_fn(t):
    """
    Returns the value of the membrane potential at time t, assuming
    alpha-shaped post-synaptic currents and an incoming spike at t=0.
    The weight and neuron parameters are taken from outer scope.
    """
    if t < 0.0:
        return 0.0
    prefactor = weight * math.e / (tau_syn * C_m)
    term1 = (exp(-t / tau_m) - exp(-t / tau_syn)) / (1 / tau_syn - 1 / tau_m)**2
    term2 = t * exp(-t / tau_syn) / (1 / tau_syn - 1 / tau_m)
    return prefactor * (term1 - term2)


def spiketrain_response(spiketrain):
    """
    Compute the value of the membrane potential at time T
    given a spiketrain. Assumes all synaptic variables
    and membrane potential to have values 0 at time t=0.
    """
    response = 0.0
    for sp in spiketrain:
        t = T - delay - sp
        response += V_m_response_fn(t)
    return response


def create_spiketrain():
    nest.ResetKernel()
    nest.set(tics_per_ms=2**-min_exponent, resolution=1)

    pg = nest.Create("poisson_generator_ps", {"rate": poisson_rate})
    sr = nest.Create("spike_recorder")

    nest.Connect(pg, sr)
    nest.Simulate(T)
    return sr.get("events", "times")


spiketrain = create_spiketrain()


@pytest.mark.parametrize("h", range(min_exponent, max_exponent, 2))
def test_poisson_spikes_different_stepsizes(h):
    print("Spiketrain: ", spiketrain)
    nest.ResetKernel()

    nest.set(tics_per_ms=2**-min_exponent, resolution=2**h)

    sg = nest.Create("spike_generator", {"start": 0, "spike_times": spiketrain, "precise_times": True})

    neuron = nest.Create("iaf_psc_alpha_ps", params=neuron_params)
    sr = nest.Create("spike_recorder")

    if DEBUG:
        mm = nest.Create("multimeter", params={"record_from": ["V_m"], "interval": 2**h})
        nest.Connect(mm, neuron)

    nest.Connect(sg, neuron, syn_spec={"weight": weight, "delay": delay})

    nest.Simulate(T)

    reference_potential = spiketrain_response(spiketrain)
    if DEBUG:
        u = neuron.get("V_m")
        nest.Simulate(1.0)  # to get V_m recording until time T
        times = mm.get("events", "times")
        V_m = mm.get("events", "V_m")
        import matplotlib.pyplot as plt

        plt.plot(times, V_m)
        plt.scatter([T], [u], s=20.0)
        plt.scatter([T], [reference_potential], s=20, marker="X")
        plt.show()
        neuron.set(V_m=u)  # reset to value before extra 1s simulation
    assert neuron.get("V_m") == pytest.approx(reference_potential, abs=1e-12)
