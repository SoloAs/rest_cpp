#include "ll_typedefs.h"    
#include "ll_api_class.h"

    
    ll_api_class* ll_api_class::instance;

    ll_api_class* ll_api_class::get_instance() {
        if (instance == nullptr) {
            instance = new ll_api_class;
        }
        return instance;
    }

    ll_api_class::ll_api_class(){
    };

    ll_api_class::~ll_api_class(){
    };

    LLError ll_api_class::post_bringup_params(init_params params){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::init(){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::reset(){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::get_version(unsigned int *firmware_ver, unsigned int *software_ver){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::check_modules_version(unsigned int *check_result){
        return LLError::LL_UNIMPLEMENTED;
    }
 
    LLError ll_api_class::set_tx_stream(tx_stream_info data){
        return LLError::LL_UNIMPLEMENTED;
    }
 
    LLError ll_api_class::set_rx_stream(rx_stream_info data){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::stop_stream(unsigned int ip_address, unsigned int udp_port, unsigned int channel){ // TODO: channel ???
        return LLError::LL_UNIMPLEMENTED;
    }
    LLError ll_api_class::stop_stream(char *id){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::get_tx_stream_info(std::vector<tx_stream_info> *tx_streams){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::get_rx_stream_info(std::vector<rx_stream_info> *rx_streams){
        return LLError::LL_UNIMPLEMENTED;
    }

    LLError ll_api_class::validate_receiver(tx_stream_info sender, bool *result){
        return LLError::LL_UNIMPLEMENTED;
    }