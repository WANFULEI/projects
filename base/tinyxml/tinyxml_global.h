#ifndef TINYXML_GLOBAL_H
#define TINYXML_GLOBAL_H


#ifdef TINYXML_LIB
# define TINYXML_EXPORT __declspec(dllexport)
#else
# define TINYXML_EXPORT __declspec(dllimport)
#endif

#endif // TINYXML_GLOBAL_H