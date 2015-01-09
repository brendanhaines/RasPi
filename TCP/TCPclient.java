/**
 * TCPclient.java
 * Written by Brendan Haines
 * Based on example from http://www.java-samples.com/showtutorial.php?tutorialid=1167
 */

import java.lang.*;
import java.io.*;
import java.net.*;

class TCPclient {
  /** Socket at the server */
  private Socket socket;
  /** buffer to hold received info */
  private BufferedReader buffer;

  /**
   * Connects to socket at server
   * @param host - the server's IP
   * @param port - the port number of the socket at the server
   */
  public TCPclient( String host, int port ) {
    try {
      System.out.println( "setting up socket" );
      socket = new Socket( host, port );
      System.out.println( "socket set up\nsetting up buffer" );
      buffer = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );
      System.out.println( "buffer set up" );
    }
    catch( Exception e ) {
      System.out.println( "ERROR -- constructor" );
      close();
    }
  }

  /**
   * reads one line from the receive buffer
   * @return one line of the receive buffer on success, null on failure
   */
  public String readLine() {
    try{
      if( buffer.ready() ) {
        return buffer.readLine();
      }
      else {
        return null;
      }
    }
    catch( Exception e ) {
      System.out.println( "Hmmm... it won't read" );
      return null;
    }
  }

  /**
   * close connection
   */
  public void close() {
    try {
      buffer.close();
      socket.close();
    }
    catch( Exception e ) {
      System.out.println( "SH*T! the connection won't close" );
    }
  }
}

