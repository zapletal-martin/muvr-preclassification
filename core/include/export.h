#ifndef PRECLASSIFICATION_EXPORT_H
#define PRECLASSIFICATION_EXPORT_H

#include "sensor_data.h"

namespace muvr {

    void export_data(std::ostream& stream, const fused_sensor_data& data);

}

#endif //PRECLASSIFICATION_EXPORT_H
