#include <iostream>
#include <vector>

int main() {
    size_t n;
    std::cin >> n;
    std::vector<int> arr(n);
    size_t count1 = 0, count2 = 0;
    for (size_t i = 0; i < n; i++) {
        std::cin >> arr[i];
        if (arr[i] == 1) {
            ++count1;
        } else if (arr[i] == 2) {
            ++count2;
        }
    }

    size_t res = 0;

    for (size_t i = 0; i < count1 + count2; i++) {
        if (i < count1) { // zone 1
            if (arr[i] == 3) {
                for (size_t j = n - 1; j >= count1; j--) {
                    if (arr[j] == 1) {
                        std::swap(arr[i], arr[j]);
                        ++res;
                        break;
                    }
                }
            } else if (arr[i] == 2) {
                for (size_t j = count1; j < n; j++) {
                    if (arr[j] == 1) {
                        std::swap(arr[i], arr[j]);
                        ++res;
                        break;
                    }
                }
            }
        } else { // zone 2
            if (arr[i] == 3) {
                for (size_t j = count1 + count2; j < n; j++) {
                    if (arr[j] == 2) {
                        std::swap(arr[i], arr[j]);
                        ++res;
                        break;
                    }
                }
            }
        }
    }
    std::cout << res << '\n';
    return 0;
}
