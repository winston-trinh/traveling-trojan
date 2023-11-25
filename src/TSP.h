#pragma once
#include <string>
#include <vector>
#include <random>

struct Location
{
	std::string mName;
	double mLatitude = 0.0;
	double mLongitude = 0.0;
};

struct Population
{
	std::vector<std::vector<int>> mMembers;
};

// Initialize the starting population
std::vector<int> initPop(std::vector<int> population, std::mt19937 &randGen, size_t &popSize, size_t &numLocs);

// Convert a string of text into a Location type
Location convertCoords(const std::string& line);

// Haversine distance formula to compute the distance of each segment on the path
// Adapted from Lecture slides
double distance(const std::pair<double, double>& point1, const std::pair<double, double>& point2);

// Once we have the segment distances, sum them together for the total tour distance
double tourDistances(const std::vector<std::pair<double, double>>& path);

// Calculating the running sum (allowed to use loops)
size_t select(const std::vector<double>& probs, double randomValue);

// Function to handle crossover approach
std::vector<int> cross(const std::vector<int>& parentA, const std::vector<int>& parentB, double& mutationChance, std::mt19937& randGen);