/*

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#include "odininterface.h"
#include "odin_ii.h"
#include "vtr_memory.h"
#include "vtr_util.h"


OdinInterface::OdinInterface()
{
    fprintf(stderr,"Creating Odin II object\n");
    wave = -1;
    myCycle = 0;
}

/*---------------------------------------------------------------------------------------------
 * (function: startOdin)
 *-------------------------------------------------------------------------------------------*/
#define put_at_end(vector, in) vector.insert( vector.end(), in)
void OdinInterface::startOdin()
{
    /* pass arguments here to odin */
    std::vector<std::string> arguments;

    arguments.insert( arguments.end(), "--interractive_simulation" );
    if(OdinInterface::edge_output != "")
        arguments.insert( arguments.end(), OdinInterface::edge_output );
    arguments.insert( arguments.end(),{"-b",  OdinInterface::myFilename.trimmed().toLocal8Bit().data()} );
    

    /* converting into format digestible by odin */
    char **args_list = (char**)malloc(arguments.size()*sizeof(char*));
    for( int i=0; i<arguments.size(); i ++)
        args_list[i] = vtr::strdup(arguments[i].c_str());
    
    int success = odin_ii(arguments.size(),args_list);
    if(!success)
    {
        printf("Odin has ran into errors, shutting down, view logs");
        //exit(1);
    }

    for( int i=0; i<arguments.size(); i ++)
        vtr::free(args_list[i]);
    vtr::free(args_list);
}

/*---------------------------------------------------------------------------------------------
 * (function: getNodeTable)
 *-------------------------------------------------------------------------------------------*/
QHash<QString, nnode_t *> OdinInterface::getNodeTable()
{
    int i, items;
    items = 0;
    for (i = 0; i < verilog_netlist->num_top_input_nodes; i++){
        nodequeue.enqueue(verilog_netlist->top_input_nodes[i]);
        //enqueue_node_if_ready(queue,netlist->top_input_nodes[i],cycle);
    }

    // Enqueue constant nodes.
    nnode_t *constant_nodes[] = {verilog_netlist->gnd_node, verilog_netlist->vcc_node, verilog_netlist->pad_node};
    int num_constant_nodes = 3;
    for (i = 0; i < num_constant_nodes; i++){
        nodequeue.enqueue(constant_nodes[i]);
            //enqueue_node_if_ready(queue,constant_nodes[i],cycle);
    }

    // go through the netlist. While doing so
    // remove nodes from the queue and add followup nodes
    nnode_t *node;
    QHash<QString, nnode_t *> result;
    while(!nodequeue.isEmpty()){
        node = nodequeue.dequeue();
        items++;
        //remember name of the node so it is not processed again
        QString nodeName(node->name);
        //save node in hash, so it will be processed only once
        result[nodeName] = node;
        //Enqueue child nodes which are ready
        int num_children = 0;
        nnode_t **children = get_children_of(node, &num_children);


        //make sure children are not already done or in queue
        for(i=0; i< num_children; i++){
            nnode_t *nodeKid = children[i];
            QString kidName(nodeKid->name);
            //if not in queue and not yet processed

            if(!nodequeue.contains(nodeKid) && !result.contains(kidName)){
                nodequeue.enqueue(nodeKid);

            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------------------------
 * (function: setFilename)
 *-------------------------------------------------------------------------------------------*/
void OdinInterface::setFilename(QString filename)
{
    myFilename = filename;
}

/*---------------------------------------------------------------------------------------------
 * (function: setUpSimulation)
 *-------------------------------------------------------------------------------------------*/
void OdinInterface::setUpSimulation()
{
    sim_data = init_simulation(verilog_netlist);
}

/*---------------------------------------------------------------------------------------------
 * (function: simulateNextWave)
 *-------------------------------------------------------------------------------------------*/
int OdinInterface::simulateNextWave()
{
    //TODO should we increment the wave ?
    return single_step(sim_data, wave++);
}

/*---------------------------------------------------------------------------------------------
 * (function: endSimulation)
 *-------------------------------------------------------------------------------------------*/
void OdinInterface::endSimulation(){
    sim_data = terminate_simulation(sim_data);
}

/*---------------------------------------------------------------------------------------------
 * (function: getOutputValue)
 *-------------------------------------------------------------------------------------------*/
int OdinInterface::getOutputValue(nnode_t* node, int outPin, int actstep)
{
    oassert(node);
    oassert(node->num_output_pins > outPin);
    return get_pin_value(node->output_pins[outPin],actstep);
}

/*---------------------------------------------------------------------------------------------
 * (function: setEdge)
 *-------------------------------------------------------------------------------------------*/
void OdinInterface::setEdge(int i ){
    if(i==-1)
        OdinInterface::edge_output = "-E";
    else if(i==0)
        OdinInterface::edge_output = "-R";
    else
        OdinInterface::edge_output = "";
}


