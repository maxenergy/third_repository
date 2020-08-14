/* $Id$ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/**
 * simple_pjsua.c
 *
 * This is a very simple but fully featured SIP user agent, with the 
 * following capabilities:
 *  - SIP registration
 *  - Making and receiving call
 *  - Audio/media to sound device.
 *
 * Usage:
 *  - To make outgoing call, start simple_pjsua with the URL of remote
 *    destination to contact.
 *    E.g.:
 *	 simpleua sip:user@remote
 *
 *  - Incoming calls will automatically be answered with 200.
 *
 * This program will quit once it has completed a single call.
 */

#include <pjsua-lib/pjsua.h>

#define THIS_FILE	"APP"

#define SIP_DOMAIN	"121.40.80.20"
#define SIP_USER	"1001"
#define SIP_PASSWD	"12345"

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
}

int sip_client_init(pjsua_callback *cb,int port){
	pjsua_acc_id acc_id;
	pj_status_t status;
	
   /* Create pjsua first! */
   status = pjsua_create();
   if (status != PJ_SUCCESS){
	   	error_exit("Error in pjsua_create()", status);
		return -1;
   }
   
   /* Init pjsua */
   {
   pjsua_config cfg;
   pjsua_logging_config log_cfg;

   pjsua_config_default(&cfg);
   cfg.cb.on_incoming_call = cb->on_incoming_call;
   cfg.cb.on_call_media_state = cb->on_call_media_state;
   cfg.cb.on_call_state = cb->on_call_state;
   //cfg.outbound_proxy_cnt = 1;
   //cfg.outbound_proxy[0] = pj_str("sip:121.40.80.20");

   pjsua_logging_config_default(&log_cfg);
   log_cfg.console_level = 1;

   status = pjsua_init(&cfg, &log_cfg, NULL);
   if (status != PJ_SUCCESS){ 
   		error_exit("Error in pjsua_init()", status);
		return -1;
   	}
   }
#if 0
   /* Add UDP transport. */
   {
   pjsua_transport_config cfg;

   pjsua_transport_config_default(&cfg);
   cfg.port = port;
   status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
   if (status != PJ_SUCCESS){ 
   		error_exit("Error creating transport", status);
		return -1;
   	}
   }
#endif
   /*add tcp transport*/
  {
   pjsua_transport_config tcp_cfg;
   pjsua_transport_config_default(&tcp_cfg);
   tcp_cfg.port = 0;
   
   status = pjsua_transport_create(PJSIP_TRANSPORT_TCP,
								   &tcp_cfg, NULL);
   if (status != PJ_SUCCESS) {
   		error_exit("Error creating transport", status);
		return -1;
   }
  }

   /* Initialization is done, now start pjsua */
   status = pjsua_start();
   if (status != PJ_SUCCESS){
	   	error_exit("Error starting pjsua", status);
		return -1;
  	}
  return 0;
}

void sip_hangup_all()
{
	pjsua_call_hangup_all();
}

int sip_make_call(pjsua_acc_id acc_id,char * url)
{	
    pj_status_t status;
	pj_str_t uri = pj_str(url);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
	if (status != PJ_SUCCESS){
		return -1;
	}
	return 0;
}

int sip_client_register(pjsua_acc_id *acc_id,char* user,char* domain,char* passwd)
{
    pj_status_t status;

	pjsua_acc_config cfg;
	pjsua_acc_config_default(&cfg);
	char buf_id[255];
	char buf_regurl[255];
	sprintf(buf_id,"sip:%s@%s;transport=tcp",user,domain);
	sprintf(buf_regurl,"sip:%s;transport=tcp",domain);
	cfg.id = pj_str(buf_id);
	cfg.reg_uri = pj_str(buf_regurl);
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str("*");
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str(user);
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str(passwd);
	cfg.vid_in_auto_show = 0;
	cfg.vid_out_auto_transmit = 1;
	status = pjsua_acc_add(&cfg, PJ_TRUE, acc_id);
	if (status != PJ_SUCCESS){ 
		error_exit("Error adding account", status);
		return -1;
	}
	return 0;
}

#if 0
int sip_client_init(pjsua_callback *cb,int port)
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

    /* If argument is specified, it's got to be a valid SIP URL */
    if (argc > 1) {
	status = pjsua_verify_url(argv[1]);
	if (status != PJ_SUCCESS) error_exit("Invalid URL in argv", status);
    }
    
    /* Init pjsua */
    {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;

	pjsua_config_default(&cfg);
	cfg.cb.on_incoming_call = &on_incoming_call;
	cfg.cb.on_call_media_state = &on_call_media_state;
	cfg.cb.on_call_state = &on_call_state;

	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 4;

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add UDP transport. */
    {
	pjsua_transport_config cfg;

	pjsua_transport_config_default(&cfg);
	cfg.port = 5060;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

    /* Register to SIP server by creating SIP account. */
    {
	pjsua_acc_config cfg;

	pjsua_acc_config_default(&cfg);
	cfg.id = pj_str("sip:" SIP_USER "@" SIP_DOMAIN);
	cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str(SIP_DOMAIN);
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str(SIP_USER);
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str(SIP_PASSWD);
	cfg.vid_in_auto_show = 0;
	cfg.vid_out_auto_transmit = 1;
	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }

    /* If URL is specified, make call to the URL. */
    if (argc > 1) {
	pj_str_t uri = pj_str(argv[1]);
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
	if (status != PJ_SUCCESS) error_exit("Error making call", status);
    }

    /* Wait until user press "q" to quit. */
    for (;;) {
	char option[10];

	puts("Press 'h' to hangup all calls, 'q' to quit");
	if (fgets(option, sizeof(option), stdin) == NULL) {
	    puts("EOF while reading stdin, will quit now..");
	    break;
	}

	if (option[0] == 'q')
	    break;

	if (option[0] == 'h')
	    pjsua_call_hangup_all();
    }

    /* Destroy pjsua */
    pjsua_destroy();

    return 0;
}
#endif
