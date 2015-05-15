#include "raw_sensor_data.h"
#include "easylogging++.h"

using namespace muvr;
using namespace cv;

exercise_decider::freq_powers::freq_powers(const uint max_count): m_min_power(0), m_max_count(max_count) {
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

bool exercise_decider::freq_powers::is_distinct(double const factor) const {
    if (m_items.size() < 2) return false;
    return m_items[0].power > m_items[1].power * factor;
}

sensor_duration_t exercise_decider::freq_powers::period_duration(const uint8_t sampling_rate, const int index) const {
    return 1000 * (m_items[index].frequency / (double)sampling_rate);
}

bool exercise_decider::freq_powers::is_roughly_equal(const freq_powers& that, const uint count, const double freq_tolerance) const {
    uint max_count = (uint)min(m_items.size(), that.m_items.size());
    if (max_count < count) return false;

    for (int i = 0; i < count; ++i) {
        freq_power thisi = m_items[i];
        freq_power thati = that.m_items[i];
        
        double fr = thisi.frequency / thati.frequency;

        if (std::abs(1 - fr) > freq_tolerance) return false;
    }

    return true;
}

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
    exercise_decider::freq_powers result(10);
    for (int i = 1; i < tmp.rows / 2; ++i) {
        Complexf v = tmp.at<Complexf>(i, 0);
        exercise_decider::freq_power x {.frequency = (double)tmp.rows / i, .power = pow(abs(v), 2)};
        result.push_back(x);
    }

    return result;
}

namespace muvr {
    bool is_within(sensor_duration_t x, sensor_duration_t min, sensor_duration_t max) {
        return x < max && !(x <= min);
    }
}

exercise_decider::exercise_result exercise_decider::has_exercise(const raw_sensor_data &source, exercise_context &context) {
    if (source.reported_duration() < 4000) return undecidable;

    if (source.type() == accelerometer || source.type() == rotation) {
        auto pfx = fft(source.data().col(0));
        auto pfy = fft(source.data().col(1));
        auto pfz = fft(source.data().col(2));

        LOG(TRACE) << "pfx=" << pfx << ",pfy=" << pfy << ",pfz=" << pfz;

        // at least one of the periods has to be distinct enough
        if (!pfx.is_distinct(2) && !pfy.is_distinct(2) && !pfz.is_distinct(2)) return no;

        // the one that is distinct has to fit the times
        if (pfx.is_distinct(2)) {
            if (!is_within(pfx.period_duration(source.samples_per_second()), 800, 2000)) return no;
        }
        if (pfy.is_distinct(2)) {
            if (!is_within(pfy.period_duration(source.samples_per_second()), 800, 2000)) return no;
        }
        if (pfz.is_distinct(2)) {
            if (!is_within(pfz.period_duration(source.samples_per_second()), 800, 2000)) return no;
        }
        
        // TODO: this needs to be configurable based on exercise type. For now, we'll take anything in
        // 0.8 to 2 seconds.
        if (!is_within(pfx.period_duration(source.samples_per_second()), 800, 2000)) return no;

        if (context.diverges(pfx, pfy, pfz)) {
            context.diverged();
            LOG(TRACE) << "exercise_context " << context << " diverged";
            return no;
        }
        context.update(pfx, pfy, pfz);

        return yes;
    }

    return undecidable;
}
