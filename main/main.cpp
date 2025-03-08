#include <limits> // Required for std::numeric_limits<double>::infinity()
#include "TopModel.hpp"  // Top-level coupled model header
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;
using namespace std;

int main(){
    // Instantiate the top-level coupled model.
    auto model = std::make_shared<TopModel>("MarketImpactTopModel");
    
    // Create the RootCoordinator with the top model.
    auto rootCoordinator = RootCoordinator(model);
    
    // Set a logger: Uncomment STDOUTLogger for console output or use CSVLogger for CSV logging.
    // rootCoordinator.setLogger<STDOUTLogger>(";");
    rootCoordinator.setLogger<CSVLogger>("simulation_log.csv", ";");
    
    // Initialize the simulation.
    rootCoordinator.start();
    
    // Run the simulation for # of seconds.
    rootCoordinator.simulate(100.0);
    
    // Stop the simulation and clean up.
    rootCoordinator.stop();
    
    return 0;
}
