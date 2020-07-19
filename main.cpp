#include <iostream>
#include "config.h"
#include "escape.h"

#define READLIST(lst, idx) ((lst & (0b11 << (2*idx))) >> (2*idx))

enum direction
    {
    up    = 0b0001,
    right = 0b0010,
    down  = 0b0100,
    left  = 0b1000
    };
enum block
        {
    // Terrain Generation
    air               = ' ',
    ground            = 177,
    deep_ground       = 178,
    
    // Initial path generation
    corridor          = 196,
    ladder_both       = 197,
    entry             = '*',
    end_left          = '[',
    end_right         = ']',
    ladder_left       = 180,
    ladder_right      = 195,
    ladder_top_left   = 191,
    ladder_top_right  = 218,
    ladder_top        = 194,
    ladder_bot        = 193,
    ladder_bot_left   = 217,
    ladder_bot_right  = 192,
    ladder            = 179,
    
    custom_section    = 219,
    
    // A.E.
    // ...
    };

int getNegativeDir (uint8_t d)
    {
    if (d > 0b0010)
        return d>>2;
    else
        return d<<2;
    }

void setblock (block map [][MAP_H],
               int x, int y, int dir_sel)
    {
    switch (dir_sel)
        {
        // Ladders
        case up | down:
            map [x][y] = block::ladder;
            break;
        case up | down | left:
            map [x][y] = block::ladder_left;
            break;
        case up | down | right:
            map [x][y] = block::ladder_right;
            break;
        case up | down | right | left:
            map [x][y] = block::ladder_both;
            break;
            // Ladder ends
        case up | left:
            map [x][y] = block::ladder_bot_left;
            break;
        case up | right:
            map [x][y] = block::ladder_bot_right;
            break;
        case up | right | left:
            map [x][y] = block::ladder_bot;
            break;
        case down | left:
            map [x][y] = block::ladder_top_left;
            break;
        case down | right:
            map [x][y] = block::ladder_top_right;
            break;
        case down | right | left:
            map [x][y] = block::ladder_top;
            break;
            // Corridor
        case right | left:
            map [x][y] = block::corridor;
            break;
        default:
            break;
        }
    }

uint8_t selectDirections (uint8_t dir)
    {
    uint8_t dir_sel = getNegativeDir (dir);
    int prob         = rand() % 100;
    int prob_prolong = rand() % 100;
    
    // Change direction switch
    switch (dir)
        {
        case right:
        case left:
            if (prob < HTV_PROB_BOTH)
                dir_sel |= (up | down); // both
            
            else if (prob - HTV_PROB_BOTH <
                     HTV_PROB_SNGL)
                {
                if(rand()%2)
                    dir_sel |= up;
                else
                    dir_sel |= down;
                }
            else
                {} // none
            break;
        case up:
        case down:
            if (prob < VTH_PROB_BOTH)
                dir_sel |= (left | right); // both
            
            else if (prob - VTH_PROB_BOTH <
                     VTH_PROB_SNGL)
                {
                if(rand()%2)
                    dir_sel |= left;
                else
                    dir_sel |= right;
                }
            else
                {} // none
            break;
        default:
            break;
        }
    // Prolong move switch
    if (dir & (up | down))
        {
        if (prob_prolong < VRT_PROB_CONT)
            dir_sel |= dir;
        }
    else
        {
        if (prob_prolong < HRT_PROB_CONT)
            dir_sel |= dir;
        }
        
    return dir_sel;
    }
    
uint8_t getRandomDirectionsList ()
    {
    uint8_t list = 0;
    uint8_t used = 0b0000;
    uint8_t tmp;
    
    while (used != 0b1111)
        {
        while (used & (1 << (tmp = rand () % 4)));
        
        used |= (1 << tmp);
    
        list |= tmp;
        if (used != 0b1111)
            list <<= 2;
        }
        
    return list;
    }
    
uint8_t getStyle (uint8_t curr_style)
    {
    if (rand()%100 < STYLE_CH_PROB)
        return curr_style+1;
    else
        return curr_style;
    }
    
void generate (block map [][MAP_H], uint8_t smap [][MAP_H],
               int x, int y, uint8_t dir, uint8_t style)
    {
    // Edge detection
    if (x == 0 || x == MAP_W-1 ||
        y == 0 || y == MAP_H-1)
        return;
    // Overwrite detection
    if (map [x][y] != 0)
        return;
    
    // Randomly selects further directions
    uint8_t dir_sel = selectDirections(dir);
    
    // Sets block according to selected directions & style
    setblock(map, x, y, dir_sel);
    smap [x][y] = style;
    
    // Exclude inverted current direction from selected ones
    dir_sel &= (~getNegativeDir(dir));
    
    // Generate new map_gens
    uint8_t dirlst = getRandomDirectionsList();
    
    for (int i = 0; i < 4; i++)
        {
        dir = (1<<READLIST(dirlst, i));
        if (dir & dir_sel)
            switch (dir)
                {
                case up:
                    generate(map, smap, x, y-1, up, getStyle(style));
                    break;
                case down:
                    generate(map, smap, x, y+1, down, getStyle(style));
                    break;
                case left:
                    generate(map, smap, x-1, y, left, getStyle(style));
                    break;
                case right:
                    generate(map, smap, x+1, y, right, getStyle(style));
                    break;
                }
        }
    
    //goto prolong_move_label;
    }

void printMap (block map [][MAP_H],
               uint8_t smap [][MAP_H])
    {
    for (int y = 0; y < MAP_H; y++)
        {
        for (int x = 0; x < MAP_W; x++)
            {
            std::cout << console::SGR (30 + smap [x][y]%8);
            
            if (map[x][y] == 0)
                std::cout << " ";
            else
                std::cout << (char) map[x][y];
            }
            
        std::cout << std::endl;
        }
    }
    
// TODO: branches merge
// TODO: cleanup
int main ()
    {
    srand(time(nullptr));
    
    block map          [MAP_W][MAP_H] = {};
    uint8_t style_map [MAP_W][MAP_H] = {};
    
    int x0 = MAP_W/2, y0 = MAP_H/2;
    int d = (1<<(rand ()%4));
    
    /*
         {-------<T>-----]
         [-------<|>-----]
                  |
         [-------<|
                  |>-----]
         }-------<B
                           */
    generate(map, style_map, x0, y0, d, 0);
    printMap(map, style_map);
    
    return 0;
    }



#ifdef ABANDONED
void setCustomBlock (block map [][MAP_H],
                     int x, int y, block b)
    {
    // Edge detection
    if (x == 0 || x == MAP_W-1 ||
        y == 0 || y == MAP_H-1)
        return;
    // Overwrite detection
    if (map [x][y] != 0)
        return;
    
    map [x][y] = b;
    }
#endif

#ifdef ABANDONED
std::ostream & operator<< (std::ostream &str, uint8_t list)
    {
    if (list & up)
        str << "Up; ";
    if (list & down)
        str << "Down; ";
    if (list & left)
        str << "Left; ";
    if (list & right)
        str << "Right; ";
    
    return str;
    }
#endif
