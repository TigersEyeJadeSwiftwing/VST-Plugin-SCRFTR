#include "custdata_types.h"

const int RateFC::vi[7] = {1,1,2,4,8,16,1};
const flx RateFC::vl[] = {1.0, 1.0, 2.0, 4.0, 8.0, 16.0, 1.0};
const fsx RateFC::vs[] = {1.0, 1.0, 2.0, 4.0, 8.0, 16.0, 1.0};
const fhx RateFC::vh[] = {1.0, 1.0, 2.0, 4.0, 8.0, 16.0, 1.0};

RateFC::RateFC() :
zl(flx (0.0000000001f)),
zs(fsx (0.00000000000000000001)),
zh(fhx (0.000000000000000000000000000001))
{}
