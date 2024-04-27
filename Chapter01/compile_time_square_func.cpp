constexpr int square(int a) {
 return a*a;
}

int main() {
 constexpr int ret = square(2);
 return ret;
}
