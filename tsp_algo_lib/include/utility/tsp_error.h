#ifndef TSP_TSP_ERROR_H
#define TSP_TSP_ERROR_H

typedef enum TspParserStatus TspParserStatus;

/* Error codes for instance loading */
typedef enum {
    TSP_OK = 0,
    TSP_ERR_FILE_OPEN,
    TSP_ERR_PARSE_HEADER, // Dimension or format error
    TSP_ERR_PARSE_NODES, // Error reading node coordinates
    TSP_ERR_MEMORY,
    TSP_ERR_INVALID_EXT,
    TSP_ERR_UNKNOWN_FORMAT
} TspError;

/**
 * @brief Helper to get a string representation of the error.
 */
const char *tsp_error_to_string(TspError err);

TspError tsp_error_from_parser_status(TspParserStatus st);

#endif //TSP_TSP_ERROR_H
