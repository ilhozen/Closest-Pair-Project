#include <iostream>
#include <vector>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <SFML/Graphics.hpp>
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

Result dacMerge(const vector<Point>& vec, int lo, int median, int hi, Result left, Result right) {
	Result closestPair;
	if (left.distance < right.distance) {
		closestPair = left;
	}
	else {
		closestPair = right;
	}

	vector<Point> strip;
	double medianX = vec[median].x;
	for (int i = lo; i <= hi; i++) {
		if (fabs(vec[i].x - medianX) < closestPair.distance) {
			strip.push_back(vec[i]);
		}
	}

	sort(strip.begin(), strip.end(), [](const Point& a, const Point& b) {
		return a.y < b.y;
		});

	for (int i = 0; i < strip.size(); i++) {
		for (int j = i + 1; j < strip.size() && (strip[j].y - strip[i].y) < closestPair.distance; j++) {
			double d = calculateDistance(strip[i], strip[j]);
			if (d < closestPair.distance) {
				closestPair.distance = d;
				closestPair.p1 = strip[i];
				closestPair.p2 = strip[j];
			}
		}
	}

	return closestPair;
}

Result dacRecursive(const vector<Point>& vec, int lo, int hi) {
	Result closestPair;
	if (hi - lo == 1) {
		closestPair.p1 = vec[lo];
		closestPair.p2 = vec[hi];
		closestPair.distance = calculateDistance(vec[lo], vec[hi]);
		return closestPair;
	}
	if (hi - lo == 0) {
		closestPair.p1 = vec[lo];
		closestPair.p2 = vec[lo];
		//Default distance is max
		return closestPair;
	}
	int median = (lo + hi) / 2;
	Result left = dacRecursive(vec, lo, median);
	Result right = dacRecursive(vec, median + 1, hi);
	return dacMerge(vec, lo, median, hi, left, right);
}

Result divideAndConquer(const vector<Point>& unsortedVec) {
	vector<Point> vec = unsortedVec;
	sort(vec.begin(), vec.end(), [](const Point& a, const Point& b) {
		return a.x < b.x;
		});
	return dacRecursive(vec, 0, (vec.size() - 1));
}

int main() {
	string line;
	double x, y;
	vector<Point> points;
	ifstream pointFile("points_.txt");

	if (!pointFile.is_open()) {
		cout << "Unable to open file." << endl;
		return 0;
	}

	while (getline(pointFile, line)) {
		stringstream pointStream(line);
		if (pointStream >> x >> y) {
			Point p({ x, y });
			points.push_back(p);
		}
	}

	Result result = divideAndConquer(points);

	cout << "Closest Pair: (" << result.p1.x << ", " << result.p1.y << ") and ("
		<< result.p2.x << ", " << result.p2.y << ")" << endl;
	cout << "Distance: " << result.distance << endl;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Closest Pair");

	sf::Font font;
	if (!font.loadFromFile("Minecraftia-Regular.ttf"))
		return EXIT_FAILURE;
	sf::Text text("Hello World!", font, 24);
	text.setPosition(352, 336);

	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(text);
		window.display();
	}

	return 0;
}