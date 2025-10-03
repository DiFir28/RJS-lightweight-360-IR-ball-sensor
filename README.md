# RJS-lightweight-360-IR-ball-sensor

<img width="401" height="300" alt="General view" src="https://github.com/user-attachments/assets/930d4bf8-b608-4607-9177-4990a8d32c3b" />
<img width="300" height="300" alt="Top view" src="https://github.com/user-attachments/assets/790fdb9e-d7a9-49ff-a266-bea45dbccb4f" />


## What is it for?

The RJS lightweight rules require the position of the ball emitting IR waves to be determined. 
>The ball's main emission period is 833 microseconds.

<img width="400" height="300" alt="Top view" src="https://github.com/robocup-junior/ir-golf-ball/blob/main/img%2FIR_ball_modulation.png" />

Learn more about [IR ball](https://github.com/robocup-junior/ir-golf-ball).
## Hardware solution
The sensor is a custom PCB. We use as main brain atmega2560. 24 TSSP4038 sensors are located around the circle to read the IR signal.

<img width="300" height="300" alt="TSSP4038" src="https://github.com/user-attachments/assets/c88ed9de-1e67-4d41-8ddb-65a1ec8a9bed" />

## Software solution

Due to the time limit of the ball study period and the need for uniform reading of the circle, at 833 microseconds, data is collected by bytes of pin registers and split into specific pins in a separate cycle. Vectors are constructed based on the sensors that receive the IR signal, with the beginning in the center of the board and the end in each sensor. We calculate a vector equal to the sum of all vectors constructed before the sensors, taking into account the vector on the sensor as many times as the IR signal was detected . The desired angle to the ball will be the angle of the final vector.
The sensor readings are transmitted via the I2C protocol.

