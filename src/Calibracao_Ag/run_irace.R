# Load the irace package (iterated racing for automatic algorithm configuration).
library(irace)

# Create the default irace scenario; every setting not assigned below keeps irace's default.
# Create the default scenario structure
scenario <- defaultScenario()

# Correct file and limit settings
# Program that irace runs for each candidate configuration, instance and seed.
scenario$targetRunner <- "./target-runner"
scenario$trainInstancesFile <- "instances.txt"
scenario$parameterFile <- "parameters.txt"
# Budget: at most 1000 target-runner calls (experiments) for the whole tuning.
scenario$maxExperiments <- 1000

# Run up to 6 experiments in parallel.
scenario$parallel <- 6

# Run the tuning; irace returns the elite configurations it found.
# Run the tuning
irace(scenario = scenario)