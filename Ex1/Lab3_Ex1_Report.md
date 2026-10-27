# Lab 3 - Exercise 1: FSM design

The proposed traffic-light controller uses two coordinated finite-state machines (FSMs). Figure `Lab3_Ex1_FSM.png` shows the four-mode user interface and the normal traffic-light sequence.

The mode FSM starts in Mode 1 (normal operation). Each debounced press of the MODE button advances the state through Mode 2 (edit red duration), Mode 3 (edit amber duration), Mode 4 (edit green duration), and back to Mode 1. In an edit mode, a debounced INC press increments a temporary duration from 1 to 99, wrapping from 99 to 1. A debounced SET press stores the value and leaves the controller in the current edit mode. The selected color's four LEDs blink at 2 Hz. Two seven-segment digits display the mode number and the other two display the temporary duration.

The traffic FSM runs only in Mode 1. It cycles through four states: Road A green/Road B red, Road A amber/Road B red, Road A red/Road B green, and Road A red/Road B amber. A phase ends when its countdown reaches zero. Road A represents the north-south direction and Road B the east-west direction. Both directions are never green simultaneously. The four seven-segment displays show the two roads' countdowns in Mode 1.

The implementation must validate saved durations so the opposing road stays red until the active road's green and amber phases finish. Button inputs are sampled and debounced before generating MODE, INC, or SET events, so one physical press causes one state transition or value change.
