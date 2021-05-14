#ifndef ROBOT_OUTPUT_H
#define ROBOT_OUTPUT_H

typedef struct robotOutput
{
    double sensor1;
    double sensor2;
    double sensor3;
    double sensor4;
    uint8_t leftEncoderSigA;
    uint8_t leftEncoderSigB;
    uint8_t rightEncoderSigA;
    uint8_t rightEncoderSigB;
} robotOutputStruct;

#endif // ROBOT_OUTPUT_H
