#ifndef _PRECLASSIFICATION_RAWSENSORDATA_H_
#define _PRECLASSIFICATION_RAWSENSORDATA_H_
#include <inttypes.h>
#include <opencv2/opencv.hpp>

using namespace cv;

namespace muvr {

    /// sensor data type
    enum sensor_data_type {
        accelerometer = 0xad,
        rotation = 0xbd,
        heart_rate = 0xed
    };

    /// sensor time is a synthetic, but monotonously increasing time in ms
    typedef int32_t sensor_time_t;
    /// a "NO-time" marker value. Note that we don't want to use boost::optional to reduce the
    /// number of dependencies especially for mobile clients.
    const sensor_time_t EXERCISE_TIME_NAN = INT32_MAX;

    ///
    /// Decoded, but still raw sensor data.
    ///
    struct raw_sensor_data {
        /// the type
        sensor_data_type type;
        /// the sampling rate
        uint8_t samples_per_second;
        /// the negative time offset
        uint8_t time_offset;
        /// the decoded data
        cv::Mat data;

        ///
        /// Corrects the ``received_at`` by taking into account the ``time_offset``,
        /// together with the number of samples and ``samples_per_second``.
        ///
        sensor_time_t received_at(const sensor_time_t received_at) const;

        ///
        /// Computes the duration in milliseconds
        ///
        sensor_time_t duration() const;

        ///
        /// Constructs the raw_sensor_data, assigns the given fields.
        ///
        raw_sensor_data(const cv::Mat &data, const sensor_data_type type, const uint8_t samples_per_second, const uint8_t time_offset);

        ///
        /// Writes the ``obj`` to the given ``stream``
        ///
        friend std::ostream &operator<<(std::ostream &stream, const raw_sensor_data &obj) {
            stream << "raw_sensor_data { "
                   << "type=" << obj.type
                   << ", time_offset=" << static_cast<int>(obj.time_offset)
                   << ", samples_per_second=" << static_cast<int>(obj.samples_per_second)
                   << ", duration=" << obj.duration()
                   << "}";
            return stream;
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

        ///
        /// Checks to see if there is no movement in the given ``source``.
        ///
        virtual movement_result has_movement(const raw_sensor_data& source) const;
    private:
        movement_result has_movement(const cv::Mat &source, const int16_t threshold) const;
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
#ifdef EYEBALL_DEBUG
                for (auto &i : m_freq_powers) std::cout << i << std::endl;
                std::cout << std::endl;
                std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
                std::cout << std::endl;
#endif
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
