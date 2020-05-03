#include "rpc_server.h"

float *average_1_svc(args *argp, struct svc_req *rqstp) {
    static float result;
    int i;
    float sum = 0.0;

    for (i = 0; i < argp->array.array_len; i++) {
        sum += argp->array.array_val[i];
    }

    if (argp->array.array_len != 0) {
        result = sum / argp->array.array_len;
    }

    return &result;
}

min_max *min_max_1_svc(args *argp, struct svc_req *rqstp) {
    static min_max result;
    int i;

    result.min = argp->array.array_val[0];
    result.max = argp->array.array_val[0];

    for (i = 1; i < argp->array.array_len; i++) {
        if (argp->array.array_val[i] < result.min) {
            result.min = argp->array.array_val[i];
        }
        else if (argp->array.array_val[i] > result.max) {
            result.max = argp->array.array_val[i];
        }
    }

    return &result;
}

float_array *product_1_svc(args *argp, struct svc_req *rqstp) {
    static float_array result;
    int i;

    result.array.array_len = argp->array.array_len;
    result.array.array_val = malloc(result.array.array_len * sizeof(float));

    for (i = 0; i < argp->array.array_len; i++) {
        result.array.array_val[i] = argp->array.array_val[i] * argp->a;
    }

    return &result;
}
