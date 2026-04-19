#include "MagnetometerCalibrator.hpp"
#include "params.hpp"

MagnetometerCalibrator::MagnetometerCalibrator(Magnetometer &mag)
    : m_mag(mag), m_rawBuffer(nullptr), m_angleBuffer(nullptr), m_numPoints(0)
{
}

void MagnetometerCalibrator::runInterativeCalibration(int numPoints, const String &legLabel)
{
    m_numPoints = numPoints;

    if (m_rawBuffer) delete[] m_rawBuffer;
    if (m_angleBuffer) delete[] m_angleBuffer;

    m_rawBuffer  = new float[m_numPoints];
    m_angleBuffer = new float[m_numPoints];

    Serial.println();
    Serial.print("=== Calibrating "); Serial.print(legLabel); Serial.println(" ===");

    float startAngle = -90.0;
    float endAngle   =  90.0;
    float step = (endAngle - startAngle) / (m_numPoints - 1);

    for (int i = 0; i < m_numPoints; i++)
    {
        float targetAngle = startAngle + (i * step);
        m_angleBuffer[i]  = targetAngle;

        Serial.print("\n[Step "); Serial.print(i + 1); Serial.print("/"); Serial.print(m_numPoints); Serial.println("]");
        Serial.print(">> Move joint to: "); Serial.print(targetAngle, 1); Serial.println(" degrees");
        Serial.println(">> Press ANY key to capture...");

        waitForUser();

        if (!m_mag.isMagnetPresent())
        {
            Serial.print("WARNING: No magnet detected (magnitude=");
            Serial.print(m_mag.getMagnitude(), 2);
            Serial.print(", threshold=");
            Serial.print(Params::MagSensorValues::minMagnitude, 0);
            Serial.println("). Move magnet closer and press any key to retry.");
            waitForUser();
        }

        float sum = 0;
        const int samples = 50;
        for (int s = 0; s < samples; s++)
        {
            sum += m_mag.getRawHeading();
            delay(10);
        }
        m_rawBuffer[i] = sum / (float)samples;

        Serial.print("Captured: "); Serial.print(m_rawBuffer[i], 4);
        Serial.print("  (magnitude: "); Serial.print(m_mag.getMagnitude(), 2); Serial.println(")");
    }

    printCalibrationArrays(legLabel);
}

void MagnetometerCalibrator::waitForUser()
{
    while (Serial.available() > 0) Serial.read();
    while (Serial.available() == 0) delay(10);
    while (Serial.available() > 0) Serial.read();
}

void MagnetometerCalibrator::printCalibrationArrays(const String &legLabel)
{
    // Normalize all raw readings relative to the 0-degree capture to fix atan2 wrap-around.
    // The 0-degree reading is the middle index.
    float ref = m_rawBuffer[m_numPoints / 2];
    for (int i = 0; i < m_numPoints; i++)
    {
        float diff = m_rawBuffer[i] - ref;
        while (diff >  180) diff -= 360;
        while (diff < -180) diff += 360;
        m_rawBuffer[i] = diff + ref;
    }

    // Sort (raw, angle) pairs by raw value ascending so the LUT bracket search works.
    for (int i = 0; i < m_numPoints - 1; i++)
    {
        for (int j = 0; j < m_numPoints - 1 - i; j++)
        {
            if (m_rawBuffer[j] > m_rawBuffer[j + 1])
            {
                float tmpR = m_rawBuffer[j];   m_rawBuffer[j]   = m_rawBuffer[j + 1];   m_rawBuffer[j + 1]   = tmpR;
                float tmpA = m_angleBuffer[j]; m_angleBuffer[j] = m_angleBuffer[j + 1]; m_angleBuffer[j + 1] = tmpA;
            }
        }
    }

    Serial.println("\n--- CALIBRATION COMPLETE ---");
    Serial.println("Copy these into params.hpp:\n");

    Serial.print("static const float RAW_READINGS_"); Serial.print(legLabel); Serial.print("[7] = {");
    for (int i = 0; i < m_numPoints; i++)
    {
        Serial.print(m_rawBuffer[i], 4);
        if (i < m_numPoints - 1) Serial.print(", ");
    }
    Serial.println("};");
}
