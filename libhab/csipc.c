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
 * Include logging functionality
 */
#include <hab/cslog.h>

/*
 * Include mqueue for POSIX message queues
 */
#include <mqueue.h>

/*
 * Include errno.h for error handling
 */
#include <errno.h>

/*
 * Include stddef.h for type defs
 */
#include <stddef.h>

char *csipc_listener_name;

void csipc_set_program( const char *name )
{
	csassert( name != NULL );

	csipc_listener_name = malloc ( strlen ( name ) + 1 );

	strcpy ( csipc_listener_name, name );

}

char *csipc_int_generate_chname( const char *name, const char *chan )
{

	char *ipc_name;
	size_t length;

	csassert( name != NULL );
	csassert( chan != NULL );

	length = 1 + strlen ( name ) + 1 + strlen ( chan ) + 1;

	cserror( length <  256, LOG_ERROR, 
		"Tried to generate channel name longer (%i) than 256 characters"
		, length );

	ipc_name = malloc ( length );

	sprintf ( ipc_name, "/%s_%s", name, chan );
	
	return ipc_name;

}

cs_chan_t *csipc_int_create_channel_zerocopy (	const char 	*server, 
						const char 	*name, 
						void		*buffer,
						size_t 		 msg_size, 
						int 		 max_msg )
{

	cs_chan_t *channel;
	char *ipc_name;
	struct mq_attr ipc_attr;

	csassert( server != NULL );
	csassert( name != NULL );
	csassert( buffer != NULL );

	channel = malloc ( sizeof (cs_chan_t) );
	
	cserror( channel != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel" );

	channel->ls_name = malloc ( strlen ( name ) + 1 );
	
	cserror( channel->ls_name != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel name" );
	
	strcpy ( channel->ls_name, name );
	
	ipc_name = csipc_int_generate_chname ( server, name );
	
	ipc_attr.mq_flags   = O_NONBLOCK;
	ipc_attr.mq_maxmsg  = max_msg;
	ipc_attr.mq_msgsize = msg_size;
	ipc_attr.mq_curmsgs = 0;

	channel->mq_handle = mq_open (	ipc_name, 
					O_RDWR | O_NONBLOCK | O_CREAT,
					CSIPC_MODE,
					&ipc_attr );

	cserror( channel->mq_handle != ( mqd_t ) -1, LOG_ERROR, 
		"Error creating mqueue: %i (%s)", errno, strerror(errno) );

	channel->mq_size   = ipc_attr.mq_maxmsg;
	channel->pl_size   = (size_t) ipc_attr.mq_msgsize;
	channel->pl_buffer = buffer;
		
	free ( ipc_name );

	return channel;

}


cs_chan_t *csipc_int_create_channel (	const char 	*server, 
					const char 	*name, 
					size_t 		 msg_size, 
					int 		 max_msg )
{

	void *buffer;

	csassert( server != NULL );
	csassert( name != NULL );

	buffer = malloc ( msg_size );
	
	cserror( buffer != NULL, LOG_ERROR,
		"Ran out of memory allocating channel buffer" );
		
	return csipc_int_create_channel_zerocopy ( 	server, 
							name, 
							buffer, 
							msg_size, 
							max_msg);

}


cs_chan_t *csipc_int_open_channel_zerocopy ( 	const char *server, 
						const char *name,
						void	   *buffer,
						size_t	    buffer_size )
{
	int status;
	cs_chan_t *channel;
	char *ipc_name;
	struct mq_attr ipc_attr;

	csassert( server != NULL );
	csassert( name != NULL );
	csassert( buffer != NULL );

	channel = malloc ( sizeof (cs_chan_t) );
	
	cserror( channel != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel" );

	channel->ls_name = malloc ( strlen ( name ) + 1 );
	
	cserror( channel->ls_name != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel name" );
	
	strcpy ( channel->ls_name, name );
	
	ipc_name = csipc_int_generate_chname ( server, name );

	channel->mq_handle = mq_open (	ipc_name, 
					O_RDWR | O_NONBLOCK | O_CREAT );

	cserror( channel->mq_handle != ( mqd_t ) -1, LOG_ERROR, 
		"Error opening mqueue: %i (%s)", errno, strerror(errno) );

	status = mq_getattr( channel->mq_handle, &ipc_attr );

	cserror( status == 0, LOG_ERROR, 
		"Error getting mqueue attributes: %i (%s)", 
		errno, strerror(errno) );

	channel->mq_size   = ipc_attr.mq_maxmsg;
	channel->pl_size   = (size_t) ipc_attr.mq_msgsize;
	channel->pl_buffer = buffer;
	
	cserror( buffer_size == channel->pl_size , LOG_ERROR,
		"Mismatched channel buffer size" );
		
	free ( ipc_name );

	return channel;

}


cs_chan_t *csipc_int_open_channel ( const char *server, const char *name )
{
	int status;
	cs_chan_t *channel;
	char *ipc_name;
	struct mq_attr ipc_attr;

	csassert( server != NULL );
	csassert( name != NULL );

	channel = malloc ( sizeof (cs_chan_t) );
	
	cserror( channel != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel" );

	channel->ls_name = malloc ( strlen ( name ) + 1 );
	
	cserror( channel->ls_name != NULL, LOG_ERROR, 
		"Ran out of memory allocating channel name" );
	
	strcpy ( channel->ls_name, name );
	
	ipc_name = csipc_int_generate_chname ( server, name );

	channel->mq_handle = mq_open (	ipc_name, 
					O_RDWR | O_NONBLOCK | O_CREAT );

	cserror( channel->mq_handle != ( mqd_t ) -1, LOG_ERROR, 
		"Error opening mqueue: %i (%s)", errno, strerror(errno) );

	status = mq_getattr( channel->mq_handle, &ipc_attr );

	cserror( status == 0, LOG_ERROR, 
		"Error getting mqueue attributes: %i (%s)", 
		errno, strerror(errno) );

	channel->mq_size   = ipc_attr.mq_maxmsg;
	channel->pl_size   = (size_t) ipc_attr.mq_msgsize;
	channel->pl_buffer = malloc ( channel->pl_size );
	
	cserror( channel->pl_buffer != NULL , LOG_ERROR,
		"Ran out of memory allocating channel buffer" );
		
	free ( ipc_name );

	return channel;

}

int	csipc_int_read_channel( cs_chan_t *channel )
{
	ssize_t rd_size;

	csassert( channel != NULL );
	
	rd_size = mq_receive (  channel->mq_handle, 
				channel->pl_buffer, 
				channel->pl_size,
				NULL );

	if ( rd_size == -1 ) {

		cserror( errno == EAGAIN, LOG_ERROR, 
			"Error reading from channel: %i (%s)", 
			errno, strerror(errno) );

		return 0;
	
	} else {

		cserror( rd_size == channel->pl_size, LOG_ERROR, 
			"Incomplete read from channel" );

		return 1;

	}

}

int	csipc_int_write_channel( cs_chan_t *channel )
{
	int status;

	csassert( channel != NULL );
	
	status = mq_send ( channel->mq_handle, 
			   channel->pl_buffer, 
			   channel->pl_size,
			   2 );

	if ( status == -1 ) {

		cserror( errno == EAGAIN, LOG_ERROR, 
			"Error writing to channel: %i (%s)", 
			errno, strerror(errno) );

		return 0;
	
	} else {

		return 1;

	}

}

int  csipc_int_find_listener_iterator( llist_t *node, void *param )
{
	const char *name = param;
	cs_chan_t *channel = (cs_chan_t *) node;

	return strcmp ( channel->ls_name, name ) == 0;
}

void csipc_server_add_listener( cs_srv_t *server, const char *name )
{
	cs_chan_t *channel;

	csassert( server != NULL );
	csassert( name != NULL );
	
	if ( llist_iterate_select( &( server->ch_listeners ), 
				   csipc_int_find_listener_iterator,
				   (void *) name ) != NULL ) {
		cs_log(LOG_INFO, "IPC client %s has reconnected", name);
		return;	
	}
	
	channel = csipc_int_open_channel ( server->ch_name, name );

	if ( channel->pl_size != server->pl_size ) {
		cs_log( LOG_WARN, 
		"IPC client %s tried to connect with invalid payload size %i",
		 name, channel->pl_size );
		return;
	}

	llist_add_end ( &( server->ch_listeners ), (llist_t *) channel );
	
	cs_log(LOG_INFO, "IPC client %s has connected", name);

}

cs_srv_t *csipc_create_server( const char *name, size_t msg_size, int max_msg )
{
	cs_srv_t *server;

	csassert( name != NULL );

	server = malloc ( sizeof ( cs_srv_t ) );
	
	cserror( server != NULL, LOG_ERROR, 
		"Ran out of memory allocating server" );

	server->ch_name = malloc ( strlen ( name ) + 1 );
	
	cserror( server->ch_name != NULL, LOG_ERROR, 
		"Ran out of memory allocating server name" );
	
	strcpy ( server->ch_name, name );

	server->ch_announce = csipc_int_create_channel (name, 
							CS_SES_ANNOUNCE,
							CSIPC_ANNOUNCE_SIZE,
							CSIPC_ANNOUNCE_MAXMSG);
	
	cserror( server->ch_announce != NULL, LOG_ERROR, 
		"Failed to create announce channel" );
	
	llist_create ( &( server->ch_listeners ) );
	
	server->mq_size = max_msg;
	
	server->pl_size = msg_size;

	server->pl_buffer = malloc ( server->pl_size );
	
	cserror( server->pl_buffer != NULL, LOG_ERROR, 
		"Ran out of memory allocating server buffer" );

	return server;
	
}

cs_chan_t *csipc_open_channel_zerocopy( const char 	*name, 
					void		*buffer,
					size_t 		 msg_size, 
					int 		 max_msg )
{
	cs_chan_t *channel;

	csassert( name != NULL );
	csassert( buffer != NULL );

	channel = csipc_int_create_channel_zerocopy ( 	name, 
					     		csipc_listener_name, 
							buffer,
					     		msg_size, 
					     		max_msg );

	channel->ch_announce = csipc_int_open_channel ( name, CS_SES_ANNOUNCE );
	
	strcpy ( channel->ch_announce->pl_buffer, csipc_listener_name );
	
	if ( csipc_int_write_channel ( channel->ch_announce ) ) {
		return channel;		
	} else {
		cs_log_fatal(	LOG_ERROR, 
				"Failed to write to announce channel" );
		return channel;
	}
}

cs_chan_t *csipc_open_channel( const char *name, size_t msg_size, int max_msg )
{
	cs_chan_t *channel;

	csassert( name != NULL );

	channel = csipc_int_create_channel ( name, 
					     csipc_listener_name, 
					     msg_size, 
					     max_msg );

	channel->ch_announce = csipc_int_create_channel (name, 
							CS_SES_ANNOUNCE,
							CSIPC_ANNOUNCE_SIZE,
							CSIPC_ANNOUNCE_MAXMSG);
	
	
	strcpy ( channel->ch_announce->pl_buffer, csipc_listener_name );
	
	if ( csipc_int_write_channel ( channel->ch_announce ) ) {
		return channel;		
	} else {
		cs_log(	LOG_ERROR, 
				"Failed to write to announce channel" );
		return channel;
	}
}

void csipc_server_process( cs_srv_t *server )
{
	int rd_ctr;
	size_t announce_size;
	llist_t *node;
	cs_chan_t *listener;

	csassert( server != NULL );

	for ( rd_ctr = 0; rd_ctr < server->ch_announce->mq_size; rd_ctr++) {

		if ( ! csipc_int_read_channel( server->ch_announce ) )
			break;
		
		announce_size = strnlen ( server->ch_announce->pl_buffer,
					  server->ch_announce->pl_size );
		
		if ( announce_size == server->ch_announce->pl_size ) {
			cs_log( LOG_WARN, 
			 "IPC client tried to connect with unterminated name" );
			continue;
		}

		csipc_server_add_listener ( server, 
					    server->ch_announce->pl_buffer );
			
	}	

	for ( 	node = server->ch_listeners.next; 
		node != &( server->ch_listeners );
		node = node->next ) {
		
		listener = (cs_chan_t *) node;
		
		memcpy(listener->pl_buffer, server->pl_buffer, server->pl_size);

		if ( ! csipc_int_write_channel ( listener ) ) {
	
			if ( listener->mq_size >= 2 ) {
				
				csipc_int_read_channel  ( listener );
		
				memcpy(listener->pl_buffer, server->pl_buffer, server->pl_size);

				csipc_int_write_channel ( listener );

			}
		
		}
	}
		
}

void csipc_client_process( cs_chan_t *channel )
{
	int	rd_ctr;
	time_t	timenow;

	csassert( channel != NULL );

	time ( &timenow );

	for ( rd_ctr = 0; rd_ctr < channel->mq_size; rd_ctr++) {

		if ( ! csipc_int_read_channel( channel ) )
			break;
			
		channel->ch_lastupd = timenow;

	}
	
	if ( difftime(timenow, channel->ch_lastupd) > CSIPC_UPDATE_TIMEOUT ) {
		
		channel->ch_lastupd = timenow + CSIPC_ANNOUNCE_TIMEOUT;

		strcpy ( channel->ch_announce->pl_buffer, csipc_listener_name );
	
		if ( ! csipc_int_write_channel ( channel->ch_announce ) ) {
			cs_log(	LOG_ERROR, 
		"Failed to write to announce channel ( for reannounce )" );
		}

	}
		
}



