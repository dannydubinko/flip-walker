#include "MagnetometerCalibrator.hpp"

MagnetometerCalibrator::MagnetometerCalibrator(Magnetometer &mag)
    : m_mag(mag), m_rawBuffer(nullptr), m_angleBuffer(nullptr), m_numPoints(0)
{
}

void MagnetometerCalibrator::runInterativeCalibration(int numPoints)
{
    m_numPoints = numPoints;

    // Clean up old buffers if they exist
    if (m_rawBuffer)
        delete[] m_rawBuffer;
    if (m_angleBuffer)
        delete[] m_angleBuffer;

    m_rawBuffer = new float[m_numPoints];
    m_angleBuffer = new float[m_numPoints];

    Serial.println("\n--- FlipWalker Magnetometer Calibration ---");
    Serial.print("Preparing to calibrate ");
    Serial.print(m_numPoints);
    Serial.println(" points.");

    float startAngle = -90.0;
    float endAngle = 90.0;
    float step = (endAngle - startAngle) / (m_numPoints - 1);

    for (int i = 0; i < m_numPoints; i++)
    {
        float targetAngle = startAngle + (i * step);
        m_angleBuffer[i] = targetAngle;

        Serial.print("\n[Step ");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.print(m_numPoints);
        Serial.println("]");
        Serial.print(">> Move FlipWalker to exactly: ");
        Serial.print(targetAngle);
        Serial.println(" degrees.");
        Serial.println(">> Press ANY key in Serial Monitor to capture...");

        waitForUser();

        // Take an average of 50 readings to eliminate noise/jitter
        float sum = 0;
        const int samples = 50;
        for (int s = 0; s < samples; s++)
        {
            sum += m_mag.getRawHeading();
            delay(10);
        }

        m_rawBuffer[i] = sum / (float)samples;

        Serial.print("Captured Raw Value: ");
        Serial.println(m_rawBuffer[i]);
    }

    printCalibrationArrays();
}

void MagnetometerCalibrator::waitForUser()
{
    while (Serial.available() > 0)
        Serial.read(); // Flush existing
    while (Serial.available() == 0)
        delay(10); // Wait for new
    while (Serial.available() > 0)
        Serial.read(); // Flush trigger
}

void MagnetometerCalibrator::printCalibrationArrays()
{
    Serial.println("\n--- CALIBRATION COMPLETE ---");
    Serial.println("Copy and paste these arrays into your main script:\n");

    // Print Physical Angles (reversed: 90 to -90 for params)
    Serial.print("float physicalAngles[");
    Serial.print(m_numPoints);
    Serial.print("] = {");
    for (int i = m_numPoints - 1; i >= 0; i--)
    {
        Serial.print(m_angleBuffer[i], 1);
        if (i > 0)
            Serial.print(", ");
    }
    Serial.println("};");

    // Print Raw Readings (reversed: 90 to -90 for params)
    Serial.print("float rawReadings[");
    Serial.print(m_numPoints);
    Serial.print("] = {");
    for (int i = m_numPoints - 1; i >= 0; i--)
    {
        Serial.print(m_rawBuffer[i], 4);
        if (i > 0)
            Serial.print(", ");
    }
    Serial.println("};");

    Serial.println("\nUse mag.setFullCalibration(rawReadings, physicalAngles, 7);");
}
