#include <iostream>
#include <cmath>

long long length(long long num) {
    long long res = 0;
    while (num != 0) {
        num /= 10;
        res++; 
    }
    return res;
}

int main() {
    long long n, m, res;
    std::cin >> n >> m;
    res = (n % m != 0) ? 0 : -1; // считаем числа, которые строго меньше n
    while (n != 0) {
        if (length(n) != length(n - 1)) { // 10, 100, 1000,... не дают эффекта, если m != 1
            break;
        }
        long long temp = std::pow(10, length(n) - 1);
        temp = (temp % m != 0) ? temp : temp-1;
        res += n/m - temp/m;
        n /= 10;
    }
    std::cout << (res < 0 ? 0 : res) << std::endl;
    return 0;
}
