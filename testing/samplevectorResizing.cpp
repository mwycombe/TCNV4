#include <iostream>
#include <vector>

int32_t get_size(const std::vector<int32_t>& v )
{
    return v.size();
}
int32_t get_size(const std::vector<std::vector<int32_t>& va_list)
{
    int32_t size {};
    for (const auto& vv : v)
        {
            size += get_size(vv);
        }
    return size;
}

int main ()
{
   
  int32_t x_dim{10};
  int32_t y_dim{20};
  int32_t z_dim(30);

   std::vector<std::vector<std::vector<int>>> cube;
    cube.resize(x_dim);
    for (auto& plane : cube) {
        plane.resize(y_dim);
        for (auto& row : plane) {
            row.resize(z_dim);
        }
    }

    // practice access dm properties

    std::cout << "cube size/capacity := " << (int) (cube.size()) << " / " << (int) (cube.capacity()) << "\n";

    // to get size of plane, have to walk through all the rows and add them up

    int32_t plane_size{0};

    return 0;

}