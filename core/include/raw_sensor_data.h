#ifndef _PRECLASSIFICATION_RAWSENSORDATA_H_
#define _PRECLASSIFICATION_RAWSENSORDATA_H_
#include <inttypes.h>
#include <opencv2/opencv.hpp>

using namespace cv;

namespace muvr {

    /// sensor data type
    enum sensor_type_t {
        accelerometer = 0xad,
        rotation = 0xbd,
        heart_rate = 0xed
    };

    ///
    /// The device id
    ///
    typedef uint8_t device_id_t;

    ///
    /// The known device ids
    ///
    enum device_ids {
        pebble = 0x00,
        iphone_like = 0xf0
    };

    /// sensor time is a synthetic, but monotonously increasing time in ms
    typedef uint64_t sensor_time_t;
    /// duration of sensor data
    typedef uint64_t sensor_duration_t;
    /// a "NO-time" marker value. Note that we don't want to use boost::optional to reduce the
    /// number of dependencies especially for mobile clients.
    const sensor_time_t EXERCISE_TIME_NAN = INT64_MAX;

    ///
    /// Decoded, but still raw sensor data.
    ///
    struct raw_sensor_data {
    private:
        /// the type
        sensor_type_t m_type;
        /// the sampling rate
        uint8_t m_samples_per_second;
        /// the sensor data timestamp
        sensor_time_t m_end_timestamp;
        /// the duration
        sensor_duration_t m_reported_duration;
        /// the device id sending the data
        device_id_t m_device_id;
        /// the decoded data
        cv::Mat m_data;

        /// converts the time to the number of samples
        inline uint time_to_samples(const sensor_time_t time) const {
            assert(time < 60000 * 1000); // more than an hour is almost certainly programming error

            return static_cast<uint>(time / (1000 / m_samples_per_second));
        }
    public:

        ///
        /// Returns the device-reported duration
        ///
        inline sensor_duration_t reported_duration() const { return m_reported_duration; }

        ///
        /// Returns the type
        ///
        inline sensor_type_t type() const { return m_type; }

        ///
        /// Returns the data
        ///
        inline const Mat &data() const { return m_data; }

        ///
        /// Returns the samples per second
        ///
        inline uint8_t samples_per_second() const { return m_samples_per_second; }

        ///
        /// Returns the end timestamp
        ///
        inline sensor_time_t end_timestamp() const { return m_end_timestamp; }

        ///
        /// Returns the start timestamp
        ///
        inline sensor_time_t start_timestamp() const { return m_end_timestamp - m_reported_duration; }

        ///
        /// Returns the device id
        ///
        inline device_id_t device_id() const { return m_device_id; }

        ///
        /// Evaluates if this instance "is compatible with" ``that``. Compatibility means sampling rate within
        /// some small tolerance, and same sensor data.
        ///
        bool matches(const raw_sensor_data &that) const;

        ///
        ///
        ///
        void push_back(const raw_sensor_data &that, const sensor_time_t gap_length = 0);

        ///
        /// Slices the data in this instance so that they fall between start and end
        ///
        raw_sensor_data slice(const sensor_time_t start, const sensor_time_t end) const;

        ///
        /// Slices the data from end
        ///
        raw_sensor_data slice_from_end(const sensor_duration_t duration) const;

        ///
        /// Constructs the raw_sensor_data, assigns the given fields.
        ///
        raw_sensor_data(
                const cv::Mat &data,
                const device_id_t device_id,
                const sensor_type_t type,
                const uint8_t samples_per_second,
                const sensor_time_t timestamp,
                const sensor_duration_t duration);

        raw_sensor_data(const raw_sensor_data &that);

        ///
        /// Writes the ``obj`` to the given ``stream``
        ///
        friend std::ostream &operator<<(std::ostream &stream, const raw_sensor_data &obj) {
            stream << "raw_sensor_data "
                   << "{ device_id=" << std::to_string(obj.m_device_id)
                   << ", type=" << obj.m_type
                   << ", timestamp=" << static_cast<sensor_time_t>(obj.m_end_timestamp)
                   << ", samples_per_second=" << static_cast<int>(obj.m_samples_per_second)
                   << ", duration=" << obj.m_reported_duration
                   << "}";
            return stream;
        }

        friend bool operator==(const raw_sensor_data &lhs, const raw_sensor_data &rhs) {
            bool simple =
                    lhs.m_data.cols == rhs.m_data.cols &&
                    lhs.m_data.rows == rhs.m_data.rows &&
                    lhs.m_type == rhs.m_type &&
                    lhs.m_end_timestamp == rhs.m_end_timestamp &&
                    lhs.m_reported_duration == rhs.m_reported_duration &&
                    lhs.m_samples_per_second == rhs.m_samples_per_second;
            if (simple) {
                for (int i = 0; i < lhs.m_data.cols; ++i) {
                    auto s = sum(lhs.m_data.col(i) - rhs.m_data.col(i))[0];
                    if (s > 0) return false;
                }
                return true;
            }

            return false;
        }
    };

    ///
    /// Applies the kalman filter to even out / smooth the values the
    /// matrix ``source``, which must contain only a single column.
    ///
    cv::Mat kalman_smooth(const cv::Mat &source);

    ///
    /// Implementations decide whether there is movement or not in the given source
    ///
    class movement_decider {
    public:

        /// result of the evaluation is either yes, no or "file not found" :)
        enum movement_result {
            no, yes, undecidable
        };

        /// ctor
        movement_decider();

        ///
        /// Checks to see if there is no movement in the given ``source``.
        ///
        virtual movement_result has_movement(const raw_sensor_data& source) const;
    private:
        /// the decision function
        movement_result has_movement(const cv::Mat &source, const int16_t threshold) const;
    protected:
        /// thresholds for the different devices
        std::map<device_id_t, int16_t> m_thresholds;
    };

    ///
    /// Implementations decide whether there is exercise or not in the given source.
    ///
    /// The call to ``no_exercise`` will typically only run iff ``movement_decider::no_movement()``
    /// returns ``false``.
    ///
    class exercise_decider {
    private:
        uint m_min_samples;

        ///
        /// "tuple" for the frequency and power
        ///
        struct freq_power {
            /// the signal frequency (in 1/sample)
            double frequency;
            /// the signal power
            double power;

            /// ordering by power
            bool operator < (const freq_power& that) const {
                return (power > that.power);
            }

        public:
            friend std::ostream& operator<<(std::ostream &stream, const freq_power &obj) {
                stream << "frequency = " << obj.frequency << ", power = " << obj.power;
                return stream;
            }
        };

        ///
        /// container for the top ``n`` most powerful signals
        ///
        struct freq_powers {
        private:
            uint m_max_count;
            std::vector<freq_power> m_items;
            double m_min_power;
        public:
            ///
            /// Ctor, sets the maximum number items with the greatest power to record
            ///
            freq_powers(const uint max_count);

            ///
            /// Check for the item's power, if acceptable, add
            ///
            void push_back(const freq_power& item);

            ///
            /// Computes whether there is enough distiction between the powers of the first and other
            /// frequencies by at least ``factor``.
            ///
            bool is_distinct(const double factor = 100) const;

            ///
            /// Computes whether this freq_powers roughly matches the frequencies in ``that``.
            ///
            bool is_roughly_equal(const freq_powers& that, const uint count = 2, const double freq_tolerance = 0.2) const;

            friend std::ostream &operator<<(std::ostream &stream, const freq_powers &obj) {
                for (auto &x : obj.m_items) stream << x << std::endl;
                stream << std::endl;
                return stream;
            }
        };

        /// compute the periodogram of the real numbers in the rows of first column in ``source``
        exercise_decider::freq_powers fft(const Mat& source) const;

    public:
        ///
        /// The opaque structure that the clients hold
        ///
        struct exercise_context {
        private:
            std::vector<freq_powers> m_freq_powers;
        public:
            /// tests whether this instance diverges from the x, y, z
            bool diverges(const freq_powers &x, const freq_powers &y, const freq_powers &z) const {
                if (m_freq_powers.size() == 0) return false;

                if (!m_freq_powers[0].is_roughly_equal(x)) return true;
                if (!m_freq_powers[1].is_roughly_equal(y)) return true;
                return !m_freq_powers[2].is_roughly_equal(z);
            };

            /// update this instance after divergence
            void diverged() {
                m_freq_powers.erase(m_freq_powers.begin(), m_freq_powers.end());
            }

            void update(const freq_powers &x, const freq_powers &y, const freq_powers &z) {
                m_freq_powers.erase(m_freq_powers.begin(), m_freq_powers.end());
                m_freq_powers.push_back(x);
                m_freq_powers.push_back(y);
                m_freq_powers.push_back(z);
            }
        };

        /// result of the evaluation is either yes, no or "file not found" :)
        enum exercise_result {
            no, yes, undecidable
        };

        ///
        /// Constructs an instance of exercise_decider
        ///
        exercise_decider();

        ///
        /// Checks to see if there is movement that is typical for exercise in the
        /// given ``source``.
        ///
        virtual exercise_result has_exercise(const raw_sensor_data& source, exercise_context &context) const;
    };


}


#endif //_PRECLASSIFICATION_RAWSENSORDATA_H_
