//
// Created by kntzn on 19/07/2020.
//

#ifndef MAPGEN_CONFIG_H
#define MAPGEN_CONFIG_H

#define MAP_H 20
#define MAP_W 100

#define HTV_PROB_SNGL 10
#define HTV_PROB_BOTH (HTV_PROB_SNGL*HTV_PROB_SNGL/100)

#define VTH_PROB_SNGL 20
#define VTH_PROB_BOTH (VTH_PROB_SNGL*VTH_PROB_SNGL/100)

#define VRT_PROB_CONT 70
#define HRT_PROB_CONT 95

#define STYLE_CH_PROB 5

/*
 * v2 definitions
 * */
#define MIN_DIST_BEFORE_NODES 2
#define MIN_NODE_DIST 4
#define MAX_NODE_DIST 8
#define FILTER_SINGLE_NODES false


// Automatic
#define DELTA_NODE_DIST (MAX_NODE_DIST - MIN_NODE_DIST)
#if MIN_DIST_BEFORE_NODES >= MAX_NODE_DIST
    #error Nodes can not be created
#endif
#if FILTER_SINGLE_NODES
    #define _FSN_ACTIVE
#endif


#endif //MAPGEN_CONFIG_H
