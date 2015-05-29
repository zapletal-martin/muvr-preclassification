#include "raw_sensor_data.h"
#include "easylogging++.h"

using namespace muvr;
using namespace cv;

exercise_decider::freq_powers::freq_powers(const uint max_count, const double min_power): m_min_power(min_power), m_max_count(max_count) {
}

void exercise_decider::freq_powers::push_back(const freq_power& item) {
    if (item.power > m_min_power) {
        m_items.push_back(item);
        std::sort(m_items.begin(), m_items.end());
        if (m_items.size() > m_max_count) {
            m_items.pop_back();
        }

        m_min_power = m_items[m_items.size() - 1].power;
    }
}

double exercise_decider::freq_powers::peakiness() const {
    static const int n = 3;
    if (m_items.size() < n) return -1;

    double h = m_items[0].power - m_items[n - 1].power;
    auto mm = std::minmax_element(m_items.begin(), m_items.begin() + n, [](const freq_power &l, const freq_power &r) {
       return l.frequency < r.frequency;
    });
    double w = mm.second->frequency - mm.first->frequency;

    return h / w;
}

double exercise_decider::freq_powers::peak_frequency() const {
    if (m_items.empty()) throw std::runtime_error("peak_frequency() of empty");
    return m_items.front().frequency;
}

sensor_duration_t exercise_decider::freq_powers::peak_duration(const uint8_t sampling_rate) const {
    if (m_items.empty()) throw std::runtime_error("peak_duration() of empty");
    return 1000 / sampling_rate * m_items.front().frequency;
}

double exercise_decider::freq_powers::peak_power() const {
    if (m_items.empty()) throw std::runtime_error("peak_power() of empty");
    return m_items.front().power;
}

// ------------------------------------------------------

exercise_decider::exercise_decider(): m_min_samples(200) {

}

exercise_decider::freq_powers exercise_decider::fft(const Mat& source) const {
    assert(source.cols == 1);

    Mat filtered(source);
    filtered.convertTo(filtered, CV_32FC1);

    // take FFT of real numbers in filtered
    Mat tmp;
    dft(filtered, tmp, DFT_COMPLEX_OUTPUT);

    // the first row is just a sum, drop it
    exercise_decider::freq_powers result(10, 1e+06);
    for (int i = 1; i < tmp.rows / 2; ++i) {
        Complexf v = tmp.at<Complexf>(i, 0);
        exercise_decider::freq_power x {.frequency = (double)tmp.rows / i, .power = pow(abs(v), 2)};
        result.push_back(x);
    }

    return result;
}

namespace muvr {
    bool is_within(sensor_duration_t x, sensor_duration_t min, sensor_duration_t max) {
        return x < max && x > min;
    }
}

std::experimental::optional<std::pair<int, double>> exercise_decider::max_peak(const uint8_t samples_per_second, const std::vector<freq_powers> fps) const {
    assert(fps.size() > 0);
    const double min_peakiness = 1e+8;

    double max_peakiness = -1;
    int index = -1;
    for (int i = 0; i < fps.size(); ++i) {
        auto &fp = fps[i];
        auto duration = fp.peak_duration(samples_per_second);
        auto peakiness = fp.peakiness();
        LOG(TRACE) << "duration=" << duration << ", peakiness=" << peakiness;
        
        if (is_within(duration, 1200, 2000) && peakiness > max_peakiness) {
            max_peakiness = peakiness;
            index = i;
        }
    }
    
    if (max_peakiness > min_peakiness && index > -1) {
        return std::pair<int, double>(index, max_peakiness);
    }

    return std::experimental::nullopt;
}

exercise_decider::exercise_result exercise_decider::has_exercise(const raw_sensor_data &source, state &state) {
    if (source.reported_duration() < 4000) return undecidable;
    if (source.type() != accelerometer && source.type() != rotation) return undecidable;

    const std::vector<freq_powers> fps = { fft(source.data().col(0)), fft(source.data().col(1)), fft(source.data().col(2)) };
    const double axis_fitness_decay = 0.4;
    const double frequency_decay = 0.3;
    const double peakiness_decay = 0.3;
    const double min_fitness_threshold = 0.3;
    const double steady_state_nondecay = 0.4;

    if (state.m_axis == -1) {
        // need to determine the most prominent axis
        const auto& peak = max_peak(source.samples_per_second(), fps);
        if (peak) {
            state.m_axis = peak->first;
            state.m_freq_powers = fps;
            state.m_fitness = 1.0;
            return yes;
        }
        return no;
    } else {
        const double current_fitness = state.m_fitness;
        
        const auto& peak = max_peak(source.samples_per_second(), fps);
        if (peak) {
            if (peak->first != state.m_axis) state.m_fitness -= axis_fitness_decay;
        } else {
            state.m_fitness -= peakiness_decay;
        }
        // we already know which axis to watch
        auto &curr_fp = fps[state.m_axis];
        auto last_fp = state.m_freq_powers[state.m_axis];
        
        state.m_freq_powers = fps;

        const double frequency_epsilon = curr_fp.peak_frequency() * 0.2; // 20% drift of frequency OK
        if (std::abs(last_fp.peak_frequency() - curr_fp.peak_frequency()) > frequency_epsilon) state.m_fitness -= frequency_decay;

        LOG(TRACE) << "state.m_fitness=" << state.m_fitness;
        
        if (state.m_fitness < min_fitness_threshold) {
            // we have drifted
            state.m_freq_powers.clear();
            state.m_axis = -1;
            return no;
        }
        
        if (state.m_fitness == current_fitness) {
            state.m_fitness = MIN(1, state.m_fitness + steady_state_nondecay);
        }
        return yes;
    }
    
    return undecidable;
}
/*
bool diverged(const freq_powers &x, const freq_powers &y, const freq_powers &z) {
    const std::vector<freq_powers> fps = {x, y, z};
    if (m_axis == -1) {
        double max_peakiness = -1;
        size_t index = 0;
        for (size_t i = 0; i < fps.size(); ++i) {
            auto &fp = fps[i];
            if (fp.peakiness() > max_peakiness) {
                max_peakiness = fp.peakiness();
                index = i;
            }
        }
        
        static const double min_peakiness = 1e+6;
        if (max_peakiness > min_peakiness) {
            
        }
        
        double px = x.peakiness();
        double py = y.peakiness();
        double pz = z.peakiness();
        
        LOG(TRACE) << "px = " << px << ", py " << py << ", pz " << pz;
        
        if (px >= py >= pz && px > min_peakiness) {
            m_axis = 0;
            m_freq_powers = x;
            return false;
        } else if (py >= px >= pz && py > min_peakiness) {
            m_axis = 1;
            m_freq_powers = y;
            return false;
        } else if (pz >= px >= py && pz > min_peakiness) {
            m_axis = 2;
            m_freq_powers = z;
            return false;
        }
        
        return true;
    } else {
        const freq_powers& fp = fps[m_axis];
        
        const double epsilon = m_freq_powers->peak_frequency() / 5; // 20% drift is OK
        if (std::abs(m_freq_powers->peak_frequency() - fp.peak_frequency()) > epsilon) {
            // we have drifted
            m_freq_powers = std::experimental::nullopt;
            m_axis = -1;
            return true;
        }
        return true;
    }
}
*/
