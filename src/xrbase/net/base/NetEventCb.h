#pragma once

void reconnect_cb(struct bufferevent* bev, short events, void* arg);
void conn_event_cb(struct bufferevent* pBuffer, short events, void* args);
void conn_write_cb(struct bufferevent* pBuffer, void* args);
void conn_read_cb(struct bufferevent* pBuffer, void* args);
void init_buffer_mask(class NetBufferEv* pBuffer);