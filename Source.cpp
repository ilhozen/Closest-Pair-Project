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

#define M_PI 3.14159265358979323846
#define DELAY 200
#define THICKNESS 5
#define ALGORITHM naive

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

sf::RectangleShape createThickLine(sf::Vector2f p1, sf::Vector2f p2, double thickness, sf::Color color) {

	sf::Vector2f direction = p2 - p1;
	double length = sqrt(direction.x * direction.x + direction.y * direction.y);
	sf::RectangleShape line(sf::Vector2f(length, thickness));
	line.setPosition(p1);
	line.setFillColor(color);
	line.setRotation(atan2(direction.y, direction.x) * 180 / M_PI);
	return line;

}

void drawPoints(sf::RenderWindow& window, const vector<sf::Vector2f>& animPoints) {

	sf::CircleShape point(1);
	point.setFillColor(sf::Color::Color(255, 199, 0));

	for (const auto& p : animPoints) {
		point.setPosition(p);
		window.draw(point);
	}

}

void drawGlobalLine(sf::RenderWindow& window, Result& gcp, double scale) {

	sf::Vector2f globalP1(400 + (gcp.p1.x) * scale, 400 - (gcp.p1.y) * scale);
	sf::Vector2f globalP2(400 + (gcp.p2.x) * scale, 400 - (gcp.p2.y) * scale);
	sf::RectangleShape purpleLine = createThickLine(globalP1, globalP2, THICKNESS, sf::Color::Color(158, 0, 255, 255));
	window.draw(purpleLine);

}

void drawClosestLine(sf::RenderWindow& window, Result& closestPair, double scale) {

	sf::Vertex closestLine[2];
	closestLine[0].position = sf::Vector2f(400 + (closestPair.p1.x) * scale, 400 - (closestPair.p1.y) * scale);
	closestLine[1].position = sf::Vector2f(400 + (closestPair.p2.x) * scale, 400 - (closestPair.p2.y) * scale);
	closestLine[0].color = sf::Color::Color(115, 255, 130, 255);
	closestLine[1].color = sf::Color::Color(115, 255, 130, 255);
	window.draw(closestLine, 2, sf::Lines);

}

void drawMedianLine(sf::RenderWindow& window, const vector<sf::Vector2f>& animPoints, int median) {

	sf::Vertex line[2];
	line[0].position = sf::Vector2f(animPoints[median].x, 25);
	line[1].position = sf::Vector2f(animPoints[median].x, 775);
	line[0].color = sf::Color::Red;
	line[1].color = sf::Color::Red;
	window.draw(line, 2, sf::Lines);

}

void drawStrip(sf::RenderWindow& window, int stripLen, int stripPosX) {

	sf::RectangleShape rectangle(sf::Vector2f(stripLen, 750));
	rectangle.setPosition(stripPosX - (stripLen / 2), 25);
	rectangle.setFillColor(sf::Color(255, 240, 100, 32));
	window.draw(rectangle);

}

Result naive(const vector<Point>& vec, sf::RenderWindow& window, vector<sf::Vector2f>& animPoints, double scale) {

	Result closestPair;
	Result progress;

	for (int i = 0; i < vec.size(); i++) {
		for (int j = i + 1; j < vec.size(); j++) {

			progress.p1 = vec[i];
			progress.p2 = vec[j];

			if (calculateDistance(vec[i], vec[j]) < closestPair.distance) {
				closestPair.distance = calculateDistance(vec[i], vec[j]);
				closestPair.p1 = vec[i];
				closestPair.p2 = vec[j];
			}

			window.clear(sf::Color::Color(0, 8, 16, 255));

			drawPoints(window, animPoints);
			drawClosestLine(window, progress, scale);
			drawGlobalLine(window, closestPair, scale);

			window.display();
			//sf::sleep(sf::milliseconds(DELAY));
		}
	}
	return closestPair;

}

Result dacMerge(const vector<Point>& vec, int lo, int median, int hi, Result left, Result right, sf::RenderWindow& window, double scale, const vector<sf::Vector2f>& animPoints, Result& gcp) {

	Result closestPair;

	if (left.distance <= right.distance) {
		closestPair = left;
	}
	else{
		closestPair = right;
	}

	if (closestPair.distance < gcp.distance) {
		gcp = closestPair;
	}

	window.clear(sf::Color::Color(0, 8, 16, 255));

	drawPoints(window, animPoints);
	drawGlobalLine(window, gcp, scale);
	drawMedianLine(window, animPoints, median);
	drawClosestLine(window, closestPair, scale);

	window.display();
	sf::sleep(sf::milliseconds(DELAY/2));

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

	if (closestPair.distance < gcp.distance) {
		gcp = closestPair;
	}

	int stripLen = closestPair.distance * 2 * scale;
	int stripPosX = 400 + (vec[median].x * scale);

	window.clear(sf::Color::Color(0, 8, 16, 255));

	drawPoints(window, animPoints);
	drawGlobalLine(window, gcp, scale);
	drawClosestLine(window, closestPair, scale);
	drawMedianLine(window, animPoints, median);
	drawStrip(window, stripLen, stripPosX);

	window.display();
	sf::sleep(sf::milliseconds(DELAY));

	return closestPair;

}

Result dacRecursive(const vector<Point>& vec, int lo, int hi, sf::RenderWindow& window, const vector<sf::Vector2f>& animPoints, double scale, Result& gcp) {

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

	drawPoints(window, animPoints);
	drawGlobalLine(window, gcp, scale);
	drawMedianLine(window, animPoints, median);

	window.display();
	sf::sleep(sf::milliseconds(DELAY/2));

	Result left = dacRecursive(vec, lo, median, window, animPoints, scale, gcp);
	Result right = dacRecursive(vec, median + 1, hi, window, animPoints, scale, gcp);
	return dacMerge(vec, lo, median, hi, left, right, window, scale, animPoints, gcp);

}

Result divideAndConquer(const vector<Point>& unsortedVec, sf::RenderWindow& window, vector<sf::Vector2f>& animPoints, double scale) {

	Result globalClosestPair;
	vector<Point> vec = unsortedVec;

	sort(vec.begin(), vec.end(), [](const Point& a, const Point& b) {
		return a.x < b.x;
		});

	sort(animPoints.begin(), animPoints.end(), [](const sf::Vector2f& a, const sf::Vector2f& b) {
		return a.x < b.x;
		});

	return dacRecursive(vec, 0, (vec.size() - 1), window, animPoints, scale, globalClosestPair);

}

int main() {

	string line;
	double x, y;
	vector<Point> points;
	ifstream pointFile("points_.txt");

	sf::RenderWindow window(sf::VideoMode(800, 800), "Closest Pairs");

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

	vector<sf::Vector2f> animPoints;
	for (auto& p : points) {
		double scaledX = (p.x) * scale;
		double scaledY = (p.y) * scale;
		animPoints.emplace_back(400 + scaledX, 400 - scaledY);
	}

	Result result = ALGORITHM(points, window, animPoints, scale);

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