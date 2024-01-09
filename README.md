# Traveling Trojan #

### Welcome!
This project showcases a genetic algorithm designed to optimize tours of landmarks in Los Angeles, starting and ending at LAX. I developed this project to demonstrate my understanding of functional programming in C++, adhering strictly to rules that emphasize the use of higher-order functions with minimal side effects.

### Purpose
Traveling Trojan is a tour simulator that applies genetic algorithms to solve the Traveling Salesman Problem (TSP) in a novel and efficient way. Its primary goal is to find the best possible route visiting all landmarks with the least total distance.

## How It Works

#### Guidelines That I Followed
##### No Custom Classes
In line with the functional style, no custom classes are declared. Only built-in classes are used, ensuring simplicity and effectiveness.

##### Side Effects and Iteration
The program minimizes side effects and iteration, utilizing higher-order functions like `std::transform`, `std::accumulate`, and `std::generate` wherever possible. Loops are reserved for file I/O, the main generational loop, and a few specific calculations.

### Initial Output and File I/O
The program outputs its initial population to a `log.txt` file, following a specific format for easy understanding and comparison.

### Fitness and Selection
Fitness is calculated using the Haversine formula:  

    dlon = lon2 - lon1

    dlat = lat2 - lat1

    a = (sin(dlat/2))2 + cos(lat1) * cos(lat2) * (sin(dlon/2))2

    c = 2 * atan2( sqrt(a), sqrt(1-a) )

    distance = 3961 * c

Selection is based on fitness scores. The program employs a unique probability vector and selection algorithm to choose parent pairs for crossover.

### Crossover and Generations
The crossover function follows steps outlined below to create new generations.

1. Generate a random crossover index value from [1, size – 2]
2. “Flip a coin” to decide which parent should go first
3. Selected first parent will copy all elements from beginning up to
and including crossover index into child
4. Second parent will start at the beginning, and copy over all
elements that don’t already appear in the child
5. Mutate, based on probability

Mutation is incorporated with a specified chance, contributing to the evolution of solutions.

1. There’s a chance a mutation occurs when creating a child
2. If we mutate, pick two random indices (not including index 0), and
swap the values

### Final Output
After a specified number of generations, the program selects the best route based on fitness and outputs the final path, including the return to LAX, along with the total distance.

## Where to Start
To test this program, ensure that your system meets the prerequisites and follow the setup instructions:

- **Mac Setup**: [Mac Setup Guide](https://itp435-20241.github.io/SetupMac.html)
- **PC Setup**: [PC Setup Guide](https://itp435-20241.github.io/SetupPC.html)

The program is executed via command line with specific parameters, including input file, population size, number of generations, mutation chance, and seed.

### Explore the Code
I encourage you to explore the `/src` folder, particularly the `SrcMain.cpp` and `TSP.cpp`, to see the implementation details and my functional programming approach.

#### Sample Outputs
Sample outputs can be accessed with the path  
`/sampleoutput/(your_platform)/(sample_output_example)`   
  
For example, I use a Mac with Clang, so to access the sample outputs I would use the path:  
`/sampleoutput/macclang/log_8_5_10_1337.txt`  
  
Windows:  
`/sampleoutput/win/log_8_5_10_1337.txt` 

#### Running Tests
The test file can be accessed with the path  
`/tests/StudentTests.cpp`  
  
Uncomment the tests from lines `41` to `110`.

#### Input files
The input files can be accessed with the path  
`/input/locations.txt` (main file used for tests)  
or  
`/input/locations2.txt`  
  

Please message me if you would like to talk or share your thoughts. Thank you for taking the time to explore my project!

Contact: [wntrinh@usc.edu](mailto:wntrinh@usc.edu)

### Further Improvements
In the future, I want to add random probabilities of cataclysmic events that wipe out a percentage of the population.

Additionally, I want to make this program accessible to more users. To do this, I plan to:
1. Add a front-end (by turning this into a web-app or iOS app)
    - This can allow users can enter a list of locations
2. Implement an API, such as Google Maps, to query for coordinates from locations provided by user inputs
