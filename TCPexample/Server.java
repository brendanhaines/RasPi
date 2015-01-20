import java.lang.*;
import java.io.*;
import java.net.*;

/**
 * Echo server. if receives "DISCONNECT", will shut down server properly
 */
class Server {
   public static void main(String args[]) {
      ServerSocket srvr;
      Socket skt;
      BufferedReader in;
      PrintWriter out;

      try {
         System.out.print( "Setting up server\n" );
         srvr = new ServerSocket( 51717 );
         skt = srvr.accept();
         System.out.print( "Server has connected!\n" );

         in = new BufferedReader( new InputStreamReader( skt.getInputStream() ) );
         out = new PrintWriter( skt.getOutputStream(), true);

         while( true ) {
            System.out.print( "waiting for message...\n" );
            while( !in.ready() ) {}
            String rx = in.readLine();
            System.out.print( "message received: " + rx + "\n" );

            if( rx.equals( "DISCONNECT" ) ) {
               System.out.print( "disconnecting\n" );
               break;
            }

            System.out.print("Sending string: '" + rx + "'\n");
            out.println( rx );
         }
         out.close();
         in.close();
         skt.close();
         srvr.close();
      }
      catch(Exception e) {
         System.out.print("Whoops! It didn't work!\n");
      }
   }
}