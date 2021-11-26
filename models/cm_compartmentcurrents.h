/*
 *  cm_compartmentcurrents.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SYNAPSES_NEAT_H
#define SYNAPSES_NEAT_H

#include <stdlib.h>

#include "ring_buffer.h"

namespace nest
{

class Na{
private:
  // state variables sodium channel
  double m_Na_ = 0.0;
  double h_Na_ = 0.0;
  // user-defined parameters sodium channel (maximal conductance, reversal potential)
  double gbar_Na_ = 0.0;
  double e_Na_ = 0.0;

public:
  Na();
  explicit Na( const DictionaryDatum& channel_params );
  ~Na(){};

  // calibrate initialization
  void calibrate(){m_Na_ = 0.0; h_Na_ = 0.0;};
  void append_recordables(std::map< Name, double* >* recordables,
                          const long compartment_idx);

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt );
};


class K{
private:
  // state variables potassium channel
  double n_K_ = 0.0;
  // user-defined parameters potassium channel (maximal conductance, reversal potential)
  double gbar_K_ = 0.0;
  double e_K_ = 0.0;

public:
  K();
  explicit K( const DictionaryDatum& channel_params );
  ~K(){};

  // calibrate initialization
  void calibrate(){n_K_ = 0.0;};
  void append_recordables(std::map< Name, double* >* recordables,
                          const long compartment_idx);

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt );
};


class AMPA{
private:
  // global synapse index
  long syn_idx = 0;
  // user defined parameters
  double e_rev_ = 0.0; // mV
  double tau_r_ = 0.2, tau_d_ = 3.; // ms

  // assigned variables
  double g_norm_ = 1.0;

  // state variables
  double g_r_AMPA_ = 0., g_d_AMPA_ = 0.;

  // spike buffer
  RingBuffer* b_spikes_;

public:
  // constructor, destructor
  AMPA(const long syn_index);
  AMPA(const long syn_index, const DictionaryDatum& receptor_params);
  ~AMPA(){};

  // calibrateialization of the state variables
  void calibrate()
  {
    g_r_AMPA_ = 0.; g_d_AMPA_ = 0.;
    b_spikes_->clear();
  };
  void append_recordables( std::map< Name, double* >* recordables );
  void set_buffer_ptr( std::vector< RingBuffer >& syn_buffers )
  {
    b_spikes_ = &syn_buffers[ syn_idx ];
  };

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt, const long lag );
};


class GABA{
private:
  // global synapse index
  long syn_idx = 0;
  // user defined parameters
  double e_rev_ = 0.0; // mV
  double tau_r_ = 0.2, tau_d_ = 10.; // ms

  // assigned variables
  double g_norm_ = 1.0;

  // state variables
  double g_r_GABA_ = 0., g_d_GABA_ = 0.;

  // spike buffer
  RingBuffer* b_spikes_;

public:
  // constructor, destructor
  GABA(const long syn_index);
  GABA(const long syn_index, const DictionaryDatum& receptor_params);
  ~GABA(){};

  // calibrate state variables
  void calibrate()
  {
    g_r_GABA_ = 0.; g_d_GABA_ = 0.;
    b_spikes_->clear();
  };
  void append_recordables(std::map< Name, double* >* recordables);
  void set_buffer_ptr( std::vector< RingBuffer >& syn_buffers )
  {
    b_spikes_ = &syn_buffers[ syn_idx ];
  };

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt, const long lag );
};


class NMDA{
private:
  // global synapse index
  long syn_idx = 0;
  // user defined parameters
  double e_rev_ = 0.0; // mV
  double tau_r_ = 0.2, tau_d_ = 43.; // ms

  // assigned variables
  double g_norm_ = 1.0;

  // state variables
  double g_r_NMDA_ = 0., g_d_NMDA_ = 0.;

  // spike buffer
  RingBuffer* b_spikes_;

public:
  // constructor, destructor
  NMDA(const long syn_index);
  NMDA(const long syn_index, const DictionaryDatum& receptor_params);
  ~NMDA(){};

  // calibrate state variables
  void calibrate(){
    g_r_NMDA_ = 0.; g_d_NMDA_ = 0.;
    b_spikes_->clear();
  };
  void append_recordables(std::map< Name, double* >* recordables);
  void set_buffer_ptr( std::vector< RingBuffer >& syn_buffers )
  {
    b_spikes_ = &syn_buffers[ syn_idx ];
  };

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt, const long lag );

  // synapse specific funtions
  inline double NMDAsigmoid( double v_comp )
  {
    return 1. / ( 1. + 0.3 * std::exp( -.1 * v_comp ) );
  };
  inline double d_NMDAsigmoid_dv( double v_comp )
  {
    return 0.03 * std::exp( -0.1 * v_comp ) / std::pow( 0.3 * std::exp( -0.1*v_comp ) + 1.0, 2 );
  };
};


class AMPA_NMDA{
private:
  // global synapse index
  long syn_idx = 0;
  // user defined parameters
  double e_rev_ = 0.0; // mV
  double tau_r_AMPA_ = 0.2, tau_d_AMPA_ = 43.; // ms
  double tau_r_NMDA_ = 0.2, tau_d_NMDA_ = 43.; // ms
  double NMDA_ratio_ = 2.0;

  // assigned variables
  double g_norm_AMPA_ = 1.0;
  double g_norm_NMDA_ = 1.0;

  // state variables
  double g_r_AN_AMPA_ = 0., g_d_AN_AMPA_ = 0.;
  double g_r_AN_NMDA_ = 0., g_d_AN_NMDA_ = 0.;

  // spike buffer
  RingBuffer* b_spikes_;

public:
  // constructor, destructor
  AMPA_NMDA(const long syn_index);
  AMPA_NMDA(const long syn_index, const DictionaryDatum& receptor_params);
  ~AMPA_NMDA(){};

  // calibrate state variables
  void calibrate()
  {
    g_r_AN_AMPA_ = 0.; g_d_AN_AMPA_ = 0.;
    g_r_AN_NMDA_ = 0.; g_d_AN_NMDA_ = 0.;
    b_spikes_->clear();
  };
  void append_recordables(std::map< Name, double* >* recordables);
  void set_buffer_ptr( std::vector< RingBuffer >& syn_buffers )
  {
    b_spikes_ = &syn_buffers[ syn_idx ];
  };

  // numerical integration step
  std::pair< double, double > f_numstep( const double v_comp, const double dt, const long lag );

  // synapse specific funtions
  inline double NMDAsigmoid( double v_comp )
  {
    return 1. / ( 1. + 0.3 * std::exp( -.1 * v_comp ) );
  };
  inline double d_NMDAsigmoid_dv( double v_comp )
  {
    return 0.03 * std::exp( -0.1 * v_comp ) / std::pow( 0.3 * std::exp( -0.1*v_comp ) + 1.0, 2 );
  };
};


class CompartmentCurrents {
private:
  // ion channels
  Na Na_chan_;
  K K_chan_;
  // synapses
  std::vector < AMPA > AMPA_syns_;
  std::vector < GABA > GABA_syns_;
  std::vector < NMDA > NMDA_syns_;
  std::vector < AMPA_NMDA > AMPA_NMDA_syns_;

public:
  CompartmentCurrents(){};
  explicit CompartmentCurrents(const DictionaryDatum& channel_params)
  {
    Na_chan_ = Na( channel_params );
    K_chan_ = K( channel_params );
  };
  ~CompartmentCurrents(){};

  void calibrate(){
    // calibrate ion channels
    Na_chan_.calibrate();
    K_chan_.calibrate();

    // calibrate AMPA synapses
    for( auto syn_it = AMPA_syns_.begin();
         syn_it != AMPA_syns_.end();
         ++syn_it )
    {
      syn_it->calibrate();

    }
    // calibrateialization of GABA synapses
    for( auto syn_it = GABA_syns_.begin();
         syn_it != GABA_syns_.end();
         ++syn_it )
    {
      syn_it->calibrate();
    }
    // calibrateialization of NMDA synapses
    for( auto syn_it = NMDA_syns_.begin();
         syn_it != NMDA_syns_.end();
         ++syn_it )
    {
      syn_it->calibrate();
    }
    // calibrateialization of AMPA_NMDA synapses
    for( auto syn_it = AMPA_NMDA_syns_.begin();
         syn_it != AMPA_NMDA_syns_.end();
         ++syn_it )
    {
      syn_it->calibrate();
    }
  }

  void add_synapse( const std::string& type, const long syn_idx )
  {
    if ( type == "AMPA" )
    {
      AMPA syn( syn_idx );
      AMPA_syns_.push_back( syn );
    }
    else if ( type == "GABA" )
    {
      GABA syn( syn_idx );
      GABA_syns_.push_back( syn );
    }
    else if ( type == "NMDA" )
    {
      NMDA syn( syn_idx );
      NMDA_syns_.push_back( syn );
    }
    else if ( type == "AMPA_NMDA" )
    {
      AMPA_NMDA syn( syn_idx );
      AMPA_NMDA_syns_.push_back( syn );
    }
    else
    {
      assert( false );
    }
  };

  void add_synapse( const std::string& type, const long syn_idx, const DictionaryDatum& receptor_params )
  {
    if ( type == "AMPA" )
    {
      AMPA syn( syn_idx, receptor_params );
      AMPA_syns_.push_back( syn );
    }
    else if ( type == "GABA" )
    {
      GABA syn( syn_idx, receptor_params );
      GABA_syns_.push_back( syn );
    }
    else if ( type == "NMDA" )
    {
      NMDA syn( syn_idx, receptor_params );
      NMDA_syns_.push_back( syn );
    }
    else if ( type == "AMPA_NMDA" )
    {
      AMPA_NMDA syn( syn_idx, receptor_params );
      AMPA_NMDA_syns_.push_back( syn );
    }
    else
    {
      assert( false );
    }
  };

  void set_syn_buffers( std::vector< RingBuffer >& syn_buffers )
  {

    // syn_buffers for AMPA synapses
    for( auto syn_it = AMPA_syns_.begin();
         syn_it != AMPA_syns_.end();
         ++syn_it )
    {
      syn_it->set_buffer_ptr( syn_buffers );
    }
    // syn_buffers for GABA synapses
    for( auto syn_it = GABA_syns_.begin();
         syn_it != GABA_syns_.end();
         ++syn_it )
    {
      syn_it->set_buffer_ptr( syn_buffers );
    }
    // syn_buffers for NMDA synapses
    for( auto syn_it = NMDA_syns_.begin();
         syn_it != NMDA_syns_.end();
         ++syn_it )
    {
      syn_it->set_buffer_ptr( syn_buffers );
    }
    // syn_buffers for AMPA_NMDA synapses
    for( auto syn_it = AMPA_NMDA_syns_.begin();
         syn_it != AMPA_NMDA_syns_.end();
         ++syn_it )
    {
      syn_it->set_buffer_ptr( syn_buffers );
    }
  }

  std::map< Name, double* > get_recordables( const long compartment_idx )
  {

    std::map< Name, double* > recordables;

    // recordables sodium channel
    Na_chan_.append_recordables( &recordables, compartment_idx );
    // recordables potassium channel
    K_chan_.append_recordables( &recordables, compartment_idx );

    // recordables AMPA synapses
    for( auto syn_it = AMPA_syns_.begin(); syn_it != AMPA_syns_.end(); syn_it++)
      syn_it->append_recordables( &recordables );
    // recordables GABA synapses
    for( auto syn_it = GABA_syns_.begin(); syn_it != GABA_syns_.end(); syn_it++)
      syn_it->append_recordables( &recordables );
    // recordables NMDA synapses
    for( auto syn_it = NMDA_syns_.begin(); syn_it != NMDA_syns_.end(); syn_it++)
      syn_it->append_recordables( &recordables );
    // recordables AMPA_NMDA synapses
    for( auto syn_it = AMPA_NMDA_syns_.begin(); syn_it != AMPA_NMDA_syns_.end(); syn_it++)
      syn_it->append_recordables( &recordables );

    return recordables;
  };

  std::pair< double, double > f_numstep( const double v_comp, const double dt, const long lag )
  {
    std::pair< double, double > gi(0., 0.);
    double g_val = 0.;
    double i_val = 0.;

    // contribution of Na channel
    gi = Na_chan_.f_numstep( v_comp, dt );

    g_val += gi.first;
    i_val += gi.second;

    // contribution of K channel
    gi = K_chan_.f_numstep( v_comp, dt );

    g_val += gi.first;
    i_val += gi.second;

    // contribution of AMPA synapses
    for( auto syn_it = AMPA_syns_.begin();
         syn_it != AMPA_syns_.end();
         ++syn_it )
    {
      gi = syn_it->f_numstep( v_comp, dt, lag );

      g_val += gi.first;
      i_val += gi.second;
    }
    // contribution of GABA synapses
    for( auto syn_it = GABA_syns_.begin();
         syn_it != GABA_syns_.end();
         ++syn_it )
    {
      gi = syn_it->f_numstep( v_comp, dt, lag );

      g_val += gi.first;
      i_val += gi.second;
    }
    // contribution of NMDA synapses
    for( auto syn_it = NMDA_syns_.begin();
         syn_it != NMDA_syns_.end();
         ++syn_it )
    {
      gi = syn_it->f_numstep( v_comp, dt, lag );

      g_val += gi.first;
      i_val += gi.second;
    }
    // contribution of AMPA_NMDA synapses
    for( auto syn_it = AMPA_NMDA_syns_.begin();
         syn_it != AMPA_NMDA_syns_.end();
         ++syn_it )
    {
      gi = syn_it->f_numstep( v_comp, dt, lag );

      g_val += gi.first;
      i_val += gi.second;
    }

    return std::make_pair(g_val, i_val);
  };
};

} // namespace

#endif /* #ifndef SYNAPSES_NEAT_H */
