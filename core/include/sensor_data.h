#ifndef PRECLASSIFICATION_SENSOR_DATA_H
#define PRECLASSIFICATION_SENSOR_DATA_H
#include "raw_sensor_data.h"

namespace muvr {

    /// Sensor location
    typedef enum  {
        wrist, chest, waist, foot, any
    } sensor_location;

    /// sensor time is a synthetic, but monotonously increasing time in ms
    typedef uint32_t sensor_time_t;
    /// a "NO-time" marker value. Note that we don't want to use boost::optional to reduce the
    /// number of dependencies especially for mobile clients.
    const sensor_time_t EXERCISE_TIME_NAN = UINT32_MAX;

    ///
    /// Fused exercise data
    ///
    struct fused_sensor_data {
        /// the type
        sensor_data_type type;
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
            sensor_time_t    m_start_time;
            /// the data (padded & continuous)
            raw_sensor_data  m_data;

            sensor_time_t end_time() const;
        public:
            ///
            /// Construct entry, assign the fields
            ///
            raw_sensor_data_entry(const sensor_location location, const sensor_time_t start_time,
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
            void push_back(const raw_sensor_data &data, const sensor_time_t received_at);

            ///
            /// Returns a fused view if this entry
            ///
            fused_sensor_data fused();
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
            void push_back(const raw_sensor_data &data, const sensor_location location, const sensor_time_t received_at);

            ///
            /// Returns a subset of
            ///
            std::vector<fused_sensor_data> range(const sensor_time_t start, const sensor_time_t end) const;
        };

        movement_decider m_movement_decider;
        exercise_decider m_exervise_decider;
        sensor_time_t m_exercise_start;
        raw_sensor_data_table m_table;

        void erase_ending_before(const sensor_time_t time);
    public:
        ///
        /// Constructs new instance of the fuser
        ///
        sensor_data_fuser();

        ///
        /// Construct new instance of the fuser
        ///
        sensor_data_fuser(movement_decider movement_decider, exercise_decider exercise_decider);

        ///
        /// Push back a block of data arriving from a given location at the specified time
        ///
        void push_back(const uint8_t *buffer, const sensor_location location, const sensor_time_t received_at);

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
        virtual void exercise_block_ended(const std::vector<fused_sensor_data> data, const fusion_stats &fusion_stats) = 0;

        ///
        /// Implementations will have this method called when the exercise block has started:
        /// either automatically or explicitly. Once this method has been called, you can expect
        /// that the ``exercise_block_ended(...)`` will be called "soon".
        ///
        virtual void exercise_block_started() = 0;
    };

}
#endif //PRECLASSIFICATION_SENSOR_DATA_H
