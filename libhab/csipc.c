/* 
 * This file is part of libhab.
 *
 * libhab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * libhab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with libhab.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * 
 * Created:	18/02/2015
 * Author:	Peter Bosch <peterbosc@gmail.com>
 */

#ifndef __csipc_h__
#define __csipc_h__

/**
 * We need stdint for the unsigned integer type definitions.
 */
#include <hab/csipc.h>

/*
 * Include stdio for fprintf, sprintf, 
 */
#include <stdio.h>

/*
 * Include stdlib for memory management
 */
#include <stdlib.h>

/*
 * Include string for str‌ing and memory manipulation
 */
#include <string.h>


/*
 * Include mqueue for POSIX message queues
 */
#include <mqueue.h>

cs_chan_t *csipc_open_channel( const char *name )
{
}

char *csipc_int_generate_chname( const char *name, const char *chan )
{

	char *ipc_name;
	size_t length;

	assert( name != NULL );
	assert( chan != NULL );

	length = 1 + strlen ( name ) + 1 + strlen ( chan ) + 1;

	assert( length <  256 );

	ipc_name = malloc ( length );

	sprintf ( ipc_name, "/%s_%s", name, chan );
	
	return ipc_name;

}

cs_chan_t *csipc_int_create_channel (	const char 	*server, 
					const char 	*name, 
					size_t 		 msg_size, 
					int 		 max_msg )
{

	cs_chan_t *channel;
	char *ipc_name;
	struct mq_attr ipc_attr;

	assert( server != NULL );
	assert( name != NULL );

	channel = malloc ( sizeof (cs_chan_t) );
	
	assert( channel != NULL );

	channel->ls_name = malloc ( strlen ( name ) );
	
	assert( channel->ls_name != NULL);
	
	strcpy ( channel->ls_name, name );
	
	ipc_name = csipc_int_generate_chname ( server, name );
	
	ipc_attr.mq_flags   = O_NONBLOCK;
	ipc_attr.mq_maxmsg  = max_msg;
	ipc_attr.mq_msgsize = msg_size;
	ipc_attr.mq_curmsgs = 0;

	channel->mq_handle = mq_open (	ipc_name, 
					O_RDONLY | O_NONBLOCK | O_CREAT,
					CSIPC_MODE,
					&ipc_attr );

	assert( channel->mq_handle != ( mqd_t ) -1 );

	channel->mq_size   = ipc_attr.mq_maxmsg;
	channel->pl_size   = (size_t) ipc_attr.mq_msgsize;
	channel->pl_buffer = malloc ( channel->pl_size );
	
	assert( channel->pl_buffer != NULL );
		
	free ( ipc_name );

	return channel;

}

cs_chan_t *csipc_int_open_channel ( const char *server, const char *name )
{
	int status;
	cs_chan_t *channel;
	char *ipc_name;
	struct mq_attr ipc_attr;

	assert( server != NULL );
	assert( name != NULL );

	channel = malloc ( sizeof (cs_chan_t) );
	
	assert( channel != NULL );

	channel->ls_name = malloc ( strlen ( name ) );
	
	assert( channel->ls_name != NULL);
	
	strcpy ( channel->ls_name, name );
	
	ipc_name = csipc_int_generate_chname ( server, name );

	channel->mq_handle = mq_open (	ipc_name, 
					O_RDONLY | O_NONBLOCK );

	assert( channel->mq_handle != ( mqd_t ) -1 );

	status = mq_getattr( channel->mq_handle, &ipc_attr );

	channel->mq_size   = ipc_attr.mq_maxmsg;
	channel->pl_size   = (size_t) ipc_attr.mq_msgsize;
	channel->pl_buffer = malloc ( channel->pl_size );
	
	assert( channel->pl_buffer != NULL );
		
	free ( ipc_name );

	return channel;

}

int	csipc_int_read_channel( cs_chan_t *channel )
{
	ssize_t rd_size;

	assert( channel != NULL );
	
	rd_size = mq_receive (  channel->mq_handle, 
				channel->pl_buffer, 
				channel->pl_size,
				NULL );

	if ( rd_size == -1 ) {

		assert( errno == EAGAIN );

		return FALSE;
	
	} else {

		assert( rd_size == channel->pl_size );

		return TRUE;

	}

}

int	csipc_int_write_channel( cs_chan_t *channel )
{
	int status;

	assert( channel != NULL );
	
	status = mq_send ( channel->mq_handle, 
			   channel->pl_buffer, 
			   channel->pl_size,
			   NULL );

	if ( status == -1 ) {

		assert( errno == EAGAIN );

		return FALSE;
	
	} else {

		return TRUE;

	}

}

int  csipc_int_find_listener_iterator( llist _t *node, void *param )
{
	const char *name = param;
	cs_chan_t *channel = (cs_chan_t *) node;

	return strcmp ( channel->ls_name, name ) == 0;
}

void csipc_server_add_listener( cs_srv_t *server, const char *name )
{
	cs_chan_t *channel;

	assert( server != NULL );
	assert( name != NULL );
	
	if ( llist_iterate_select( &( server->ch_listeners ), 
				   csipc_int_find_listener_iterator,
				   name ) != NULL ) {
		return;	
	}
	
	channel = csipc_int_open_channel ( server->ch_name, name );

	llist_add_end ( &( server->ch_listeners ), (llist_t *) channel );

}

void csipc_server_process( cs_srv_t *server )
{
	int rd_ctr;
	size_t announce_size;
	llist_t *node;
	cs_chan_t *listener;

	assert( server != NULL );

	for ( rd_ctr = 0; rd_ctr < server->ch_announce->mq_size; rd_ctr++) {

		if ( ! csipc_int_read_channel( server->ch_announce ) )
			break;
		
		announce_size = strnlen ( server->ch_announce->pl_buffer,
					  server->ch_announce->pl_size );
		
		if ( announce_size == server->ch_announce->pl_size )
			continue;

		csipc_server_add_listener ( server, 
					    server->ch_announce->pl_buffer );
			
	}	

	for ( 	node = &( server->ch_listeners ); 
		node != &( server->ch_listeners );
		node = node->next ) {
		
		listener = (cs_chan_t *) node;
		
		csipc_int_write_channel ( listener );
	}
		
}



