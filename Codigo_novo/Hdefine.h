#define EPS (DBL_EPSILON*100)
#define EQ(x,y) (fabs((x)-(y)) <= fabs((x)+(y))*EPS)
#define LE(x,y) ((x)<(y) || EQ(x,y))
#define GE(x,y) ((y)<(x) || EQ(x,y))
#define LT(x,y) (!GE(x,y))
#define GT(x,y) (!LE(x,y))
#define MAX(x,y) ((x)<(y) ? (y) : (x))
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define SWAP(x,y) ((x)^=(y),(y)^=(x),(x)^=(y))
//------------------------------------------------------------------------------
#define disponivel -1
#define compativel  1
#define estdin 0 //1 caso estatico 0 cso dinamico

 