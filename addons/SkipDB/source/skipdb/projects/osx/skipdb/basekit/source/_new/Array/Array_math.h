/*
 *  Array_math.h
 */

Array *Array_plus_(Array *self, Array *other);
void Array_plusEquals_(Array *self, Array *other);

Array *Array_minus_(Array *self, Array *other);
void Array_minusEquals_(Array *self, Array *other);

Array *Array_divide_(Array *self, Array *other);
void Array_divideEquals_(Array *self, Array *other);

Array *Array_multiply_(Array *self, Array *other);
void Array_multiplyEquals_(Array *self, Array *other);

void Array_multiplyPlusEquals_(Array *self, Array *other, Array *another);
void Array_plusMultiplyEquals_(Array *self, Array *other, Array *another);

Array *Array_crossProduct_(Array *self, Array *other);

Array *Array_min_(Array *self, Array *other);
Array *Array_max_(Array *self, Array *other);

NUM_TYPE Array_mean(Array *self);
void Array_square(Array *self);
void Array_squareRoot(Array *self);
void Array_squarePow_(Array *self, NUM_TYPE n);
void Array_log(Array *self);
void Array_log10(Array *self);
void Array_random(Array *self, NUM_TYPE min, NUM_TYPE max);
void Array_setMin_(Array *self, NUM_TYPE min);
void Array_setMax_(Array *self, NUM_TYPE max);

void Array_ceil(Array *self);
void Array_floor(Array *self);
void Array_roundUp(Array *self);
void Array_roundDown(Array *self);

int Array_isZero(Array *self);
void Array_zero(Array *self);
void Array_normalize(Array *self);
void Array_absolute(Array *self);

/* --- reduction --- */

NUM_TYPE Array_sum(Array *self);
NUM_TYPE Array_dotProduct(Array *self, NUM_TYPE max);
NUM_TYPE Array_meanSquareDifference(Array *self, NUM_TYPE max);
NUM_TYPE Array_rootMeanSquareDifference(Array *self, NUM_TYPE max);

/* --- trig --- */

void Array_sin(Array *self);
void Array_asin(Array *self);
void Array_cos(Array *self);
void Array_acos(Array *self);
void Array_tan(Array *self);
void Array_atan(Array *self);
void Array_cosFill(Array *self, NUM_TYPE w, NUM_TYPE a, NUM_TYPE o);
void Array_sinFill(Array *self, NUM_TYPE w, NUM_TYPE a, NUM_TYPE o);





