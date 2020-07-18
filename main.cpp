#include <iostream>

#define READLIST(lst, idx) ((lst & (0b11 << (2*idx))) >> (2*idx))
#define MAP_H 20
#define MAP_W 20

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

#define HTV_PROB_SNGL 40
#define HTV_PROB_BOTH (HTV_PROB_SNGL*HTV_PROB_SNGL/100)

#define VTH_PROB_BOTH 30
#define VTH_PROB_SNGL 40

#define VRT_PROB_CONT 70
#define HRT_PROB_CONT 60

#define CSTM_SEC_PROB 5
#define CSTM_SIZE_FACTOR 5

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
    
void generate (block map [][MAP_H], uint8_t pmap [][MAP_H],
               int x, int y, uint8_t dir, uint8_t priority)
    {
    std::cout << x << ' ' << y << std::endl << std::flush;
    
    // Edge detection
    if (x == 0 || x == MAP_W-1 ||
        y == 0 || y == MAP_H-1)
        return;
    // Overwrite detection
    if (map [x][y] != 0)
        return;
    
    // Randomly selects further directions
    uint8_t dir_sel = selectDirections(dir);
    
    // Sets block according to selected directions
    setblock(map, x, y, dir_sel);
    if (priority == 0)
        map [x][y] = block::entry;
        
        // Exclude current direction from selected ones
    dir_sel &= (~dir);
    
    // Generate new map_gens
    uint8_t dirlst = getRandomDirectionsList();
    
    for (int i = 0; i < 4; i++)
        {
        dir = (1<<READLIST(dirlst, i));
        if (dir & dir_sel)
            switch (dir)
                {
                case up:
                    generate(map, pmap, x, y-1, up, priority-1);
                    break;
                case down:
                    generate(map, pmap, x, y+1, down, priority-1);
                    break;
                case left:
                    generate(map, pmap, x-1, y, left, priority-1);
                    break;
                case right:
                    generate(map, pmap, x+1, y, right, priority-1);
                    break;
                }
        }
    
    //goto prolong_move_label;
    }

void printMap (block map [][MAP_H])
    {
    for (int y = 0; y < MAP_H; y++)
        {
        for (int x = 0; x < MAP_W; x++)
            if (map [x][y] == 0)
                std::cout << " ";
            else
                std::cout << (char)map [x][y];
        
        std::cout << std::endl;
        }
    }

int main ()
    {
    srand(time(nullptr));
    
    block map          [MAP_W][MAP_H] = {};
    uint8_t priority_map [MAP_W][MAP_H] = {};
    
    int x0 = 10, y0 = MAP_H - 10;
    direction d = right;
    
    /*
         {-------<T>-----]
         [-------<|>-----]
                  |
         [-------<|
                  |>-----]
         }-------<B
                           */
    generate(map, priority_map, x0, y0, d, 0);
    printMap(map);
    
    return 0;
    }

    