#include <array>
#include <cmath>
#include <fstream>

template <typename T, std::size_t N>
struct signal : public std::array<T, N>
{

  constexpr signal() {}

  constexpr signal(float begin, float end)
  {
    float step = (end - begin) / (N - 1);

    for (std::size_t i = 0; i < N; i++)
    {
      this->at(i) = begin + i * step;
    }
  }

  constexpr signal(const std::array<T, N> &x, auto fun)
  {
    for (std::size_t i = 0; i < N; i++)
    {
      this->at(i) = fun(x.at(i));
    }
  }

  constexpr signal(const signal &sig, auto fun)
  {
    for (std::size_t i = 0; i < N; i++)
    {
      this->at(i) = fun(sig.at(i));
    }
  }

  constexpr signal operator+(const T &t) const
  {
    return signal(*this, [&](T elem)
                  { return elem + t; });
  };

  constexpr signal operator-(const T &t) const
  {
    return signal(*this, [&](T elem)
                  { return elem - t; });
  };

  constexpr signal operator*(const T &t) const
  {
    return signal(*this, [&](T elem)
                  { return elem * t; });
  };

  constexpr signal operator/(const T &t) const
  {
    return signal(*this, [&](T elem)
                  { return elem / t; });
  };

  constexpr signal operator+(const signal &sig) const
  {
    signal ret;
    for (std::size_t i = 0; i < N; i++)
    {
      ret.at(i) = this->at(i) + sig.at(i);
    }
    return ret;
  };

  constexpr signal operator-(const signal &sig) const
  {
    signal ret;
    for (std::size_t i = 0; i < N; i++)
    {
      ret.at(i) = this->at(i) - sig.at(i);
    }
    return ret;
  };

  friend constexpr signal operator+(const T &t, const signal &sig)
  {
    return sig + t;
  }

  friend constexpr signal operator*(const T &t, const signal &sig)
  {
    return sig * t;
  }

  friend constexpr signal operator/(const T &t, const signal &sig)
  {
    signal ret;
    for (std::size_t i = 0; i < N; i++)
    {
      ret.at(i) = t / sig.at(i);
    }
    return ret;
  }
};

int main()
{

  constexpr float A = 1.18090254918130e-3;
  constexpr float B = 2.16884014794388e-4;
  constexpr float C = 1.90058756197216e-6;
  constexpr float D = 1.83161892641824e-8;

  constexpr int c_lut_points = 50;

  constexpr signal<float, c_lut_points> resistance(1e3, 10e3);

  constexpr auto temperature_k = 1 / (A +
                                      B * signal(resistance, [](float x)
                                                 { return std::log(x); }) +
                                      C * signal(resistance, [](float x)
                                                 { return std::pow(std::log(x), 2); }) +
                                      D * signal(resistance, [](float x)
                                                 { return std::pow(std::log(x), 3); }));

  constexpr auto temperature_celsius = temperature_k - 273.15f;

  std::ofstream file("out.csv");
  file << "Resistance[Ohm], Temperature[Celsius]\n";
  for (int i = 0; i < c_lut_points; i++)
  {
    file << resistance[i] << ", " << temperature_celsius[i] << "\n";
  }

  return 0;
}