import java.util.Scanner;

import java.lang.*;
import java.io.*;
import java.net.*;

import javax.swing.*;

class ExampleServer {
    private ServerSocket srvr;
    private Socket skt;
    private BufferedReader in;
    private PrintWriter out;

    private MessageContent txContent;
    private MessageContent rxContent;

    private JFrame window;
    private JCheckBox motorsEnabled;
    private JCheckBox motorTest;
    private JTextArea motorTestValues;

    public ExampleServer( int port ) {
        window = new JFrame( "Example Server" );
        window.getContentPane().setLayout( new BoxLayout( window.getContentPane(), BoxLayout.PAGE_AXIS ) );

        motorsEnabled = new JCheckBox( "motors enabled" );
        motorsEnabled.setEnabled( false );
        window.getContentPane().add( motorsEnabled );

        motorTest = new JCheckBox( "motor test" );
        motorTest.setEnabled( false );
        window.getContentPane().add( motorTest );

        motorTestValues = new JTextArea(   "MOTOR TEST VALUES:\n" +
                                           "   m3    ^   m1   \n" +
                                           "  ----   |  ----  \n" +
                                           "       FRONT      \n" +
                                           "   m2        m0   \n" +
                                           "  ----      ----  " );
        motorTestValues.setEnabled( false );
        window.getContentPane().add( motorTestValues );

        window.setBounds( 850, 100, 10, 10 );
        window.pack();
        window.setVisible(true);

        System.out.println( "Setting up TCP server..." );
        try {
            System.out.print( "Setting up server\n" );
            srvr = new ServerSocket( port );
            System.out.println( "address: " + srvr.getInetAddress() );
            System.out.println( "port: " + srvr.getLocalPort() );
            System.out.println( "Waiting for connection..." );
            skt = srvr.accept();
            System.out.println( "Server has connected!" );

            in = new BufferedReader( new InputStreamReader( skt.getInputStream() ) );
            out = new PrintWriter( skt.getOutputStream(), true);

            while( true ) {
                while( !in.ready() );

                try {
                    String messageIn = in.readLine();
                    Scanner temp = new Scanner( messageIn );
                    boolean motorsEnabledBool = false;
                    boolean motorTesting = false;
                    int[] motorValues = { 0, 0, 0, 0 };

                    while( temp.hasNext() ) {
                        String next = temp.next();
                        if( next.indexOf( "D" ) >= 0 || next.indexOf( "DISCONNECT" ) >= 0 ) {
                            System.out.print( "\n[ .... ] DISCONNECTING..." );
                            skt.close();
                            srvr.close();
                            System.out.println( "\r[ DONE ]" );
                            System.exit(0);
                        }
                        else if( next.indexOf( "H" ) == 0 ) {
                            //System.out.print( "\nH " );
                        }
                        else if( next.indexOf( "E" ) == 0 ) {
                            motorsEnabledBool = true;
                            //System.out.print( "E " );
                        }
                        else if( next.indexOf( "M" ) == 0 ) {
                            motorTesting = true;
                            int motorNum = Integer.parseInt( next.substring( 1, 3 ) );
                            int motorVal = Integer.parseInt( next.substring( 4, 8 ) );
                            motorValues[ motorNum ] = motorVal;
                            //System.out.print( "M " );
                        }
                    }
                    motorsEnabled.setSelected( motorsEnabledBool );
                    motorTest.setSelected( motorTesting );
                    motorTestValues.setText(
                        "MOTOR TEST VALUES:\n" +
                       "   m3    ^   m1   \n" +
                       String.format( "  %04d   |  %04d  \n", motorValues[3], motorValues[1] ) +
                       "       FRONT      \n" +
                       "   m2        m0   \n" +
                       String.format( "  %04d   |  %04d  \n", motorValues[2], motorValues[0] ) );
                }
                catch( IOException ex ) {}
            }
        }
        catch( Exception ex ) {
            System.out.println( ex );
        }
    }

    public static void main( String[] args ) {
        ExampleServer example = new ExampleServer( 51717 );
    }
}