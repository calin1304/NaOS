#ifndef ERROR_H
#define ERROR_H

typedef enum error_code {
    ENOMEM,
    EBADARG,
    EFAULT
} error_code_t;

error_code_t last_error;

#endif