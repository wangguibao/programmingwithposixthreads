#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>

#define ERROR_RETURN(str, errcode) {\
    printf("%s", str);              \
    return errcode;                 \
}

#endif // UTILITY_H
