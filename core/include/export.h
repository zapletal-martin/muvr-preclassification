#ifndef PRECLASSIFICATION_EXPORT_H
#define PRECLASSIFICATION_EXPORT_H

#include "sensor_data.h"

namespace muvr {
    
    ///
    /// Exports the ``data`` to the given ``stream`` in a JSON format so that
    /// it can be very easily received & interpeted by some server.
    ///
    void export_data(std::ostream& stream, const fused_sensor_data& data);

}

#endif //PRECLASSIFICATION_EXPORT_H
