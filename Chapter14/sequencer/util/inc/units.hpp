#pragma once

namespace units 
{
    template<typename T, typename ST>
    class unit {
    private:
	    T val_; 
    public:
	    explicit unit(T val) : val_(val){}
	    [[nodiscard]] T get() const {return val_;}
        [[nodiscard]] T get_mili() const {return 1e3 * val_;}

        constexpr T operator/(const unit& second) const{
            return val_ / second.get();
        }

        constexpr unit operator*(const T& second) const{
            return unit(val_ * second);
        }

        constexpr unit operator/(const T& second) const{
            return unit(val_ / second);
        }
    };

    using voltage = unit<float, struct the_voltage>;
    using resistance = unit<float, struct the_resistance>;

    voltage operator""_V(long double volts);

    resistance operator""_Ohm(long double ohms);
};