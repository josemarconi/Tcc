// Macros for tolerance-based floating-point comparison and small helpers.
// EPS is the relative tolerance (DBL_EPSILON * 100). Before this header, <float.h> (DBL_EPSILON)
// and <math.h> (fabs) must be included.
#define EPS (DBL_EPSILON*100)
// EQ(x,y): x and y are equal if |x-y| <= |x+y| * EPS.
#define EQ(x,y) (fabs((x)-(y)) <= fabs((x)+(y))*EPS)
// LE(x,y): x <= y within tolerance (x < y, or EQ).
#define LE(x,y) ((x)<(y) || EQ(x,y))
// GE(x,y): x >= y within tolerance.
#define GE(x,y) ((y)<(x) || EQ(x,y))
// LT(x,y): x < y, the negation of GE.
#define LT(x,y) (!GE(x,y))
// GT(x,y): x > y, the negation of LE.
#define GT(x,y) (!LE(x,y))
// MAX(x,y): the larger of x and y (the arguments are evaluated more than once).
#define MAX(x,y) ((x)<(y) ? (y) : (x))
// MIN(x,y): the smaller of x and y (the arguments are evaluated more than once).
#define MIN(x,y) ((x)<(y) ? (x) : (y))
// SWAP(x,y): swaps two integer variables in place with XOR; it does not work for floats or for the same variable.
#define SWAP(x,y) ((x)^=(y),(y)^=(x),(x)^=(y))
//------------------------------------------------------------------------------
// disponivel (-1): marks a pile whose loader entry is free, i.e. no loader assigned (alocarga.equip).
#define disponivel -1
// compativel (1): value in compatibilidade[truck][loader] for a compatible truck/loader pair.
#define compativel  1
// estdin: multiplier of the static objective terms (wfestatico * estdin); 0 disables them.
#define estdin 0 //1 static case, 0 dynamic case

 