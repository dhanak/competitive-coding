#include <numeric>
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

struct Vec {
    int index, rs, cs;
    std::vector<int> data;
    int not_nulls;
    Vec(int r, int c) : Vec(-1, r, c) {}
    Vec(int index, int r, int c) : index(index), rs(r), cs(c), data(r*c) {}
    
    int& operator()(int r, int c) {
        return data[r*cs+c];
    }
    
    int get_not_nulls_count() {
        if (not_nulls == 0) {
            for (int d : data) {
                if (d) ++not_nulls;
            }
        }
        return not_nulls;
    }
    
    int sum() {
        return std::accumulate(data.begin(), data.end(), 0);
    }
};

std::istream& operator>>(std::istream& input, Vec& v) {
    for (int i = 0; i < v.rs; ++i) {
        for (int j = 0; j < v.cs; ++j) {
            input >> v(i, j);
        }
    }
    return input;
}
std::ostream& operator<<(std::ostream& output, Vec& v) {
    // output << v.rs << " " << v.cs << std::endl;
    for (int i = 0; i < v.rs; ++i) {
        for (int j = 0; j < v.cs; ++j) {
            output << v(i, j) << " ";
        }
        output << std::endl;
    }
    return output;
}

int main() {
    int r, c, m;
    std::cin >> r >> c >> m;
    const int m_p_2 = (m + 1) / 2;
    Vec vec(r, c);
    std::cin >> vec;

    std::vector<Vec> blocks;
    int n;
    std::cin >> n;
    blocks.reserve(n);
    
    for (int i = 0; i < n; ++i) {
        int rb, cb;
        std::cin >> rb >> cb;
        blocks.emplace_back(i, rb, cb);
        std::cin >> blocks.back();
    }
    std::vector<std::pair<int, int>> result(n);

    std::sort(blocks.begin(), blocks.end(), [&vec](auto&& b1, auto&& b2) {
        return std::forward_as_tuple(b1.get_not_nulls_count(), -(vec.rs + 1 - b1.rs) * (vec.cs + 1 - b1.cs), b1.rs * b1.cs) > 
            std::forward_as_tuple(b2.get_not_nulls_count(), -(vec.rs + 1 - b2.rs) * (vec.cs + 1 - b2.cs), b2.rs * b2.cs);
    });
    
    for (auto&& block : blocks) {
        int curr_max_saving = -block.rs * block.cs * m;
        int curr_max_new_zeros = 0;
        // max elem: result[block.index];
        for (int i = 0; i < vec.rs - block.rs + 1; ++i) {
            for (int j = 0; j < vec.cs - block.cs + 1; ++j) {
                int saving = 0;
                int new_zeros = 0;
                for (int ii = 0; ii < block.rs; ++ii) {
                    for (int jj = 0; jj < block.cs; ++jj) {
                        int new_val = (vec(i+ii, j+jj) + block(ii, jj)) % m;
                        if (!new_val) ++new_zeros;

                        // saving += vec(i+ii, j+jj) - new_val;
                        // ez így nagyon buta, esetleg lehetne a 0 -tól való távolság (mod m), csak az meg a végén nem segít
                        saving += (m_p_2 - std::abs(vec(i+ii, j+jj)-m_p_2)) - (m_p_2 - std::abs(new_val - m_p_2));
                    }
                }
                
                if (std::forward_as_tuple(saving, new_zeros) > std::forward_as_tuple(curr_max_saving, curr_max_new_zeros)) {
                    curr_max_saving = saving;
                    curr_max_new_zeros = new_zeros;
                    result[block.index] = std::make_pair(i, j);
                }
            }
        }
        // apply max_saving
        int i, j;
        std::tie(i, j) = result[block.index];
        for (int ii = 0; ii < block.rs; ++ii) {
            for (int jj = 0; jj < block.cs; ++jj) {
                vec(i+ii, j+jj) = (vec(i+ii, j+jj) + block(ii, jj)) % m;
            }
        }
        std::cerr << " after place " << block.index << " to (" << i << "," << j << ") result is : " << std::endl << vec << std::endl << "sum: " << vec.sum() << std::endl;
    }
    
    for (auto&& p : result) {
        std::cout << p.first+1 << " " << p.second+1 << std::endl;
    }
}
