#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>

// Initialize the starting population
std::vector<int> initPop(std::vector<int> population, std::mt19937 &randGen, size_t &popSize, size_t &numLocs) {
	population.clear();
	population.resize(numLocs);
	std::iota(population.begin(), population.end(), 0);
	std::shuffle(population.begin() + 1, population.end(), randGen);
	return population;
}

// Convert a string of text into a Location type
Location convertCoords(const std::string& line) {
    size_t commaFirst = line.find(',');
	
    if (commaFirst != std::string::npos) {
        size_t commaSecond = line.find(',', commaFirst + 1);
		std::string nameStr = line.substr(0, commaFirst);

        if (commaSecond != std::string::npos) {
            std::string latitudeStr = line.substr(commaFirst + 1, commaSecond - commaFirst - 1);
            std::string longitudeStr = line.substr(commaSecond + 1);

            double latitude = std::stod(latitudeStr);
            double longitude = std::stod(longitudeStr);

			Location temp;
			temp.mName = nameStr;
			temp.mLatitude = latitude;
			temp.mLongitude = longitude;

            return temp;
        }
    }
	Location temp;
	temp.mName = nullptr;
	temp.mLatitude = 0.0;
	temp.mLongitude = 0.0;
    return temp;
}

// Haversine distance formula to compute the distance of each segment on the path
// Adapted from Lecture slides
double distance(const std::pair<double, double>& point1, const std::pair<double, double>& point2) {
    // Convert latitude and longitude to radians
    double lat1 = point1.first * 0.0174533;
    double lon1 = point1.second * 0.0174533;
    double lat2 = point2.first * 0.0174533;
    double lon2 = point2.second * 0.0174533;

    // Haversine calculation
    double distLon = lon2 - lon1;
    double distLat = lat2 - lat1;
    double a = std::sin(distLat / 2) * std::sin(distLat / 2) + std::cos(lat1) * std::cos(lat2) * std::sin(distLon / 2) * std::sin(distLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return 3961 * c;
}

// Once we have the segment distances, sum them together for the total tour distance
double tourDistances(const std::vector<std::pair<double, double>>& path) {
    std::vector<std::pair<double, double>> pathDistances;
	// Compute the distance between two locations in a path
	// Store the result in pathDistances (back_inserter(pathDistances) would not compile unless pathDistances was the same type as path)
    std::adjacent_difference(path.begin(), path.end(), std::back_inserter(pathDistances), [](const auto& point1, const auto& point2) -> std::pair<double, double> {
        double dist = distance(point2, point1);
        return std::make_pair(dist, dist);
    });

	// Filter out path as a vector of doubles
	std::vector<double> distances;

	// Reducing pathDistances to a vector<double> (allowed to use loops - side effect of std::adjacent_difference)
	for (const std::pair<double, double>& pair : pathDistances) {
		// pair.first == pair.second, so pick either option
		distances.push_back(pair.first);
	}
	// Double variable to store the total distance/fitness of path
	double totalDist = 0.0;

	// Start at distances.begin() + 1, so we go from 0th -> 1st -> 2nd... locations
	totalDist = std::accumulate(distances.begin() + 1, distances.end(), 0.0);

    // Return total distance/fitness of path
    return totalDist;
}

// Calculating the running sum (allowed to use loops)
size_t select(const std::vector<double>& probs, double randomValue) {
    double runningSum = 0.0;
    for (size_t i = 0; i < probs.size(); ++i) {
        runningSum += probs[i];
        if (randomValue < runningSum) {
            return i; // Return the selected individual's index
        }
    }
    return probs.size() - 1; // Return the last individual if not selected
}

// Function to handle crossover approach
std::vector<int> cross(const std::vector<int>& parentA, const std::vector<int>& parentB, double& mutationChance, std::mt19937& randGen) {
    std::uniform_int_distribution<int> crossoverIndexDist(1, parentA.size() - 2);
    int crossoverIndex = crossoverIndexDist(randGen);

    std::uniform_int_distribution<int> parentOrderDist(0, 1);
    int parentOrder = parentOrderDist(randGen);

    std::vector<int> child(parentA.size());

    if (parentOrder == 1) {
        // Parent A
        std::copy_n(parentA.begin(), crossoverIndex + 1, child.begin());

        std::copy_if(parentB.begin(), parentB.end(), child.begin() + crossoverIndex + 1, [&child](int location) {
            return std::find(child.begin(), child.end(), location) == child.end();
        });
    } else {
        // Parent B
        std::copy_n(parentB.begin(), crossoverIndex + 1, child.begin());

        std::copy_if(parentA.begin(), parentA.end(), child.begin() + crossoverIndex + 1, [&child](int location) {
            return std::find(child.begin(), child.end(), location) == child.end();
        });
    }

    std::uniform_real_distribution<double> mutationChanceDist(0.0, 1.0);
    double mutationRoll = mutationChanceDist(randGen);

    if (mutationRoll <= mutationChance) {
        std::uniform_int_distribution<int> mutationIndexDist(1, child.size() - 1);
        int index1 = mutationIndexDist(randGen);
        int index2 = mutationIndexDist(randGen);

        std::swap(child[index1], child[index2]);
    }
    return child;
}