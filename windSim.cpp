#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <getopt.h>
#include "float3.h"
using namespace std;

static constexpr int WIND_UPDATE_RATE = 15 * 1;
float minWindStrength = 0.0f;
float maxWindStrength = 20.0f;

bool verbose = false;
uint16_t iterations = 0;
unsigned int windDirTimer = 0;
float curWindStrength = 0.0f;
float3 curWindVec;
float3 oldWindVec;
float3 newWindVec;
float3 curWindDir;

//helper functions
float smoothstep(const float edge0, const float edge1, const float value)
{
	const float v = std::clamp(value, edge0, edge1);
	const float x = (v - edge0) / (edge1 - edge0);
	const float t = std::clamp(x, 0.0f, 1.0f);
	return (t * t * (3.0f - 2.0f * t));
}

template<class T, typename T2> constexpr T mix(const T v1, const T v2, const T2 a) { return (v1 + (v2 - v1) * a); }

void reset(void)
{
    windDirTimer = 0;
    curWindStrength = 0.0f;
    curWindDir = RgtVector;
	curWindVec = ZeroVector;
	newWindVec = ZeroVector;
	oldWindVec = ZeroVector;
}

// function to update wind speed
void updateWind() 
{
    if(windDirTimer == 0) 
    {
        oldWindVec = curWindVec;
        newWindVec = oldWindVec;
        
        float newStrength = 0.0f;

        do
        {
            newWindVec.x -= (rand() % 2 - 0.5f) * maxWindStrength;
            newWindVec.z -= (rand() % 2 - 0.5f) * maxWindStrength;
            newStrength = newWindVec.Length();
        } while (newStrength == 0.0f);
        
        newWindVec /= newStrength;
        newWindVec *= (newStrength = std::clamp(newStrength, minWindStrength, maxWindStrength));
    }
    else
    {
        const float mod = smoothstep(0.0f, 1.0f, windDirTimer / float(WIND_UPDATE_RATE));
        // blend between old & new wind directions
        curWindVec = mix(oldWindVec, newWindVec, mod);
        curWindStrength = curWindVec.LengthNormalize();

        curWindDir = curWindVec;
		curWindVec = curWindDir * (curWindStrength = std::clamp(curWindStrength, minWindStrength, maxWindStrength));
    }
    windDirTimer = (windDirTimer + 1) % (WIND_UPDATE_RATE + 1);
}

int main(int argc, char* argv[]) {

    //use getopt to read in command line arguments
    // options are -m minWindStrength -M maxWindStrength -s seed -i iterations
    // default values are minWindStrength = 0.0, maxWindStrength = 20.0, seed = time(NULL), iterations = 5000
    // if no arguments are provided, default values are used
    
    option longopt[] = {
        {"minWindStrength", required_argument, NULL, 'm'},
        {"maxWindStrength", required_argument, NULL, 'M'},
        {"seed", required_argument, NULL, 's'},
        {"iterations", required_argument, NULL, 'i'},
        {"verbose", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };

    //set default values
    minWindStrength = 0.0f;
    maxWindStrength = 20.0f;
    srand((unsigned) time(NULL));
    iterations = 5000;

    // read in command line arguments
    int opt;
    while((opt = getopt_long(argc, argv, "m:M:s:i:v", longopt, 0)) != -1)
    {
        if(opt == -1)
            break;
        switch(opt)
        {
            case 'm':
                minWindStrength = atof(optarg);
                break;
            case 'M':
                maxWindStrength = atof(optarg);
                break;
            case 's':
                srand(atoi(optarg));
                break;
            case 'i':
                iterations = atoi(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            case '?':
                printf("Unknown option '%c' was specified.\n", optopt);
                break;
            case ':':
                printf("Option '%c' requires an argument.\n", optopt);
                break;
            default:
                break;
        }
    }

    //print all options to stderr
    if(verbose == true)
    {
        std::cerr << "minWindStrength: " << minWindStrength << endl;
        std::cerr << "maxWindStrength: " << maxWindStrength << endl;
        std::cerr << "first random number: " << rand() << endl;
        std::cerr << "iterations: " << iterations << endl;
    }

    reset();

    std::cout << "x" << endl;
    
    for (size_t i = 0; i < 100; i++)
    {
        updateWind();
    }
    


    for(int i = 0; i < iterations; i++)
    {
        
        updateWind();
        // std::cout << "Wind Direction: " << curWindDir.x << ", " << curWindDir.y << ", " << curWindDir.z << endl;
        // std::cout << i << ", " << curWindStrength << endl;
        std::cout << curWindStrength << endl;
    }



    return 0;
}
