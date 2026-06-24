#ifndef __KALMAN_H
#define __KALMAN_H

typedef struct {
    float Q_angle; // Process noise variance for the accelerometer
    float Q_bias;  // Process noise variance for the gyro bias
    float R_measure; // Measurement noise variance
    float angle; // The angle calculated by the Kalman filter - part of the 2x1 state vector
    float bias; // The gyro bias calculated by the Kalman filter - part of the 2x1 state vector
    float rate; // Unbiased rate calculated by the Kalman filter
    float P[2][2]; // Error covariance matrix - This is a 2x2 matrix
} Kalman_t;

void Kalman_Init(Kalman_t *kalman);
float Kalman_GetAngle(Kalman_t *kalman, float newAngle, float newRate, float dt);

#endif
