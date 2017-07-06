/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "pxfbridge.h"
#include "access/extprotocol.h"
#include "pxfuriparser.h"

static size_t fill_buffer(gphadoop_context *context, char *start, size_t size);

/*
 * Clean up churl related data structures from the context.
 */
void gpbridge_cleanup(gphadoop_context *context)
{
    PXFLOG("bridge cleanup");
    if (context == NULL)
        return;

    churl_cleanup(context->churl_handle, false);
    context->churl_handle = NULL;

    churl_headers_cleanup(context->churl_headers);
    context->churl_headers = NULL;

    if (context->gphd_uri != NULL) {
        freeGPHDUri(context->gphd_uri);
        context->gphd_uri = NULL;
    }
}

void gpbridge_import_start(gphadoop_context *context)
{

    /* parse the URI <-- moved to pxfprotocol.c */
    //context->gphd_uri = parseGPHDUri(EXTPROTOCOL_GET_URL(fcinfo));
    //if (is_import)
    //        Assert(context->gphd_uri->fragments != NULL);

    /*
    context->current_fragment = list_head(context->gphd_uri->fragments);
    build_uri_for_read(context);
    context->churl_headers = churl_headers_init();
    add_querydata_to_http_header(context, fcinfo);

    set_current_fragment_headers(context);

    context->churl_handle = churl_init_download(context->uri.data,
                                                context->churl_headers);
    */
    /* read some bytes to make sure the connection is established */
    //churl_read_check_connectivity(context->churl_handle);
}

int
gpbridge_read(gphadoop_context *context, char *databuf, int datalen)
{
    size_t n = 0;

    PXFLOG("bridge read");
    while ((n = fill_buffer(context, databuf, datalen)) == 0)
    {
        /* done processing all data for current fragment -
         * check if the connection terminated with an error */
        //churl_read_check_connectivity(context->churl_handle);

        PXFLOG("before next fragment");
        /* start processing next fragment */
        context->current_fragment = lnext(context->current_fragment);
        PXFLOG("after next fragment");
        if (context->current_fragment == NULL) {
            PXFLOG("no more fragments");
            return 0;
        }

        //set_current_fragment_headers(context);
        //churl_download_restart(context->churl_handle, context->uri.data, context->churl_headers);

        /* read some bytes to make sure the connection is established */
        //churl_read_check_connectivity(context->churl_handle);
    }

    PXFLOG("bridge read %d bytes", n);
    return (int) n;

}

static size_t
fill_buffer(gphadoop_context *context, char *start, size_t size)
{
    PXFLOG("fill buffer");

    size_t n = 0;
    char* ptr = start;
    char* end = ptr + size;

    while (ptr < end)
    {
        n = churl_read(/*context->churl_handle,*/ context, ptr, end - ptr);
        if (n == 0)
            break;

        ptr += n;
    }

    return ptr - start;
}

/*
// TODO: mock function, remove when real from libchurl is ported
static size_t
churl_read(gphadoop_context *context, char *ptr, size_t size) {

    if (context->row_count > 0) {
        PXFLOG("no more data to read");
        return 0;
    }
    (context->row_count)++;

    PXFLOG("produce tuple");

    snprintf(ptr, size, "%d,hello world %d", GpIdentity.segindex, GpIdentity.segindex);
    return strlen(ptr);
}
 */