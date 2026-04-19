#ifndef HEADING_EMA_HPP
#define HEADING_EMA_HPP

class HeadingEMA
{
public:
    // Constructor requires the smoothing factor (alpha)
    HeadingEMA(float alpha) : alpha(alpha), initialized(false), currentEMA(0) {}

    float operator()(float input)
    {
        if (!initialized)
        {
            // On the first reading, snap directly to the input
            currentEMA = input;
            initialized = true;
        }
        else
        {
            // 1. Find the raw difference between the new reading and the current average
            float diff = input - currentEMA;

            // 2. Find the shortest path for a 180-degree span (-90 to +90)
            if (diff < -90.0f)
                diff += 180.0f;
            if (diff > 90.0f)
                diff -= 180.0f;

            // 3. Apply the EMA smoothing ONLY to that shortest difference
            currentEMA += diff * alpha;

            // 4. Keep the final output strictly bounded between -90 and +90
            if (currentEMA > 90.0f)
                currentEMA -= 180.0f;
            if (currentEMA < -90.0f)
                currentEMA += 180.0f;
        }
        return currentEMA;
    }

    void reset()
    {
        initialized = false;
    }

private:
    float alpha;
    float currentEMA;
    bool initialized;
};

#endif // HEADING_EMA_HPP
