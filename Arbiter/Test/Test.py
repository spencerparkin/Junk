# Test.py

import socket
import select
import time
import xml.etree.ElementTree as ET
import re
import sys

class ArbiterClient:

    def __init__( self, name ):
        self.name = name
        self.client_socket = None
        self.socket_buffer = ''
        self.response_map = {}

    def Connect( self ):
        port = 9000
        self.client_socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        self.client_socket.connect( ( 'localhost', port ) )
        self.client_socket.setblocking(0)

    def Disconnect( self ):
        self.client_socket = None

    def SendRequest( self, xml_request ):
        request_buffer = ET.tostring( xml_request )
        bytes_remaining = len( request_buffer )
        while bytes_remaining > 0:
            #bytes_sent = self.client_socket.send( bytes( request_buffer, 'UTF-8' ) )
            bytes_sent = self.client_socket.send( request_buffer )
            if bytes_sent == 0:
                raise RuntimeError( 'Socket connection broken.' )
            bytes_remaining -= bytes_sent
            request_buffer = request_buffer[ bytes_sent : ]

    def ReceiveResponse( self, requestId ):
        xml_response = None
        ready_to_read, ready_to_write, has_error = select.select( [ self.client_socket ], [], [], 0 )
        if len( ready_to_read ) > 0:
            chunk_size = 4098
            while True:
                try:
                    chunk = self.client_socket.recv( chunk_size )
                    if not chunk or len( chunk ) == 0:
                        break
                except Exception as ex:
                    break
                chunk_str = chunk.decode( 'UTF-8' )
                self.socket_buffer += chunk_str
            while True:
                response_buffer = None
                match = re.search( '(<ArbiterResponse.*/>)', self.socket_buffer )
                if match:
                    response_buffer = match.group(1)
                else:
                    i = self.socket_buffer.find( '<ArbiterResponse' )
                    j = self.socket_buffer.find( '<ArbiterResponse/>' )
                    if i >= 0 and i < j:
                        j += len( '<ArbiterResponse/>' )
                        response_buffer = self.socket_buffer[ i : j ]
                if response_buffer:
                    i = self.socket_buffer.find( response_buffer )
                    i += len( response_buffer )
                    self.socket_buffer = self.socket_buffer[ i : ]
                    try:
                        xml_response = ET.fromstring( response_buffer )
                        if 'id' in xml_response.attrib:
                            self.response_map[ xml_response.attrib[ 'id' ] ] = xml_response
                    except:
                        pass
                else:
                    break
        if requestId in self.response_map:
            xml_response = self.response_map[ requestId ]
            del self.response_map[ requestId ]
        return xml_response

    def WaitForResponse( self, requestId ):
        while True:
            xml_response = self.ReceiveResponse( requestId )
            if xml_response != None:
                return xml_response
            #time.sleep(1)

def Main():

    try:
        if len( sys.argv ) != 2:
            raise Exception( 'Client name not given.' )

        client = ArbiterClient( sys.argv[1] )
        client.Connect()

        requestId = 0

        xml_request = ET.Element( 'ArbiterRequest' )
        xml_request.attrib[ 'id' ] = str( requestId )
        ET.SubElement( xml_request, 'Type' ).text = 'IdentifyClient'
        ET.SubElement( xml_request, 'ClientName' ).text = client.name

        client.SendRequest( xml_request )
        xml_response = client.WaitForResponse( str( requestId ) )
        requestId += 1

        mutex_array = []
        mutex_array.append( { 'name' : 'Apple', 'acquired' : False } )
        mutex_array.append( { 'name' : 'Orange', 'acquired' : False } )
        mutex_array.append( { 'name' : 'Grape', 'acquired' : False } )
        mutex_array.append( { 'name' : 'Cherry', 'acquired' : False } )
        mutex_array.append( { 'name' : 'Banana', 'acquired' : False } )

        while True:

            for mutex_entry in mutex_array:

                if not mutex_entry[ 'acquired' ]:

                    xml_request = ET.Element( 'ArbiterRequest' )
                    xml_request.attrib[ 'id' ] = str( requestId )
                    ET.SubElement( xml_request, 'Type' ).text = 'AcquireMutex'
                    ET.SubElement( xml_request, 'MutexName' ).text = mutex_entry[ 'name' ]

                    client.SendRequest( xml_request )
                    xml_response = client.WaitForResponse( str( requestId ) )
                    requestId += 1

                    if 'mutexAcquired' in xml_response.attrib and xml_response.attrib[ 'mutexAcquired' ] == 'true':
                        mutex_entry[ 'acquired' ] = True

                else:

                    xml_request = ET.Element( 'ArbiterRequest' )
                    xml_request.attrib[ 'id' ] = str( requestId )
                    ET.SubElement( xml_request, 'Type' ).text = 'ReleaseMutex'
                    ET.SubElement( xml_request, 'MutexName' ).text = mutex_entry[ 'name' ]

                    client.SendRequest( xml_request )
                    xml_response = client.WaitForResponse( str( requestId ) )
                    requestId += 1

                    if 'mutexReleased' in xml_response.attrib and xml_response.attrib[ 'mutexReleased' ] == 'true':
                        mutex_entry[ 'acquired' ] = False

    except Exception as ex:
        print( 'Got exception: ' + str( ex ) )

if __name__ == "__main__":
    Main()

# Test.py