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
    if (m_items.size() < 4) return 0;

    double h = m_items[0].power - m_items[3].power;
    auto mm = std::minmax_element(m_items.begin(), m_items.begin() + 4, [](const freq_power &l, const freq_power &r) {
       return l.frequency < r.frequency;
    });
    double w = mm.second->frequency - mm.first->frequency;

    return h / w;
}

double exercise_decider::freq_powers::peak_frequency() const {
    if (m_items.empty()) throw std::runtime_error("Peak frequency of empty");
    return m_items.front().frequency;
}

double exercise_decider::freq_powers::peak_power() const {
    if (m_items.empty()) throw std::runtime_error("Peak power of empty");
    return m_items.front().power;
}

// ------------------------------------------------------

exercise_decider::exercise_decider(): m_min_samples(200) {

}

exercise_decider::freq_powers exercise_decider::fft(const Mat& source) const {
    assert(source.cols == 1);

    // smooth out noise
    Mat filtered;
    blur(source, filtered, Size(50, 50));
    filtered.convertTo(filtered, CV_32FC1);

    // take FFT of real numbers in filtered
    Mat tmp;
    dft(filtered, tmp, DFT_COMPLEX_OUTPUT);

    // the first row is just a sum, drop it
    exercise_decider::freq_powers result(10, 1e+08);
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

exercise_decider::exercise_result exercise_decider::has_exercise(const raw_sensor_data &source, state &context) {
    if (source.reported_duration() < 4000) return undecidable;

    if (source.type() == accelerometer || source.type() == rotation) {
        auto pfx = fft(source.data().col(0));
        auto pfy = fft(source.data().col(1));
        auto pfz = fft(source.data().col(2));
        
        LOG(TRACE) << "pfx=" << pfx << ",pfy=" << pfy << ",pfz=" << pfz;

        if (context.diverged(pfx, pfy, pfz)) {
            return no;
        }

        return yes;
    }

    return undecidable;
}
