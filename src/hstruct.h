#ifndef HSTRUCT_H
#define HSTRUCT_H

// Assignment record for one pile: the loader assigned to it and an assignment flag.
struct alocarga {
    // Index of the assigned loader; equals disponivel (-1) when the pile is free.
    int equip;
    // True when a loader is assigned to the pile (set together with equip), false otherwise.
    bool op;
};

#endif