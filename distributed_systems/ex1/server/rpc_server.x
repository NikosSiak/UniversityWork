struct args {
    int array<>;
    float a;
};

struct min_max {
    int min;
    int max;
};

struct float_array {
    float array<>;
};

program SERVER_PROG {
    version SERVER_VERS {
        float AVERAGE(args) = 1;
        min_max MIN_MAX(args) = 2;
        float_array PRODUCT(args) = 3;
    } = 1;
} = 0x23451111;

