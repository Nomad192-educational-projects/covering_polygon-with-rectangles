#include <iostream>
#include <fstream>

using namespace std;

constexpr char test[] = "02.txt";
constexpr size_t n_result_rectangles = 300;
constexpr size_t n_after_dot = 9;
size_t n;
double c1, c2;
double minX, maxX;
double minY, maxY;

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

struct point_t
{
    double x, y;
};

struct rectangle_t
{
    point_t left_bot;
    point_t right_top;
};

rectangle_t result_rectangles[3000];

point_t start_polygon[3000];

double dist(double a, double b)
{
    return MAX(a, b) - MIN(a, b);
}

bool equal(double A, double B, int maxUlps)
{
    if (maxUlps > 0 && maxUlps < 4 * 1024 * 1024)
    {
        int64_t aInt = *(int64_t*)&A;
        // Remove the sign in aInt, if any, to get a properly ordered sequence
        if (aInt < 0)
            aInt = 0x8000000000000000 - aInt;
        // NOT aInt &= 0x7fffffff;
        /// Narrowing conversion from 'unsigned int'
        /// to signed type 'int' is implementation-defined

        // Similarly for bInt
        int64_t bInt = *(int64_t*)&B;
        if (bInt < 0)
            bInt = 0x8000000000000000 - bInt;
        uint64_t intDiff = abs(aInt - bInt);
        if (intDiff <= maxUlps)
            return true;
    }
    return false;
}

pair<bool, double> intersection_segment_Y(point_t a, point_t b, double x)
{
    if(MIN(a.x, b.x) <= x && MAX(a.x, b.x) >= x)
    {
        double dist_k = dist(x, MIN(a.y, b.y) == a.y ? a.x : b.x);
        double k = dist_k;
        double dist_x = dist(a.x, b.x);
        k /= dist_x;
        double dist_y = dist(a.y, b.y);
        k *= dist_y;
        double min_y = MIN(a.y, b.y);
        k += min_y;
        return {true, k};
    }
    else if(equal(a.x, x, 100000))
    {
        return {true, a.y};
    }
    else if(equal(b.x, x, 100000))
    {
        return {true, b.y};
    }

    return {false, 0};
}

pair<double, double> intersection_polygon_Y(double x, int &number_of_intersection)
{
    pair<double, double> result;
    size_t i = 0;
    number_of_intersection = 0;
    for(; i < n - 1; i++)
    {
        size_t cur = i, next = i + 1;

        if(auto pair = intersection_segment_Y(start_polygon[cur], start_polygon[next], x); pair.first)
        {
            result.first = pair.second;
            result.second = pair.second;
            number_of_intersection++;
            break;
        }
    }
    i++;
    for(; i < n - 1; i++)
    {
        size_t cur = i, next = i + 1;
        if(auto pair = intersection_segment_Y(start_polygon[cur], start_polygon[next], x); pair.first)
        {
            if(result.first < pair.second)
                result.first = pair.second;
            if(result.second > pair.second)
                result.second = pair.second;
            number_of_intersection++;
        }
    }
    size_t cur = n - 1, next = 0;
    if(auto pair = intersection_segment_Y(start_polygon[cur], start_polygon[next], x); pair.first)
    {
        if(result.first < pair.second)
            result.first = pair.second;
        if(result.second > pair.second)
            result.second = pair.second;
        number_of_intersection++;
    }

    return result;
}


int main() {
    ifstream fin(test);

    if(!fin.is_open())
    {
        cout << "not open";
        return -1;
    }

    fin >> n >> c1 >> c2;
    fin.precision(n_after_dot);
    double x, y;
    fin >> x >> y;
    start_polygon[0] = {x, y};
    minX = x;
    maxX = x;
    minY = y;
    maxY = y;
    for(size_t i = 1; i < n; i++)
    {
        fin >> x >> y;
        start_polygon[i] = {x, y};
        if(minX > x)
            minX = x;
        if(maxX < x)
            maxX = x;
        if(minY > y)
            minY = y;
        if(maxY < y)
            maxY = y;
    }



    double left_x = minX;
    double step = (maxX - minX)/n_result_rectangles;
    int trash;
    pair<double, double> prev_result = intersection_polygon_Y(left_x, trash);
    for (size_t i = 0; i < n_result_rectangles - 1; ++i) {
        double right_x = left_x + step;
        auto res = intersection_polygon_Y(right_x, trash);

        result_rectangles[i] = {
                {left_x, MIN(prev_result.second, res.second)},
                {right_x, MAX(prev_result.first, res.first)}};

        left_x = right_x;
        prev_result = res;
    }
    {
        size_t i = n_result_rectangles - 1;
        double right_x = maxX;

        auto res = intersection_polygon_Y(right_x, trash);

        result_rectangles[i] = {
                {left_x, MIN(prev_result.second, res.second)},
                {right_x, MAX(prev_result.first, res.first)}};
    }

    ofstream fout("out.txt");
    fout.precision(n_after_dot);

    fout << n_result_rectangles << "\n";
    for (size_t i = 0; i < n_result_rectangles; ++i) {
        auto &item = result_rectangles[i];
        fout << std::fixed << item.left_bot.x << " " << item.left_bot.y << " " << item.right_top.x << " " << item.right_top.y << "\n";
    }


//    fout << 1 << "\n";
//    fout << std::fixed << minX << " " << minY << " " << maxX << " " << maxY << "\n";

    return 0;
}

/*
 *
 *
5 1 2
0 0
2 0
2 2
0 2
1.5 1


3 1 2
0 0
4 4
4 6
 */