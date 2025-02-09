#include <units.hpp>

namespace units {
voltage operator""_V(long double volts) {
    return voltage(static_cast<float>(volts));
}

resistance operator""_Ohm(long double ohms) {
    return resistance(static_cast<float>(ohms));
}
};
