Market Impact & Liquidity Risk Simulation
===========================================

This project simulates market impact and liquidity risk in a high-frequency trading 
environment using the DEVS formalism and the Cadmium simulator. It models three key 
components:
  - TraderAgent: Simulates algorithmic traders that randomly generate buy and sell orders 
    with variable quantities.
  - MatchingEngine: Processes incoming orders and updates the market price using random 
    fluctuations. It also accepts shock events that force a sudden price drop.
  - Regulator: Monitors market price updates and triggers regulatory intervention (halt/resume)
    when the price drops significantly.
  - ShockEvent: A component that injects a sudden shock (e.g. a -10 price change) at a 
    predetermined simulation time.

The system is structured hierarchically:
  1. TradingMatchingModule: Couples the TraderAgent, MatchingEngine, and ShockEvent. It 
     forwards regulatory signals to both the TraderAgent and the MatchingEngine.
  2. TopModel: Couples the TradingMatchingModule with the Regulator to complete the closed-
     loop system.

Project Structure
-----------------
```plaintext
MarketImpactSimulation/             # Root directory for the simulation project
├── build_sim.sh                    # Shell script to build the project.
│                                  # (Every run replaces the contents of build/ and bin/)
├── CMakeLists.txt                  # Top-level CMake configuration file.
├── README.md                       # Project documentation and build instructions.
└── main/                           # Main folder containing the simulation source code.
    ├── CMakeLists.txt              # CMake configuration for the simulation code.
    ├── include/                    # Header files for all atomic and coupled models.
    │   ├── TraderAgent.hpp         # Atomic model for the trader agent (generates orders).
    │   ├── MatchingEngine.hpp      # Atomic model for the matching engine (processes orders and updates market price).
    │   ├── Regulator.hpp           # Atomic model for the regulator (monitors market price and triggers intervention).
    │   ├── ShockEvent.hpp          # Atomic model for injecting external shock events (e.g., sudden price drop).
    │   ├── TradingMatchingModule.hpp  # Coupled model that integrates TraderAgent, MatchingEngine, and ShockEvent.
    │   └── TopModel.hpp            # Top-level coupled model that links TradingMatchingModule with Regulator.
    └── main.cpp                    # Main simulation driver file: instantiates TopModel and runs the simulation.

```

Dependencies
------------
  - Cadmium Simulator: This project uses Cadmium for DEVS modeling. Make sure that Cadmium 
    is installed and that the environment variable CADMIUM is set (e.g., pointing to the 
    Cadmium include directory).
  - C++ Compiler: A C++ compiler supporting C++11 or later.
  - CMake: Used to configure and generate the build system.
  - Git: To clone and manage the project repository.

Build Instructions
------------------
Every time you build the project, the contents of the build/ and bin/ directories will be replaced.
To build the project, open a terminal in the project root and run:

    source build_sim.sh

Make sure that the CADMIUM environment variable is set in your shell environment (for example, add
a line like the following to your shell profile if needed):

    export CADMIUM=/path/to/cadmium_v2/include

 If you don't have it installed you can follow the instructions here to download it:
    
    https://devssim.carleton.ca/manuals/developer/


Run the Simulation
------------------
After building the project, an executable will be created in the bin/ directory. To run the 
simulation, simply execute:

    ./bin/MarketImpactSimulation

The simulation output will be logged using the Cadmium CSV logger (or STDOUT logger if configured
in main.cpp). You can review the CSV log to analyze the evolution of the market state (liquidity, 
last trade price) and the regulator's intervention events.

Simulation Details
------------------
  - TraderAgent: Generates random orders (buy or sell) with a random quantity between 50 and 200 
    every second if not paused.
  - MatchingEngine: Processes orders by adjusting the market price with random fluctuations. For 
    sell orders, it applies a wider negative fluctuation; for buy orders, it applies a positive offset. 
    The engine also processes shock events to simulate sudden market crashes.
  - Regulator: Checks the market price every second. It triggers a halt if the current price drops 
    by at least 1 unit from the last observed price. Once a halt is triggered, it remains active for a 
    specified duration (5 seconds) before resuming normal market operation.
  - ShockEvent: Fires a shock event (e.g., a -10 price change) at 30 seconds into the simulation.

Experimentation
---------------
The simulation is designed to generate volatile market conditions. You can adjust the following 
parameters to modify the experimental conditions:
  - Random fluctuation ranges in the MatchingEngine (for both buy and sell orders).
  - Order quantity range in the TraderAgent.
  - Regulator's trigger threshold (e.g., change the required drop from 1 unit to a different value).
  - Shock event timing and magnitude in ShockEvent.

Notes
-----
  - Every time you run 'source build_sim.sh', the contents of build/ and bin/ will be replaced.
  - Ensure that the CADMIUM environment variable is set properly to compile and run the project.




Market Impact & Liquidity Risk Simulation (Top-Level Coupled Model)
├── Trading & Matching Module
│   ├── TraderAgent
│   └── MatchingEngine
├── Regulator
└── ShockEvent (NEW)

Interactions:
- TraderAgent → orders → MatchingEngine
- MatchingEngine → market_update/execution → TraderAgent
- MatchingEngine → market_update → Regulator
- Regulator → halt/resume → MatchingEngine & TraderAgent
- ShockEvent → shock events (price/liquidity impacts) → MatchingEngine


time=0; liquidity=1000; last_trade_price=50; halted=0
time=1; liquidity=812; last_trade_price=44.9795; halted=1 (market halt triggered)
time=6; liquidity=812; last_trade_price=44.9795; halted=0 (market resumes)