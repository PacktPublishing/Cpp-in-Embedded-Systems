#include <cstdio>
#include <cstdint>

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

class i2c {
public:
    virtual void write() = 0;
};

class i2c_stm32 : public i2c {
public:
    void write() override {
        printf("i2c::write...\r\n");
    }
};

class adxl_345 : public accelerometer {
public:
    adxl_345(i2c &i2c_obj) : i2c_(i2c_obj) {}
    error set_sampling_rate(sampling_rate) override {
        printf("adxl_345: setting sampling rate\r\n");
        i2c_.write();
        return error::ok;
    }
    data get_data() override {
        return data{0.02f, 0.981f, 0.03f};
    }
private:
    i2c &i2c_;
};

class tap_detection_algo {
public:
    tap_detection_algo(accelerometer &accel) : 
        accel_(accel) {
            auto err = accel_.set_sampling_rate(accelerometer::sampling_rate::c_100_hz);
            if(err == accelerometer::error::not_supported) {
                // try another sampling rate and adapt
            }
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
    i2c_stm32 i2c1;
    adxl_345 accel(i2c1);
    tap_detection_algo algo(accel);
    algo.run();

    return 0;
}