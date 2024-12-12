#include <iostream>
#include <vector>
#include <cmath>
#include <cfloat>
#include <algorithm>
using namespace std;

struct Point {
	double x, y;
};

double calculateDistance(const Point& p1, const Point& p2) {
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

struct Result {
	Point p1, p2;
	double distance = DBL_MAX;
};

Result naive(const vector<Point>& vec) {
	Result closestPair;
	for (int i = 0; i < vec.size(); i++) {
		for (int j = i + 1; j < vec.size(); j++) {
			if (calculateDistance(vec[i], vec[j]) < closestPair.distance) {
				closestPair.distance = calculateDistance(vec[i], vec[j]);
				closestPair.p1 = vec[i];
				closestPair.p2 = vec[j];
			}
		}
	}
	return closestPair;
}

Result divideAndConquer(const vector<Point>& vec) {
	sort(vec.begin(), vec.end(), [](const Point& a, const Point& b) {
		return a.x < b.x;
		});
	return divAndConRecursive(vec);
}

Result divAndConRecursive(const vector<Point>& vec, int lo, int hi) {
	if (lo >= hi) {
		return;
	}
	int median = (lo + hi) / 2;


}

int main() {



	/*vector<Point> points = {
		{0.0, 0.0},
		{1.0, 1.0},
		{2.0, 2.0},
		{1.0, 0.0},
		{0.7, 0.5}
	};

	Result result = naive(points);

	cout << "Closest Pair: (" << result.p1.x << ", " << result.p1.y << ") and ("
		<< result.p2.x << ", " << result.p2.y << ")" << endl;
	cout << "Distance: " << result.distance << endl;*/

	return 0;
}