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

Result dacMerge(const vector<Point>& vec, int lo, int median, int hi, Result left, Result right, sf::RenderWindow& window, double scale, const vector<sf::Vector2f>& animPoints, sf::CircleShape& point) {
	Result closestPair;
	if (left.distance <= right.distance) {
		closestPair = left;
	}
	else {
		closestPair = right;
	}

	window.clear(sf::Color::Color(0, 8, 16, 255));
	for (const auto& p : animPoints) {
		point.setPosition(p);
		window.draw(point);
	}

	sf::Vertex closestLine[2];
	closestLine[0].position = sf::Vector2f(400 + (closestPair.p1.x) * scale, 400 - (closestPair.p1.y) * scale);
	closestLine[0].color = sf::Color::Color(115, 255, 130, 255);
	closestLine[1].position = sf::Vector2f(400 + (closestPair.p2.x) * scale, 400 - (closestPair.p2.y) * scale);
	closestLine[1].color = sf::Color::Color(115, 255, 130, 255);
	window.draw(closestLine, 2, sf::Lines);

	sf::Vertex line[2];
	line[0].position = sf::Vector2f(animPoints[median].x, 25);
	line[0].color = sf::Color::Red;
	line[1].position = sf::Vector2f(animPoints[median].x, 775);
	line[1].color = sf::Color::Red;
	window.draw(line, 2, sf::Lines);

	cout << closestPair.distance << endl;
	int stripLen = closestPair.distance * 2 * scale;
	cout << stripLen << endl;
	int stripPosX = 400 + (vec[median].x * scale);
	sf::RectangleShape rectangle(sf::Vector2f(stripLen, 750));
	rectangle.setPosition(stripPosX - (stripLen / 2), 25);
	rectangle.setFillColor(sf::Color(255, 240, 100, 32));
	window.draw(rectangle);
	window.display();
	sf::sleep(sf::milliseconds(200));


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

	window.clear(sf::Color::Color(0, 8, 16, 255));
	for (const auto& p : animPoints) {
		point.setPosition(p);
		window.draw(point);
	}

	closestLine[0].position = sf::Vector2f(400 + (closestPair.p1.x) * scale, 400 - (closestPair.p1.y) * scale);
	closestLine[1].position = sf::Vector2f(400 + (closestPair.p2.x) * scale, 400 - (closestPair.p2.y) * scale);
	window.draw(closestLine, 2, sf::Lines);
	
	window.draw(line, 2, sf::Lines);

	window.draw(rectangle);

	window.display();
	sf::sleep(sf::milliseconds(300));
	
	return closestPair;
}

Result dacRecursive(const vector<Point>& vec, int lo, int hi, sf::RenderWindow& window, const vector<sf::Vector2f>& animPoints, sf::CircleShape& point, double scale) {
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
	window.clear(sf::Color::Color(0, 8, 16, 255));
	for (const auto& p : animPoints) {
		point.setPosition(p);
		window.draw(point);
	}

	sf::Vertex line[2];
	line[0].position = sf::Vector2f(animPoints[median].x, 25);
	line[0].color = sf::Color::Red;
	line[1].position = sf::Vector2f(animPoints[median].x, 775);
	line[1].color = sf::Color::Red;
	window.draw(line, 2, sf::Lines);
	window.display();
	sf::sleep(sf::milliseconds(200));

	Result left = dacRecursive(vec, lo, median, window, animPoints, point, scale);
	Result right = dacRecursive(vec, median + 1, hi, window, animPoints, point, scale);
	return dacMerge(vec, lo, median, hi, left, right, window, scale, animPoints, point);
}

Result divideAndConquer(const vector<Point>& unsortedVec, sf::RenderWindow& window, const vector<sf::Vector2f>& animPoints, sf::CircleShape& point, double scale) {
	vector<Point> vec = unsortedVec;
	sort(vec.begin(), vec.end(), [](const Point& a, const Point& b) {
		return a.x < b.x;
		});
	return dacRecursive(vec, 0, (vec.size() - 1), window, animPoints, point, scale);
}

int main() {
	string line;
	double x, y;
	vector<Point> points;
	ifstream pointFile("points_.txt");

	sf::RenderWindow window(sf::VideoMode(800, 800), "Closest Pairs");
	sf::CircleShape point(1);
	point.setFillColor(sf::Color::Color(255, 199, 0));

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

	double minX = DBL_MAX, minY = DBL_MAX, maxX = -DBL_MAX, maxY = -DBL_MAX;
	for (const auto& p : points) {
		if (p.x < minX) minX = p.x;
		if (p.y < minY) minY = p.y;
		if (p.x > maxX) maxX = p.x;
		if (p.y > maxY) maxY = p.y;
	}

	double scale = min(750.0 / (maxX - minX), 750.0 / (maxY - minY));

	sf::Vector2f windowCenter(400, 400);

	vector<sf::Vector2f> animPoints;
	for (auto& p : points) {
		double scaledX = (p.x) * scale;
		double scaledY = (p.y) * scale;
		animPoints.emplace_back(windowCenter.x + scaledX, windowCenter.y - scaledY);
	}

	sort(animPoints.begin(), animPoints.end(), [](const sf::Vector2f& a, const sf::Vector2f& b) {
		return a.x < b.x;
		});

	Result result = divideAndConquer(points, window, animPoints, point, scale);
	cout << "Closest Pair: (" << result.p1.x << ", " << result.p1.y << ") and ("
		<< result.p2.x << ", " << result.p2.y << ")" << endl;
	cout << "Distance: " << result.distance << endl;


	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

	}

	return 0;
}