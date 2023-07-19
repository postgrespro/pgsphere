#include "point.h"

Datum run_tests(PG_FUNCTION_ARGS){
    check_zero_conversion_to_spoint();
    PG_RETURN_VOID();
}

void check_zero_conversion_to_spoint(){
    SPoint *res;
    Vector3D v = {0,0,0};
    res = spherepoint_from_vector3d(v);
    assert(res == NULL);
}