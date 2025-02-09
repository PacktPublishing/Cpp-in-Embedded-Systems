#include <cstdio>
#include <cstdint>

class accelerometer {
public:
    struct data{
        float x;
        float y;
        float z;
    };
    enum class sampling_rate {
        c_20_hz,
        c_50_hz,
        c_100_hz,
    };
    virtual void set_sampling_rate(sampling_rate) = 0;
    virtual data get_data() = 0;
};

class adxl_345 : public accelerometer {
public:
    void set_sampling_rate(sampling_rate) override {
        printf("adxl_345: setting sampling rate\r\n");
    }
    data get_data() override {
        return data{0.02f, 0.981f, 0.03f};
    }
};

class tap_detection_algo {
public:
    tap_detection_algo(accelerometer &accel) : 
        accel_(accel) {
            accel_.set_sampling_rate(accelerometer::sampling_rate::c_100_hz);
    }
    bool run () {
        auto accel_data = accel_.get_data();
        printf("algo: x = %.2f, y = %.2f, z = %.2f\r\n", 
               accel_data.x, accel_data.y, accel_data.z);
        // process data
        return false;
    }
private:
    accelerometer &accel_;
};

int main() {
    adxl_345 accel;
    tap_detection_algo algo(accel);
    algo.run();

    return 0;
}