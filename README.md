# SPH Fluid Simulation (C++ / SDL2)
A high-performance, multithreaded fluid simulation focusing on optimization and physics stability.

![Fluid Simulation Demo](./demo.gif)

## Project Overview
Fluid dynamics is famously difficult for computers to process. While the Navier-Stokes equations are the standard, they are a massive "mathematical headache" to solve directly. This project implements **Smoothed Particle Hydrodynamics (SPH)** to simulate fluid behavior by treating the water as a collection of interacting particles that push and pull each other to reach a "rest density."

## Core Features & Optimizations

### 1. SPH (The Heart of the Program)
* **Averaging & Kernels**: Instead of solving complex grids, each particle calculates its properties (density, pressure, etc.) based on an average of its neighbors using a kernel weight function.
* **Physical Forces**: Particles simulate pressure (pushing away) and viscosity (resisting flow) to look like real water.

### 2. Multithreaded Thread Pool
* **Parallel Processing**: Running thousands of particles on one thread is slow. I implemented a custom **Thread Pool** to split the workload across $n$ threads (default: 8).
* **Workload Division**: Each thread is responsible for a specific range of particles. 
    * *Example*: 1,980 particles / 8 threads $\approx$ 247 particles per thread.
* **Safety**: Uses `std::atomic` and mutex locks to ensure threads don't "race" each other when updating the simulation.

### 3. Spatial Hash Grid
* **Neighbor Searching**: Normally, finding nearby particles takes $O(n^2)$ time. This is terrible for performance.
* **The Optimization**: I split the world into a grid (chunks). By hashing particle positions into grid cells, we only check particles in the immediate surrounding cells, bringing the complexity down to roughly $O(n)$.

### 4. Mathematical Pre-computation
* **Kernel Constants**: Values like $h^7$ or $\frac{315}{64\pi h^9}$ are pre-calculated to save CPU cycles during the main loop.
* **Fast Inverse Square Root**: I implemented the famous **Quake III Fast Inverse Square Root** function to make vector normalization significantly faster than using standard `sqrt()`.

### 5. Fixed Time-Step Accumulator
* **Stability**: Prevents the physics from "exploding" if the frame rate drops.
* **Temporal Decoupling**: Guarantees that the simulation time stays synchronized with real-world time. If a frame takes 10ms but our physics step is 2ms, the program runs the simulation 5 times before rendering once.

## Why I Built This
* **Interest**: Fluid physics is fascinating.
* **Learning**: I wanted to master C++ memory management, multithreading, and low-level optimizations.
* **Cool Factor**: Watching 2,000 particles splash in real-time is satisfying.

## Dependencies
* **SDL2**: Used for window management and pixel rendering.

## Getting Started
1.  **Clone the repo**: `git clone https://github.com/kriangkrai2549/SPH-2D-Simulator.git`
2.  **Install SDL2**: Make sure the library is in your system path.
3.  **Build**: Run the build script in your terminal:
    ```bash
    bash build.sh
    ```
4.  **Run**:
    ```bash
    bash run.sh
    ```

## Technical References
* [SCA03: Particle-Based Fluid Simulation](https://matthias-research.github.io/pages/publications/sca03.pdf) — The "Bible" of SPH fluid.
* [The GOAT Video](https://youtu.be/rSKMYc1CQHE) — Excellent visual explanation by Sebastian Lague.