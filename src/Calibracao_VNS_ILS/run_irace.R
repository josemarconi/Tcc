# Load the irace package (iterated racing for automatic algorithm configuration).
library(irace)

# Create the default irace scenario; every setting not assigned below keeps irace's default.
# 1. Configure the irace scenario
scenario <- defaultScenario()
# Program that irace runs for each candidate configuration, instance and seed.
scenario$targetRunner       <- "./target-runner"
scenario$parameterFile      <- "parameters.txt"
scenario$trainInstancesFile <- "instances.txt"  # Changed from instancesFile to trainInstancesFile
# Budget: at most 200 target-runner calls (experiments) for the whole tuning.
scenario$maxExperiments     <- 200
# Record the tuning results in this file, which can be reloaded later.
scenario$logFile            <- "irace-output.Rdata"

# Run the tuning; irace returns the elite configurations it found.
# 2. Run the tuning
results <- irace(scenario = scenario)

# Show the best configurations found at the end of the tuning.
# 3. Show the best configuration found at the end
cat("\n--- Melhor Configuração Encontrada ---\n")
# Extract the best configurations from the irace result object.
best_conf <- getBestConfigurations(results)
print(best_conf)