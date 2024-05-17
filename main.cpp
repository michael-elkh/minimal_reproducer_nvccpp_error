#include <array>
#include <execution>
#include <vector>

#include "cartesian_product.hpp"
#include "mdspan.hpp"

// For utils
#include <iomanip>
#include <iostream>

using idx_t = uint32_t;
using vec_t = std::vector<double>;

[[maybe_unused]] constexpr bool kLog = false;

constexpr std::array<idx_t, 3> kDims = {6, 5, 4};
constexpr std::array<std::array<idx_t, 2>, 3> kConstBorders = {1U, 2U, 2U, 1U, 1U, 2U};
constexpr idx_t kKernelSize = std::accumulate(kConstBorders.begin(),
                                              kConstBorders.end(),
                                              1U,
                                              [](idx_t acc, auto dim) {
                                                  return acc * (1 + dim[0] + dim[1]);
                                              });

/*
    UTILS
*/
void printVs(const vec_t& ref, const vec_t& res) {
    constexpr int kIntWidth = 3;
    auto md_a = std::mdspan(ref.data(), kDims[2], kDims[1], kDims[0]);
    auto md_b = std::mdspan(res.data(), kDims[2], kDims[1], kDims[0]);

    std::cout << "------------ Reference ------------"
              << "\t / \t";
    for (auto i = 0; i < kIntWidth; i++) {
        std::cout << " ";
    }
    std::cout << "------------ Result ------------"
              << std::endl
              << std::endl;

    for (idx_t i = 0; i < md_a.extent(0); i++) {
        for (idx_t j = 0; j < md_a.extent(1); j++) {
            for (idx_t k = 0; k < md_a.extent(2); k++) {
                std::cout << std::setw(kIntWidth)
                          << md_a(i, j, k)
                          << " ";
            }
            std::cout << "\t / \t ";
            for (idx_t k = 0; k < md_a.extent(2); k++) {
                std::cout << std::setw(kIntWidth)
                          << md_b(i, j, k)
                          << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

template <double Atol = 1e-08, double Rtol = 1e-05>
constexpr bool approxEqual(const vec_t& mat_a, const vec_t& mat_b) {
    return std::transform_reduce(mat_a.begin(),
                                 mat_a.end(),
                                 mat_b.begin(),
                                 true,
                                 std::logical_and<>(),
                                 [](double a, double b) {
                                     bool eq = std::abs(a - b) <= (Atol + Rtol * std::abs(b));
                                     if constexpr (kLog) {
                                         if (!eq) {
                                             std::cout << a << " / " << b << std::endl;
                                         }
                                     }
                                     return eq;
                                 });
}

/*
    Sequential reference
*/

vec_t sequentialReference(const vec_t& src) {
    vec_t dst;
    dst.resize(src.size());

    auto md_src = std::mdspan(src.data(), kDims[2], kDims[1], kDims[0]);
    auto md_dst = std::mdspan(dst.data(), kDims[2], kDims[1], kDims[0]);

    for (idx_t z = kConstBorders[2][0]; z < (kDims[2] - kConstBorders[2][1]); z++) {
        for (idx_t y = kConstBorders[1][0]; y < (kDims[1] - kConstBorders[1][1]); y++) {
            for (idx_t x = kConstBorders[0][0]; x < (kDims[0] - kConstBorders[0][1]); x++) {
                double sum = 0.0;

                for (idx_t i = z - kConstBorders[2][0]; i <= z + kConstBorders[2][1]; i++) {
                    for (idx_t j = y - kConstBorders[1][0]; j <= y + kConstBorders[1][1]; j++) {
                        for (idx_t k = x - kConstBorders[0][0]; k <= x + kConstBorders[0][1]; k++) {
                            sum += md_src(i, j, k);
                        }
                    }
                }

                md_dst(z, y, x) = sum / kKernelSize;
            }
        }
    }

    return dst;
}

/*
    STL Code that provokes the error.
*/

vec_t stlVersionWithCarthesianProduct(const vec_t& src) {
    constexpr auto kPolicy = std::execution::par_unseq;

    vec_t dst;
    dst.resize(src.size());

    auto md_src = std::mdspan(src.data(), kDims[2], kDims[1], kDims[0]);
    auto md_dst = std::mdspan(dst.data(), kDims[2], kDims[1], kDims[0]);

    // We consider the constant borders
    auto xs = std::views::iota(kConstBorders[0][0], kDims[0] - kConstBorders[0][1]);
    auto ys = std::views::iota(kConstBorders[1][0], kDims[1] - kConstBorders[1][1]);
    auto zs = std::views::iota(kConstBorders[2][0], kDims[2] - kConstBorders[2][1]);

    auto idxs = std::views::cartesian_product(zs, ys, xs);

    std::for_each(kPolicy,
                  idxs.begin(),
                  idxs.end(),
                  [md_src,
                   md_dst](auto idx) {
                      const auto& [z, y, x] = idx;

                      double sum = 0.0;

                      for (idx_t i = z - kConstBorders[2][0]; i <= z + kConstBorders[2][1]; i++) {
                          for (idx_t j = y - kConstBorders[1][0]; j <= y + kConstBorders[1][1]; j++) {
                              for (idx_t k = x - kConstBorders[0][0]; k <= x + kConstBorders[0][1]; k++) {
                                  sum += md_src(i, j, k);
                              }
                          }
                      }

                      md_dst(z, y, x) = sum / kKernelSize;
                  });
    return dst;
}

int main(int argc, char** argv) {
    vec_t src;
    src.resize(kDims[0] * kDims[1] * kDims[2]);

    std::iota(src.begin(), src.end(), 100);

    auto ref = sequentialReference(src);
    auto res = stlVersionWithCarthesianProduct(src);

    printVs(ref, res);

    return EXIT_SUCCESS;
}