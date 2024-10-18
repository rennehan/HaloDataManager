#ifndef MAIN_HPP
#define MAIN_HPP

#ifdef REAL_DOUBLE
typedef double real;
#else
typedef float real;
#endif

// The different ASCII file formats that we can read.
// These are specialized in the DataContainer.cpp file.
struct RockstarData { };
struct ConsistentTreesData { };

#endif

