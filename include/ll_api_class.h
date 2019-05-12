#ifndef _LL_API_CLASS_
#define _LL_API_CLASS_

#include "ll_typedefs.h"
#include <vector>

class ll_api_class {
    
    public:
    
    
    static ll_api_class* get_instance();
    
    /**
     * @brief      Loads configuration params for subsequent board initialization
     * 
     * @details    Should validate incoming parameters
     *
     * @param[in]  init_params params - parameters received from request (see ll_typedefs.h)
     *
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_WRONG_PARAM - wrong parameters passed. check the parameter.
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     */
    LLError post_bringup_params(init_params params);

    /**
     * @brief      1G module initialization function.
     *
     * @details    Parameters from post_bringup_params are utilized, thus has to always go in pair 
     *             OR be provided with set of default parameters, which can be overwritten
     * 
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     */
    LLError init();

    /**
     * @brief      1G module reset function
     *
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     * 
     * @note       Does it make sense to implement this as init function with
     *             usage of configured parameters?
     */
    LLError reset();

    /**
     * @brief      Acquires software and firmware versions of the board
     *
     * @param[out] unsigned int *firmware version - Indicates firmware version
     * 
     * @param[out] unsigned int *software version - Indicates software version
     *
     * @return     pointers to software and firmware versions
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     */
    LLError get_version(unsigned int *firmware_ver, unsigned int *software_ver);

    /**
     * @brief      Checks whether modules are compatible with the software.
     *
     * @details    Checks each module in the FPGA code for compatibility
     *             with the software
     *
     * @param[out] int *check_result  - Indicates of compatible or incompatible
     *
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_NO_ERROR
     *             
     * @todo       Implement me
     *
     */
    LLError check_modules_version(unsigned int *check_result);

    /**
     * @brief      Sets up TX stream
     *
     * @details    Uses internal modules to setup a TX stream. Won't be invoked
     *             unless board is in TX mode 
     *
     * @param[in]  tx_stream_info data - TX stream parameters
     *
     * @return     Pointer to result flag
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_CONFLICT - entry already exists
     * @retval     LL_WRONG_PARAM - wrong parameters passed
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     * 
     * @note       Guess to keep LL minimalistic as possible, we will implement
     *             restriction on RX/TX mode on higher levels, such as RESTful API,
     *             making sure user won't be able to setup RX stream in TX mode
     */
    LLError set_tx_stream(tx_stream_info data);

    /**
     * @brief      Sets up RX stream
     *
     * @details    Uses internal modules to setup a RX stream. Won't be invoked
     *             unless board is in RX mode 
     *
     * @param[in]  tx_stream_info data - RX stream parameters
     *
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_CONFLICT - entry already exists
     * @retval     LL_WRONG_PARAM - wrong parameters passed
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     * 
     *
     * @todo       Implement me
     * 
     * @note       To keep LL minimalistic as possible, we will implement
     *             restriction on RX/TX mode on higher levels, such as RESTful API,
     *             making sure user won't be able to setup RX stream in TX mode
     */
    LLError set_rx_stream(rx_stream_info data);

     /**
     * @brief      Stops stream
     *
     * @details    Uses internal modules to stop a stream
     *
     * @param[in]  unsigned int ip_address - ip address of the stream
     * @param[in]  unsigned int udp_port - udp port of the stream
     * @param[in]  unsigned int channel - stream channel
     *
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_WRONG_PARAM - can't find entry with specified params
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     * 
     */
    LLError stop_stream(unsigned int ip_address, unsigned int udp_port, unsigned int media_channel);

     /**
     * @brief      Stops stream
     *
     * @details    Uses internal modules to stop a stream. Is override for
     *             previous function
     *
     * @param[in]  char *id - NMOS-like id of the stream (UUID4) - unique
     * 
     * @return     The LL_API ERROR status for the result of the function
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_WRONG_PARAM - can't find entry with specified params
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     * 
     * @note       Do you think such override is implementable? Would be 
     *             nice to have this in terms of stream management on 
     *             higher levels
     */
    LLError stop_stream(char *id);

    /**
     * @brief      Obtains TX streams information
     *
     * @details    List of streams which can be displayed by id
     *
     * @param[out] std::vector<tx_stream_info> *tx_streams - 
     *             vector of tx streams information 
     *
     * @return     array of information about present tx streams
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_WRONG_PARAM - can't find entry with specified params
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     * 
     * @note       Using vector/map might be more convenient, but that will make 
     *             us pull std library just for one argument
     *
     */
    LLError get_tx_stream_info(std::vector<tx_stream_info> *tx_streams);

    /**
     * @brief      Obtains RX streams information
     *
     * @details    List of streams which can be displayed by id
     *
     * @param[out] std::vector<rx_stream_info> *rx_streams - 
     *             vector of rx streams information      
     *
     * @return     Array of information about present rx streams
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_WRONG_PARAM - can't find entry with specified params
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     *
     * @note       using vector/map might be more convenient, but that will make 
     *             us pull std library just for one argument
     */
     LLError get_rx_stream_info(std::vector<rx_stream_info> *rx_streams);

     /**
     * @brief      Validates device to receive from specific
     *             stream (if it's in receiver mode)
     *
     * @details    Needs to be implemented for cases where we want to 
     *             verify if we are able to receive from specific stream
     *             before we actually setup the stream
     *
     * @param[in]  tx_stream_info sender - stream we want to receive
     *             from
     * @param[out] bool *result - validation result         
     *
     * @return     receiver validation result
     *
     * @retval     LL_HW_FAIL - hardware failure
     * @retval     LL_WRONG_PARAM - can't find entry with specified params
     * @retval     LL_NOT_INIT - module wasn't initilized properly
     * @retval     LL_NO_ERROR
     *
     * @todo       Implement me
     *
     * @note       Using vector/map might be more convenient, but that will make 
     *             us pull std library just for one argument
     */
    LLError validate_receiver(tx_stream_info sender, bool *result);

    





    private:

        ll_api_class();
        ~ll_api_class();
        static ll_api_class* instance;

        // ===========================
        // list of module classes
        // ===========================

};

#endif