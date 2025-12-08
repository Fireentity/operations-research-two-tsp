#include "tsp_error.h"
#include "tsp_parser.h"


const char *tsp_error_to_string(const TspError err) {
    switch (err) {
        case TSP_OK: return "Success";
        case TSP_ERR_FILE_OPEN: return "Unable to open file";
        case TSP_ERR_PARSE_HEADER: return "Malformed TSPLIB header";
        case TSP_ERR_PARSE_NODES: return "Invalid node coordinates";
        case TSP_ERR_MEMORY: return "Memory allocation failed";
        case TSP_ERR_INVALID_EXT: return "Unsupported file extension";
        case TSP_ERR_UNKNOWN_FORMAT: return "Unknown format";
        default: return "Unknown error";
    }
}
TspError tsp_error_from_parser_status(const TspParserStatus st) {
    switch (st) {
        case PARSE_OK:
            return TSP_OK;
        case PARSE_ERR_OPEN:
            return TSP_ERR_FILE_OPEN;
        case PARSE_ERR_FORMAT:
        case PARSE_ERR_DIM:
            return TSP_ERR_PARSE_HEADER;
        case PARSE_ERR_INV:
            return TSP_ERR_PARSE_NODES;
        case PARSE_ERR_MEMORY:
            return TSP_ERR_MEMORY;
        case PARSE_ERR_EXT:
            return TSP_ERR_INVALID_EXT;
        case PARSE_ERR_OVERFLOW:
        default:
            return TSP_ERR_UNKNOWN_FORMAT;
    }
}
