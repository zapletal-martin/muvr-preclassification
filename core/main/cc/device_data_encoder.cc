#include "device_data.h"

void mk_resistance_exercise(void *memory, const char *name, const confidence_t confidence, const repetitions_t repetitions, const intensity_t intensity, const weight_t weight) {
    resistance_exercise_t *re = (resistance_exercise_t*)memory;
    strncpy(re->name, name, 24);
    re->confidence = confidence;
    re->repetitions = repetitions;
    re->intensity = intensity;
    re->weight = weight;
}
