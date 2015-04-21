#ifndef PRECLASSIFICATION_SENSOR_DATA_H
#define PRECLASSIFICATION_SENSOR_DATA_H
#include "raw_sensor_data.h"

namespace muvr {

    /// Sensor location
    typedef enum  {
        wrist, chest, waist, foot, any
    } sensor_location;

    ///
    /// Fused exercise data
    ///
    struct fused_sensor_data {
        /// the type
        sensor_type_t type;
        /// the sampling rate
        uint8_t samples_per_second;
        /// the negative time offset
        sensor_location location;
        /// the decoded data
        cv::Mat data;
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
    private:
        ///
        /// Entry in a table of raw_sensor_data, holding the location, start_time and data
        ///
        struct raw_sensor_data_entry {
        private:
            /// the sensor location
            sensor_location  m_location;
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
            raw_sensor_data_entry(const sensor_location location, const sensor_time_t wall_time,
                                  const raw_sensor_data data);

            ///
            /// Returns ``true`` if this entry matches the location and type and samples per second
            /// in data
            ///
            bool matches(const sensor_location location, const raw_sensor_data &data);

            ///
            /// Aligns the data by letting ``m_start_time`` = ``start``, and then cutting or approximating
            /// the data so that the entry remains consistent.
            ///
            raw_sensor_data_entry range(const sensor_time_t start, const sensor_time_t end) const;

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
            /// Returns the raw view of this data
            ///
            raw_sensor_data raw() const;

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
            raw_sensor_data_entry push_back(const raw_sensor_data &data, const sensor_location location, const sensor_time_t wall_time);

            ///
            /// Returns a subset of
            ///
            std::vector<fused_sensor_data> range(const sensor_time_t start, const sensor_time_t end) const;

        };

        struct sensor_context_entry {
        private:
            device_id_t m_device_id;
            sensor_type_t m_sensor_type;
            sensor_time_t m_movement_start;
            sensor_time_t m_exercise_start;
            exercise_decider::exercise_context m_exercise_context;
        public:
            sensor_context_entry(const device_id_t device_id, const sensor_type_t sensor_type);

            void evaluate(const raw_sensor_data &data, movement_decider *movement_decider, exercise_decider *exercise_decider);
            bool matches(const device_id_t device_id, const sensor_type_t sensor_type) const;
        };

        struct sensor_context_table {
        private:
            std::shared_ptr<movement_decider> m_movement_decider;
            std::shared_ptr<exercise_decider> m_exercise_decider;

            std::vector<sensor_context_entry> m_entries;
        public:
            sensor_context_table(std::shared_ptr<movement_decider> movement_decider, std::shared_ptr<exercise_decider> exercise_decider);

            void evaluate(const raw_sensor_data &data);
        };

        raw_sensor_data_table m_sensor_data_table;
        sensor_context_table m_sensor_context_table;

        void erase_ending_before(const sensor_time_t time);
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
        void push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t wall_time);

        ///
        /// Explicitly mark the start of the exercise block
        ///
        void exercise_block_start(const sensor_time_t now);

        ///
        /// Explicitly mark the end of the exercise block
        ///
        void exercise_block_end(const sensor_time_t now);

        ///
        /// Implementations will have this method called when the exercise block has ended:
        /// either automatically or explicitly. The ``data`` parameter will hold the entire
        /// exercise block; the ``fusion_stats`` parameter will have the details of the fusion
        /// steps taken.
        ///
        virtual void exercise_block_ended(const std::vector<fused_sensor_data> &data, const fusion_stats &fusion_stats) = 0;

        ///
        /// Implementations will have this method called when the exercise block has started:
        /// either automatically or explicitly. Once this method has been called, you can expect
        /// that the ``exercise_block_ended(...)`` will be called "soon".
        ///
        virtual void exercise_block_started() = 0;
    };

}
#endif //PRECLASSIFICATION_SENSOR_DATA_H
