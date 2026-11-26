library(irace)

script_arg <- grep("^--file=", commandArgs(), value = TRUE)
calibration_dir <- if (length(script_arg)) {
    dirname(normalizePath(sub("^--file=", "", script_arg[[1]]), mustWork = TRUE))
} else {
    getwd()
}
results_dir <- file.path(calibration_dir, "recalibracao")
dir.create(results_dir, showWarnings = FALSE)

scenario <- defaultScenario()
scenario$parameterFile <- file.path(calibration_dir, "parameters.txt")
scenario$trainInstancesFile <- file.path(calibration_dir, "instances.txt")
scenario$execDir <- results_dir
scenario$logFile <- file.path(results_dir, "irace.Rdata")

domains <- readParameters(file = scenario$parameterFile)$domains
grid <- expand.grid(domains, stringsAsFactors = FALSE)
scenario$configurationsFile <- file.path(results_dir, "grade.txt")
write.table(grid, scenario$configurationsFile, row.names = FALSE, quote = FALSE)

instances <- scan(scenario$trainInstancesFile, what = integer(), quiet = TRUE)
# irace budgets an additional margin per candidate before it starts the race.
scenario$maxExperiments <- 1000L
scenario$nbIterations <- 1L
scenario$nbConfigurations <- nrow(grid)
scenario$firstTest <- length(instances)
scenario$deterministic <- TRUE
scenario$sampleInstances <- FALSE
scenario$parallel <- 6L
scenario$seed <- 12345L

# The C++ reader resolves instance paths relative to src/. Run the same VND variant as AG-RLP.
scenario$targetRunner <- function(experiment, scenario) {
    source_dir <- dirname(dirname(scenario$parameterFile))
    executable <- if (.Platform$OS.type == "windows") "exec_irace_ag.exe" else "exec_irace_ag"
    previous_dir <- setwd(source_dir)
    on.exit(setwd(previous_dir))

    params <- experiment$configuration
    args <- c("--instance", experiment$instance, "--seed", experiment$seed,
              "--metodo", "12", "--pop", params[["tam_pop"]], "--alpha", params[["alpha"]])
    output <- suppressWarnings(system2(file.path(source_dir, executable), args, stdout = TRUE, stderr = TRUE))
    if (!is.null(attr(output, "status")) || length(output) != 1L || !grepl("^[0-9]+$", trimws(output))) {
        return(list(error = paste(c("AG target failed", output), collapse = "\n")))
    }
    list(cost = as.numeric(output))
}

elite <- irace(scenario = scenario)
write.csv(elite, file.path(results_dir, "elite.csv"), row.names = FALSE)

archive <- new.env()
load(scenario$logFile, envir = archive)
log <- archive$iraceResults
costs <- log$experiments
observed <- which(!is.na(costs), arr.ind = TRUE)
instance_index <- as.integer(rownames(costs)[observed[, "row"]])
configuration <- as.integer(colnames(costs)[observed[, "col"]])
instance_log <- log$state$instances_log
configs <- log$allConfigurations
config_index <- match(configuration, configs$.ID.)
evaluation <- data.frame(
    configuracao = configuration,
    mina = instances[instance_log$instanceID[instance_index]],
    semente = instance_log$seed[instance_index],
    tam_pop = configs$tam_pop[config_index],
    alpha = configs$alpha[config_index],
    fo = costs[observed]
)
stopifnot(nrow(evaluation) == nrow(grid) * length(instances),
          !anyNA(evaluation),
          !anyDuplicated(evaluation[c("configuracao", "mina")]))
write.csv(evaluation, file.path(results_dir, "avaliacoes.csv"), row.names = FALSE)
