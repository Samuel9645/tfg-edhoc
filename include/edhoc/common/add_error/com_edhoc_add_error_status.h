/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/04/2026
 * @brief Enumeration of status codes for the functions to add errors to
 * response buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_ERROR_STATUS_H_
#define EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_ERROR_STATUS_H_

enum com_edhoc_add_error_status {
  COM_EDHOC_ADD_ERROR_OK = 0,
  COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER,
  COM_EDHOC_ADD_ERROR_ERR_COMPOSE,
};

#endif  // EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_ERROR_STATUS_H_