# Self-Stabilizing-Twin-Rotor-Control-System
Introduction

<img width="1600" height="1600" alt="image" src="https://github.com/user-attachments/assets/e55d2efe-a898-4452-885f-1303c9306515" />


Feedback control lies at the heart of every modern autonomous system. From satellite attitude determination and quadrotor flight control to rocket guidance and industrial process automation, the ability to continuously estimate a system's state and generate corrective control actions is fundamental to achieving stability in the presence of uncertainty and external disturbances.

The Self-Stabilizing Twin Rotor Control System is a real-time experimental platform developed to investigate the design, implementation, and evaluation of feedback controllers on an inherently unstable nonlinear system. Unlike purely simulated environments, the platform exposes the controller to practical imperfections such as sensor noise, actuator nonlinearities, structural vibrations, parameter uncertainty, sampling delays, and external disturbances—bridging the gap between theoretical control design and embedded implementation.

The dynamics of the twin rotor system closely resemble a single-axis attitude control problem in which differential thrust is employed to regulate angular motion. The control objective is straightforward yet fundamentally challenging: maintain equilibrium despite continuously varying disturbances while satisfying stringent transient and steady-state performance requirements. Achieving this objective requires careful consideration of controller design, state estimation, signal conditioning, and actuator dynamics.

This repository primarily focuses on the implementation and experimental validation of Proportional–Integral–Derivative (PID) control. Although often regarded as the most classical feedback controller, PID remains the dominant control strategy across industrial automation due to its simplicity, robustness, computational efficiency, and remarkable effectiveness when properly tuned. The project explores not only the implementation of discrete-time PID algorithms but also the practical engineering considerations that determine real-world performance, including derivative filtering, integral windup, actuator saturation, sampling frequency, sensor filtering, and controller tuning methodologies.

Beyond controller implementation, the project emphasizes understanding the underlying system dynamics through mathematical modeling, experimental characterization, and performance analysis. Time-domain metrics such as rise time, overshoot, settling time, steady-state error, and disturbance rejection are analyzed alongside the influence of controller gains on system stability and robustness. Rather than treating PID as a collection of tuning parameters, the objective is to develop an intuitive understanding of how feedback fundamentally shapes the dynamic behavior of physical systems.

The architecture has been intentionally designed to remain modular, allowing increasingly sophisticated control strategies to be integrated with minimal changes to the sensing and actuation pipeline. This enables the platform to evolve from classical control toward modern techniques including state-space control, optimal control, observers, adaptive control, and model predictive control while maintaining a common experimental framework for quantitative comparison.

Ultimately, this project serves as a practical control engineering laboratory where theoretical concepts are validated on real hardware. Every oscillation, disturbance, overshoot, and instability becomes measurable, analyzable, and, most importantly, controllable—providing an environment in which feedback control is not merely studied, but experienced.
