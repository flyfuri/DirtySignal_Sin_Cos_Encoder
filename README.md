# DirtySignal_Sin_Cos_Encoder

### What it is used for:
This library is used in and was developed for my project [CondorRudderPedal_ESP8622](https://github.com/flyfuri/CondorRudderPedal_ESP8622). Therefore, I refer to that project for hardware documentation. This library contains classes, used to decode two sinusoidal signals with a phase shift of roughly 90°, coming from a cheap self-built sine-cosine-encoder. Although the precision of the measurement depends on the quality of the 2 signals provided, efforts were made to be able to tolerate dirtier signals where amplitudes differ to a certain amount and phase offset isn't exactly 90°. This "signal enhancing" functionality has become a major part of the project. I wanted to see how dirty a signal can be while still beeing able to get some useful position reading.
The basic principle is already working, but I am still trying new approaches to make it cope with dirtier signals. 

### How it works:
An encoder works roughly like this:
#### AutoscalerPair:
- The 2 signals first gets processed by the AutoScalerPair class which automatically scales the amplitudes of both signals to a common size and puts them on the same level. 
![Alt Text](/docs/pics/scaling.jpg?raw=true "scaling")

#### SinusoidIncCounter:
- From channel A and B a "summary-" and a "difference-channel" is calculated.
- Tooth counting: each time the "difference-channel" crosses 0, the two signals A and B are crossing. That's where the teeth were counted (actually each half-tooth). 
- To get a higher resolution, interpolation is done between the half-teeth using the "summary-channel". This interpolation is done for each half-tooth individually by taking the averages of the last five measurements for each peak and valley.
![Alt Text](/docs/pics/interpolation.jpg?raw=true "interpolation")

### Libraries used: 
The library [TimerAndFilter](https://github.com/flyfuri/TimerAndFilter). Both libraries were originally developed for the example project below.

### Example: 
[CondorRudderPedal_ESP8622](https://github.com/flyfuri/CondorRudderPedal_ESP8622)


