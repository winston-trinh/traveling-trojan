/**
 * Author: Winston Trinh, October 2023
 * File: SrcMain.cpp
 */

#include "SrcMain.h"
#include <iostream>
#include <random>
#include "TSP.h"
#include <fstream>
#include <algorithm>

void ProcessCommandArgs(int argc, const char* argv[])
{
	// Check if command-line arguments are valid
    if (argc < 6) {
		std::cout << "Please provide input in this format: <inputfile> <popsize> <generations> <mutationchance> <seed>\n";
        return;
    }

	// Part 1
	// Create an input file stream for the locations input file
	// Pass in the name of the locations input file
	std::ifstream locationsFile(argv[1]);

	// If unable to open "inputfile", print error message
	if (!locationsFile.is_open()) {
		std::cout << "Failed to open the locations file: " << argv[1] << '\n';
		return;
	}

	size_t popSize = std::stoi(argv[2]);
	size_t generations = std::stoi(argv[3]);
	size_t mutationChance = std::stoi(argv[4]);
	size_t seed = std::stoi(argv[5]);
	
	// If we reached here, then we were able to open the file
	// Temp variable to store each line from locationsFile
	size_t numLocs = 0;
	std::string word;
	std::vector<std::string> locs;

	// Parsing strings from file input (allowed to use loops - file I/O)
	// Loop continues reading input file until there are no more lines
	while (std::getline(locationsFile, word)) {
		locs.push_back(word);
		++numLocs;
	}

	// Close input file when finished
	locationsFile.close();

	// This seed was used for testing, but you can omit for random results
	std::mt19937 randGen(seed);

	// Create output file stream to output generation info
	std::ofstream output("log.txt");

	// Initialize output file stream for log.txt
	if (!output.is_open()) {
		std::cout << "Failed to open the output file: log.txt" << '\n';
		return;
	}
	std::vector<int> population(numLocs);
	output << "INITIAL POPULATION:\n";
	Population startPop;

	// Generate starting population (allowed to use loops for each generation)
	for (size_t numPop = 0; numPop < popSize; ++numPop) {
		std::vector<int> temp = initPop(population, randGen, popSize, numLocs);
		size_t num = 0;
		for (size_t pop : temp) {
			if (num == numLocs - 1) {
				output << pop;
				break;
			}
			output << pop << ',';
			++num;
		}
		output << '\n';
		// Add a zero to temp to account for the return trip
		// back to the starting location
		temp.push_back(0);
		startPop.mMembers.push_back(temp);
	}
	// Vector storing pair of coordinates for all locations by index
	std::vector<Location> coords;

	// Parsing strings from file input (allowed to use loops - file I/O)
	for (const std::string& loc : locs) {
		Location coord = convertCoords(loc);
		coords.push_back(coord);
	}
	Population prevPop = startPop;
	Population currPop;

	// Main loop that runs X iterations for n generations (allowed to use loops)
	for (size_t generation = 0; generation < generations; ++generation) {
		// Temp counter variable
		size_t idx = 0;

		// Vector storing starting population with each location converted to
		// their respective coordinates
		std::vector<std::vector<std::pair<double, double>>> popCoords;

		// Parsing strings from file input (allowed to use loops - file I/O)
		for (size_t numPop = 0; numPop < popSize; ++numPop) {
			std::vector<std::pair<double, double>> temp;
			for (size_t num = 0; num < prevPop.mMembers.at(numPop).size(); ++num) {
				idx = prevPop.mMembers.at(numPop).at(num);
				temp.emplace_back(std::make_pair(coords.at(idx).mLatitude, coords.at(idx).mLongitude));
			}
			popCoords.push_back(temp);
		}

		// Pop return location from prevPop
		for (size_t idx = 0; idx < popSize; ++idx) {
			prevPop.mMembers.at(idx).pop_back();
		}

		// Part 2
		// fitVals stores the fitness values for each path in the population
		std::vector<std::pair<size_t, double>> fitVals;

		idx = 0;
		// For each path in the population, compute their total distance/fitness
		// Store results in fitVals
		std::for_each(popCoords.begin(), popCoords.end(), [&](const auto& path) {
			double dist = tourDistances(path);
			fitVals.emplace_back(std::make_pair(idx, dist));
			++idx;
		});

		output << "FITNESS:\n";
		// Output results (allowed to use loops - file I/O)
		for (const std::pair<size_t, double>& pair : fitVals) {
			output << pair.first << ':' << pair.second << '\n';
		}

		// Sort fitness vector by fitness in ascending order
		std::sort(fitVals.begin(), fitVals.end(), [](const std::pair<size_t, double>& val1, const std::pair<size_t, double>& val2) {
			return val1.second < val2.second;
		});

		std::vector<double> probs(popSize);

		// Initialize probabilities
		std::generate(probs.begin(), probs.end(), [popSize = probs.size()]() {
			return 1.0 / static_cast<double>(popSize);
		});


		// Adjust probabilities for the top two individuals
		probs[fitVals.at(0).first] *= 6.0;
		probs[fitVals.at(1).first] *= 6.0;

		// Adjust probabilities for the remaining top half
		// (allowed to use loops - “The remaining top half” multiplied by 3x)
		for (size_t i = 2; i < popSize / 2; ++i) {
			probs[fitVals.at(i).first] *= 3.0;
		}

		// Calculate the sum of probabilities
		double totalProbability = std::accumulate(probs.begin(), probs.end(), 0.0);

		// Renormalize the probabilities
		std::transform(probs.begin(), probs.end(), probs.begin(), [totalProbability](double prob) {
			return prob / totalProbability;
		});

		// Construct a uniform distribution for random double values in [0, 1.0]
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		// Vector storing parents
		std::vector<std::pair<int, int>> parents;

		// Select parents (allowed to use loops for each generation / file I/O)
		for (size_t i = 0; i < popSize; ++i) {
			// Generate random values for selecting two parents
			double rand1 = dist(randGen);
			double rand2 = dist(randGen);

			// Select parents based on their random values and probability vector
			int parent1 = select(probs, rand1);
			int parent2 = select(probs, rand2);

			// Use parent1 and parent2 for further processing
			std::pair<int, int> newCoup = std::make_pair(parent1, parent2);
			parents.push_back(newCoup);
		}

		output << "SELECTED PAIRS:\n";
		// Output selection results (allowed to use loops - file I/O)
		for (const std::pair<int, int>& pair : parents) {
			output << '(' << pair.first << ',' << pair.second << ")\n";
		}

		// Part 3
		currPop.mMembers = {};
		double mutChance = mutationChance / 100.0;
		output << "GENERATION: " << generation + 1 << '\n';

		// Output population/generation (allowed to use loops - file I/O)
		for (size_t idx = 0; idx < parents.size(); ++idx) {
			std::vector<int> child = cross(prevPop.mMembers.at(parents.at(idx).first), prevPop.mMembers.at(parents.at(idx).second), mutChance, randGen);
			for (size_t i = 0; i < child.size(); ++i) {
				if (i == child.size() - 1) {
					output << child.at(i);
					break;
				}
				output << child.at(i) << ',';
			}
			child.push_back(0);
			currPop.mMembers.push_back(child);
			output << '\n';
		}
		prevPop = currPop;

		// If we're on the final generation, output it
		if (generation == generations - 1) {
			std::vector<std::vector<std::pair<double, double>>> optimalCoords;

			// Parsing strings from file input (allowed to use loops - file I/O)
			for (size_t numPop = 0; numPop < popSize; ++numPop) {
				std::vector<std::pair<double, double>> temp;
				for (size_t num = 0; num < prevPop.mMembers.at(numPop).size(); ++num) {
					idx = prevPop.mMembers.at(numPop).at(num);
					temp.emplace_back(std::make_pair(coords.at(idx).mLatitude, coords.at(idx).mLongitude));
				}
				optimalCoords.push_back(temp);
			}

			for (size_t idx = 0; idx < popSize; ++idx) {
				prevPop.mMembers.at(idx).pop_back();
			}
			// optimal stores the fitness values for each path in the final generation
			std::vector<std::pair<size_t, double>> optimal;

			idx = 0;
			// For each path in the population, compute their total distance/fitness
			// Store results in fitVals
			std::for_each(optimalCoords.begin(), optimalCoords.end(), [&](const auto& path) {
				double dist = tourDistances(path);
				optimal.emplace_back(std::make_pair(idx, dist));
				++idx;
			});

			output << "FITNESS:\n";
			// Output results (allowed to use loops - file I/O)
			for (const std::pair<size_t, double>& pair : optimal) {
				output << pair.first << ':' << pair.second << '\n';
			}

			// Sort fitness vector by fitness in ascending order
			std::sort(optimal.begin(), optimal.end(), [](const std::pair<size_t, double>& val1, const std::pair<size_t, double>& val2) {
				return val1.second < val2.second;
			});

			output << "SOLUTION:\n";
			/* For the final path output, compute/display the fitness of the last generation
			* and select the member with the highest fitness as the solution. Then just output
			* the path (including the return to location 0 at the end), with the distance of
			* the path, as in the reference file.
			*/
			// Output locations of optimal path (allowed to use loops - file I/O)
			for (size_t location = 0; location < prevPop.mMembers.at(optimal.at(0).first).size(); ++location) {
				// Display the locations of the optimal path from the last generation
				output << coords.at(prevPop.mMembers.at(optimal.at(0).first).at(location)).mName << '\n';
			}
			// Display the return/ending location again
			output << coords.at(prevPop.mMembers.at(optimal.at(0).first).at(0)).mName << '\n';

			// Display the distance of the optimal path from the last generation
			output << "DISTANCE: " << optimal.at(0).second << " miles" << '\n';
		}
	}
	// Close output file stream
	output.close();
}
