#ifndef PRECLASSIFICATION_SENSOR_DATA_H
#define PRECLASSIFICATION_SENSOR_DATA_H
#include "raw_sensor_data.h"

namespace muvr {

    /// Sensor location
    typedef enum  {
        wrist, chest, waist, foot, any
    } sensor_location_t;

    ///
    /// Fused exercise data
    ///
    struct fused_sensor_data {
        device_id_t device_id;
        sensor_type_t sensor_type;
        uint8_t samples_per_second;
        sensor_location_t sensor_location;
        Mat data;

        ///
        /// Convenience "ctor"
        ///
        inline static fused_sensor_data from_raw(const raw_sensor_data &raw, const sensor_location_t sensor_location) {
            return fused_sensor_data {
                    .device_id = raw.device_id(),
                    .sensor_type = raw.type(),
                    .samples_per_second = raw.samples_per_second(),
                    .sensor_location = sensor_location,
                    .data = raw.data()
                };
        }

        ///
        /// The duration
        ///
        inline sensor_duration_t duration() const {
            return static_cast<uint>(1000 * data.rows) / samples_per_second;
        }

        ///
        /// operator <<
        ///
        friend std::ostream &operator<<(std::ostream &stream, const fused_sensor_data &obj) {
            stream << "fused_sensor_data "
                   << "{ device_id=" << std::to_string(obj.device_id)
                   << ", sensor_type=" << std::to_string(obj.sensor_type)
                   << ", samples_per_second=" << std::to_string(obj.samples_per_second)
                   << ", sensor_location=" << std::to_string(obj.sensor_location)
                   << ", duration=" << obj.duration()
                   << ", samples=Mat(" << obj.data.cols << "," << obj.data.rows << ")"
                   << "}";
            return stream;
        }
    };

#ifdef NO_FUSION_STATS
    typedef void fusion_stats;
#else
    ///
    /// Holds the fusion statistics
    ///
    struct fusion_stats {
        /// true if movement automatically detected
        bool movement_detected;
        /// true if exercise automatically detected
        bool exercise_detected;

        /// true if the caller explicitly started / ended the block
        bool exercise_explicitly_set;
    };
#endif


    ///
    /// Provides ways to fuse the sensor data
    ///
    class sensor_data_fuser {
    public:

        ///
        /// The result of fusing and evaluating new block of data
        ///
        class fusion_result {
        public:
            typedef enum { not_moving, moving, exercising, exercise_ended } type_t;

            fusion_result(): m_type(not_moving) { }
            fusion_result(const fusion_result &that): m_type(that.m_type), m_exercise_fused_data(that.m_exercise_fused_data) { }

            void set_type(const type_t type) { m_type = type; }
            void set_fused_exercise_data(const std::vector<fused_sensor_data> &data) {
                m_exercise_fused_data.insert(m_exercise_fused_data.begin(), data.begin(), data.end());
                m_type = exercise_ended;
            }
            
            inline type_t type() { return m_type; }

            std::vector<fused_sensor_data> &fused_exercise_data() {
                if (m_type != exercise_ended) throw std::runtime_error("cannot get fused exercise data when not ended.");
                return m_exercise_fused_data;
            }

            friend bool operator==(const fusion_result &lhs, const type_t type) {
                return lhs.m_type == type;
            }
        private:
            type_t m_type;
            std::vector<fused_sensor_data> m_exercise_fused_data;
        };

    private:

        ///
        /// Entry in a table of raw_sensor_data, holding the location, start_time and data
        ///
        struct raw_sensor_data_entry {
        private:
            /// the sensor location
            sensor_location_t m_location;
            /// the start time of the first sample in m_data
            sensor_time_t m_wall_time;
            /// the data (padded & continuous)
            raw_sensor_data  m_data;
            /// the last-received data
            std::unique_ptr<raw_sensor_data> m_last_data;
        public:
            raw_sensor_data_entry(const raw_sensor_data_entry &that);

            ///
            /// Construct entry, assign the fields
            ///
            raw_sensor_data_entry(const sensor_location_t location, const sensor_time_t wall_time,
                                  const raw_sensor_data data);

            ///
            /// Returns ``true`` if this entry matches the location and type and samples per second
            /// in data
            ///
            bool matches(const sensor_location_t location, const raw_sensor_data &data);

            ///
            /// Appends a new block of ``data`` received at ``received_at``. If there is a gap between the
            /// end time of the last entry and ``received_at``, this method will pad the gap by attempting
            /// to reconstruct the values between the last block and the block being added.
            ///
            /// The accuracy of the reconstruction will degrade as the gap increases. At worst, the
            /// reconstructed data will form a line between the last point of the last element and the
            /// first point of the element being added.
            ///
            void push_back(const raw_sensor_data &data, const sensor_time_t wall_time);

            ///
            /// Erases data that ends before the given ``end``
            ///
            void erase_before(const sensor_time_t end);

            ///
            /// Returns the raw view of this data
            ///
            raw_sensor_data raw() const;

            ///
            /// Returns the sensor location
            ///
            inline sensor_location_t sensor_location() const { return m_location; }

            friend std::ostream &operator<<(std::ostream &stream, const raw_sensor_data_entry &obj) {
                stream << "raw_sensor_data_entry "
                       << "{ wall_time=" << obj.m_wall_time
                       << ", data=" << obj.m_data
                       << "}";

                return stream;
            }
        };

        ///
        /// "Table" holding the entries. It is OK to have an inefficient data structure here. This table
        /// will have as many entries as there are distinct locations and sensor types.
        //
        /// In rather complex scenario, the user may have _very smart_ smartwatch, giving heart rate,
        /// oxygenation, blood glucose, acceleration and rotation; mobile giving acceleration, rotation,
        /// GPS; foot sensor giving rotation and acceleration—all together 10 entries.
        ///
        /// More typically, though, we expect to have acceleration from a smartwatch together with
        /// acceleration & rotation from the mobile, giving us 3 entries.
        ///
        struct raw_sensor_data_table {
        private:
            std::vector<raw_sensor_data_entry> m_entries;
        public:
            raw_sensor_data_entry push_back(const raw_sensor_data &data, const sensor_location_t location, const sensor_time_t wall_time);
            std::vector<fused_sensor_data> slice(const sensor_time_t start, const sensor_time_t end) const;
            void clear();
            void erase_before(const sensor_time_t end);
        };

        ///
        /// Entry in the SC table that groups data from the same device & sensor
        ///
        struct sensor_context_entry {
        private:
            device_id_t m_device_id;
            sensor_type_t m_sensor_type;
            sensor_time_t m_movement_start = EXERCISE_TIME_NAN;
            sensor_time_t m_exercise_start = EXERCISE_TIME_NAN;
            exercise_decider::exercise_context m_exercise_context;
        public:
            sensor_context_entry(const device_id_t device_id, const sensor_type_t sensor_type);

            void evaluate(const raw_sensor_data &data, movement_decider *movement_decider, exercise_decider *exercise_decider);

            bool matches(const device_id_t device_id, const sensor_type_t sensor_type) const;

            inline bool has_movement() const { return m_movement_start != EXERCISE_TIME_NAN; }
            inline bool has_exercise() const { return m_movement_start != EXERCISE_TIME_NAN && m_exercise_start != EXERCISE_TIME_NAN; }
            inline sensor_time_t exercise_start() const { return m_exercise_start; }
            inline sensor_time_t movement_start() const { return m_movement_start; }
        };

        ///
        /// Context table that holds the exercise contexts across the sensors
        ///
        struct sensor_context_table {
        private:
            std::shared_ptr<movement_decider> m_movement_decider;
            std::shared_ptr<exercise_decider> m_exercise_decider;

            raw_sensor_data_table m_sensor_data_table;
            std::vector<sensor_context_entry> m_entries;

            sensor_time_t m_movement_start = EXERCISE_TIME_NAN;
            sensor_time_t m_exercise_start = EXERCISE_TIME_NAN;
        public:
            sensor_context_table(std::shared_ptr<movement_decider> movement_decider, std::shared_ptr<exercise_decider> exercise_decider);

            sensor_data_fuser::fusion_result push_back(const raw_sensor_data &new_data, const sensor_location_t location, const sensor_time_t wall_time);
        };

        sensor_context_table m_sensor_context_table;
    public:

        ///
        /// Constructs new instance of the fuser
        ///
        sensor_data_fuser();

        ///
        /// Construct new instance of the fuser
        ///
        sensor_data_fuser(std::shared_ptr<movement_decider> movement_decider, std::shared_ptr<exercise_decider> exercise_decider);

        ///
        /// Push back a block of data arriving from a given location at the specified time
        ///
        fusion_result push_back(const uint8_t *buffer, const sensor_location_t location, const sensor_time_t wall_time);

        ///
        /// Explicitly mark the start of the exercise block
        ///
        void exercise_block_start(const sensor_time_t now);

        ///
        /// Explicitly mark the end of the exercise block
        ///
        void exercise_block_end(const sensor_time_t now);
    };

}
#endif //PRECLASSIFICATION_SENSOR_DATA_H
