/* Define global variables here */

#include "vpr_types.h"
#include "globals.h"
#include "netlist.h"
#include "atom_netlist.h"
#include "atom_lookup.h"
#include "tatum/TimingGraph.hpp"
#include "tatum/TimingConstraints.hpp"
#include <map>


/******** General global variables ********/
int binary_search = -1;

float grid_logic_tile_area = 0;

char *default_output_name = NULL;

/******** Atom Netlist ********/
AtomNetlist g_atom_nl;

AtomLookup g_atom_lookup;

std::unordered_map<AtomNetId,t_net_power> g_atom_net_power;

/******** Timing ********/

std::shared_ptr<tatum::TimingGraph> g_timing_graph;
std::shared_ptr<tatum::TimingConstraints> g_timing_constraints;

timing_analysis_profile_info g_timing_analysis_profile_stats;

/******** Clustered netlist ********/
int num_blocks = 0;
struct s_block *block = NULL;

t_netlist g_clbs_nlist;

/* This identifies the t_type_ptr of an IO block */
int num_types = 0;
struct s_type_descriptor *type_descriptors = NULL;

t_type_ptr IO_TYPE = NULL;
t_type_ptr EMPTY_TYPE = NULL;
t_type_ptr FILL_TYPE = NULL;

std::string g_placement_id; //SHA256 digest of .place file

/******** Physical architecture ********/

int nx = 0;
int ny = 0;

/* true if this is a global clb pin -- an input pin to which the netlist can *
 * connect global signals, but which does not connect into the normal        *
 * routing via muxes etc.  Marking pins like this (only clocks in my work)   *
 * stops them from screwing up the input switch pattern in the rr_graph      *
 * generator and from creating extra switches that the area model would      *
 * count.                                                                    */

t_chan_width chan_width;

struct s_grid_tile **grid = NULL; /* [0..(nx+1)][0..(ny+1)] Physical block list */

/******** Structures defining the routing ********/

/* Linked list start pointers.  Define the routing. */
struct s_trace **trace_head = NULL; /* [0..(num_nets-1)] */
struct s_trace **trace_tail = NULL; /* [0..(num_nets-1)] */

std::string g_routing_id; //SHA256 digest of .route file

/******** Structures defining the FPGA routing architecture ********/

int g_num_rr_nodes = 0;
t_rr_node *g_rr_nodes = NULL; /* [0..(g_num_rr_nodes-1)] */

vtr::t_ivec ***g_rr_node_indices = NULL;

t_rr_node_state* g_rr_node_state = NULL; /* [0..(g_num_rr_nodes)-1] */

int g_num_rr_indexed_data = 0;
t_rr_indexed_data *g_rr_indexed_data = NULL; /* [0..(g_num_rr_indexed_data-1)] */

/* Gives the rr_node indices of net terminals. */
int **g_net_rr_terminals = NULL; /* [0..num_nets-1][0..num_pins-1] */

/* Gives information about all the switch types                      *
 * (part of routing architecture, but loaded in read_arch.c          */

int g_num_arch_switches = 0;
s_arch_switch_inf *g_arch_switch_inf = NULL; /* [0..(g_num_arch_switches-1)] */
int g_num_rr_switches = 0;
s_rr_switch_inf *g_rr_switch_inf = NULL; /* autogenerated in build_rr_graph based on switch fan-in. [0..(g_num_rr_switches-1)] */
map<int, int> *g_switch_fanin_remap = NULL; /* an array of map. array index: [0..(g_num_arch_switches-1)]; map key: # of fanin; map value: remapped switch index */


/* Stores the SOURCE and SINK nodes of all CLBs (not valid for pads).     */

int **rr_blk_source = NULL; /* [0..(num_blocks-1)][0..(num_class-1)] */

/* primiary inputs removed from circuit */
vtr::t_linked_vptr *circuit_p_io_removed = NULL;

/********** Structures representing timing graph information */
float pb_max_internal_delay = UNDEFINED; /* biggest internal delay of physical block */
const t_pb_type *pbtype_max_internal_delay = NULL; /* physical block type with highest internal delay */

/********** Structures representing the global clock network */
t_clock_arch * g_clock_arch;
