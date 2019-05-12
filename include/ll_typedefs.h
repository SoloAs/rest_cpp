#pragma once

enum class LLError 
{
  LL_NO_ERROR, /* No error */
  LL_HW_FAIL, /* Hw failure */
  LL_NOT_INIT, /* No initialization routine was performed */
  LL_WRONG_PARAM, /* wrong parameters passed. check the parameter. */
  LL_UNIMPLEMENTED,  /* Feature mot supported */
  LL_PREEXISTS, /* Duplicate configuration */
  LL_CONFLICT, /* conflicts with an existing entry. check the params. For example in case of joining a stream 
                  the same ip and udp are already being used for a different channel */
  LL_NOT_TX_BUILD, /*Build does not contain TX component*/
  LL_NOT_RX_BUILD /*Build does not contain RX component*/
};

struct init_params 
{
  float init_hw_clk_rate; 
  unsigned int tx_port;
  unsigned int rx_port;
  char src_ip[2][16];   // 'ff:ff:ff:ff:ff'
  char src_mac[2][20];  // '255.255.255.255' 
  int dev_number; 
};

struct stream_info 
{
  char id[40];
  unsigned int smpte_type;
  char ip_da[16];
  unsigned int media_chan;
};

struct tx_stream_info : stream_info 
{
  // char id[40];           // stream ID - maps to NMOS-type ids - UUID4     
  unsigned int udp_sp;   // Source Port
  // char ip_da[16];        // IP Destination Addr
  unsigned int udp_dp;   // Destination UDP port
  int vlan;              // -1 noVLAN (TODO: ask Andrew: do we need that?)
  int pgroup_size; 
  unsigned int ttl;          // TTL for all streamed packets
  unsigned int video_pt; 
  unsigned int audio_pt; 
  // unsigned int media_chan;   // Associated input Channel
  // unsigned int smpte_type;   // Stream format
};

struct rx_stream_info : stream_info
{
  // char id[40];         // stream ID - maps to NMOS-type ids - UUID4
  // char ip_da[16];      // MC IP Addr of received stream
  unsigned int udp_dp; // UDP Port of received stream
  // char ip_sa[16];      // Ignored unless IGMPV3 src flt (???)
  // unsigned int smpte_type; // Stream format
  char vid_format[16];         // 1080i, 1080p 720p etc
  unsigned int frame_rate;     // Frame rate(e.g, 30,50,60...
  unsigned int playout_delay;  //  Playout delay in ms
  int aud_chan;               //  Number of audios for audio
  // unsigned int media_chan;      // Output channel
};
