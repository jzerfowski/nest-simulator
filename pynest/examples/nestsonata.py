# -*- coding: utf-8 -*-
#
# nestsonata.py
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


import h5py   # TODO this need to be a try except thing
import json
import pandas as pd
import numpy as np
import warnings
import nest
import nest.raster_plot
import csv
import time

import matplotlib.pyplot as plt

nest.ResetKernel()

#example = '300_pointneurons'
example = 'GLIF'
plot = False

if example == '300_pointneurons':
    base_path = '/home/stine/Work/sonata/examples/300_pointneurons/'
    config = 'circuit_config.json'
    sim_config = 'simulation_config.json'
    plot = True
elif example == 'GLIF':
    base_path = '/home/stine/Work/sonata/examples/GLIF_NEST/'
    config = 'config_small.json'
    sim_config = None


sonata_connector = nest.SonataConnector(base_path, config, sim_config)

if not sonata_connector.config['target_simulator'] == 'NEST':
    raise NotImplementedError('Only `target_simulator` of type NEST is supported.')

nest.set(resolution=sonata_connector.config['run']['dt'])#, tics_per_ms=sonata_connector.config['run']['nsteps_block'])

# Create nodes
sonata_connector.create_nodes()
sonata_connector.create_edge_dict()

sonata_dynamics = {'nodes': sonata_connector.node_collections, 'edges': sonata_connector.edge_types}
print(sonata_connector.node_collections)

print()
#print('sonata_dynamics', sonata_dynamics)
print()

start_time = time.time()

nest.Connect(sonata_dynamics=sonata_dynamics)

end_time = time.time() - start_time

conns = nest.GetConnections()
print(conns)
print("")
print("number of connections: ", nest.GetKernelStatus('num_connections'))
#print("num_connections with alpha: ", len(conns.alpha))

#node_id_to_range = 0
#for nc in sonata_connector.node_collections['internal']:
#    old_val = node_id_to_range
#    node_id_to_range += len(nest.GetConnections(source=nc))
#    print(f'Range of connections with source node id {nc.global_id}: {old_val} {node_id_to_range}')

print(f"\nconnection took: {end_time} s")

if plot:
    s_rec = nest.Create('spike_recorder')
    nest.Connect(sonata_connector.node_collections['internal'], s_rec)

print('simulating')

simtime = 0
if 'tstop' in sonata_connector.config['run']:
    simtime = sonata_connector.config['run']['tstop']
else:
    simtime = sonata_connector.config['run']['duration']
nest.Simulate(simtime)

if plot:
    print(s_rec.events)
    nest.raster_plot.from_device(s_rec)
    plt.show()











