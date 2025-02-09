#include <cmath>
#include <vector>
#include <gtest/gtest.h>

class accelerometer {
public:
    struct data {
        float x;
        float y;
        float z;
    };

    enum class sampling_rate {
        c_20_hz,
        c_50_hz,
        c_100_hz,
    };

    enum error {
        ok,
        not_supported
    };

    virtual error set_sampling_rate(sampling_rate) = 0;
    virtual data get_data() = 0;
};

class tap_detection_algo {
public:
    tap_detection_algo(accelerometer &accel)
        : accel_(accel), first_sample_(true) {}

    bool run() {
        auto current = accel_.get_data();

        if (first_sample_) {
            prev_ = current;
            first_sample_ = false;
            return false;
        }

        bool tap = (std::fabs(current.x - prev_.x) > c_threshold) ||
                   (std::fabs(current.y - prev_.y) > c_threshold) ||
                   (std::fabs(current.z - prev_.z) > c_threshold);
        prev_ = current;
        return tap;
    }

private:
    static constexpr float c_threshold = 0.5f; 
    accelerometer &accel_;
    accelerometer::data prev_;
    bool first_sample_ = true;
};

class fake_accel : public accelerometer {
public:
    fake_accel(const std::vector<data>& samples)
        : samples_(samples), index_(0) {}

    error set_sampling_rate(sampling_rate) override {
        return error::ok;
    }

    data get_data() override {
        if (index_ < samples_.size()) {
            return samples_[index_++];
        }
        return samples_.back();
    }

private:
    std::vector<data> samples_;
    size_t index_;
};

// Test 1: No tap is detected if there is no significant change between samples.
TEST(TapDetectionAlgoTest, NoTapWhenNoSignificantChange) {
    std::vector<accelerometer::data> samples = {
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };
    fake_accel fakeAccel(samples);
    tap_detection_algo algo(fakeAccel);

    // The first call initializes the previous reading.
    EXPECT_FALSE(algo.run());
    // The subsequent calls show no significant change.
    EXPECT_FALSE(algo.run());
    EXPECT_FALSE(algo.run());
}

// Test 2: A tap is detected when there is a sudden change in the readings.
TEST(TapDetectionAlgoTest, DetectTapOnSuddenChange) {
    std::vector<accelerometer::data> samples = {
        {0.0f, 1.0f, 0.0f},  // initial reading
        {0.0f, 1.0f, 0.0f},  // no change -> false
        {0.0f, 2.0f, 0.0f}   // significant change on y axis (delta = 1.0 > 0.5) -> true
    };
    fake_accel fakeAccel(samples);
    tap_detection_algo algo(fakeAccel);

    EXPECT_FALSE(algo.run()); 
    EXPECT_FALSE(algo.run());
    EXPECT_TRUE(algo.run());  
}

// Test 3: Detect multiple tap events in a sequence.
TEST(TapDetectionAlgoTest, MultipleTapsDetection) {
    std::vector<accelerometer::data> samples = {
        {0.0f, 1.0f, 0.0f},  // initial reading
        {0.0f, 1.0f, 0.0f},  // no change -> false
        {0.0f, 2.0f, 0.0f},  // tap detected (change: 1.0 > threshold)
        {0.0f, 2.1f, 0.0f},  // small change -> false
        {0.0f, 3.0f, 0.0f}   // tap detected (change: 0.9 > threshold)
    };
    fake_accel fakeAccel(samples);
    tap_detection_algo algo(fakeAccel);

    EXPECT_FALSE(algo.run());
    EXPECT_FALSE(algo.run());
    EXPECT_TRUE(algo.run()); 
    EXPECT_FALSE(algo.run());
    EXPECT_TRUE(algo.run());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}