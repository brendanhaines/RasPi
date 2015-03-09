import java.util.ArrayList;
import java.util.Scanner;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

// communication
import java.lang.*;
import java.io.*;
import java.net.*;

/**
 *
 */
class ConnectionPanel extends JPanel implements ActionListener {
    /** list to hold subscribed actionlisteners */
    private ArrayList<ActionListener> listeners = new ArrayList<ActionListener>();

    /** TCP connection options */
    private JPanel tcpOptions;
        /** label for address field */
        private JLabel hostAddrLabel;
        /** host address field */
        private JTextField hostAddr;
        /** label for port field */
        private JLabel hostPortLabel;
        /** port field */
        private JTextField hostPort;
    /** connect button */
    private JButton connectButton;

    /** holds outgoing message info */
    private MessageContent contentOut;
    /** holds incomming message info */
    private MessageContent contentIn;

    /** timer to cause repeating sends */
    private Timer tcpTimer;
    /** tcp connection socket */
    private Socket tcpSocket;
    /** scanner for incomming tcp messages */
    public BufferedReader in;
    /** printwriter for outgoing tcp messages */
    public PrintWriter out;

    /**
     * no-args constructor
     */
    public ConnectionPanel() {
        setLayout( new FlowLayout() );

        tcpOptions = new JPanel();
        tcpOptions.setLayout( new FlowLayout() );

        hostAddrLabel = new JLabel( "Host Address: " );
        hostAddr = new JTextField( "192.168.42.1" );
        hostAddr.setColumns( 10 );
        hostPortLabel = new JLabel( "Port: " );
        hostPort = new JTextField( "51717" );
        hostPort.setColumns( 6 );

        tcpOptions.add( hostAddrLabel );
        tcpOptions.add( hostAddr );
        tcpOptions.add( hostPortLabel );
        tcpOptions.add( hostPort );
        add( tcpOptions );

        connectButton = new JButton( "Connect" );
        connectButton.addActionListener( this );
        add( connectButton );
    }

    /**
     * @param outgoing - MessageContent object containing options for 
     */
    public ConnectionPanel( MessageContent outgoing, MessageContent incomming ) {
        this();
        contentOut = outgoing;
        contentIn = incomming;
    }

    /**
     * connect to tcp socket bound to port at host addres addr
     * @param addr - host address
     * @param port - port of socket at host
     */
    public void connectTcp( String addr, int port ) {
        try {
            tcpSocket = new Socket( addr, port );
            in = new BufferedReader( new InputStreamReader( tcpSocket.getInputStream() ) );
            out = new PrintWriter( tcpSocket.getOutputStream(), true ); //autoflushing
            tcpTimer = new Timer( 100, this );  // .1 seconds
            out.println( "PROTOCOL_2" );  // identify protocol to be used with tcp
            tcpTimer.start();
            setGui( true );
            fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "CONNECT" ) );
        }
        catch( Exception ex ) {
            System.out.println( "Error Connecting to TCP server: " + ex );
        }
    }

    /**
     * disconnects from tcp server
     * @param reconnect - tells server whether to listen for new connections. If false, server will shut down
     */
    public void disconnectTcp( boolean reconnect ) {
        out.print( "DISCONNECT " );
        out.print( "D" );
        if( reconnect ) out.print( "_R" );
        out.println();

        try {
            tcpTimer.stop();
            in.close();
            out.close();
            tcpSocket.close();
            setGui( false );
            fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "DISCONNECT" ) );
        }
        catch( IOException ex ) {
            System.out.println( ex );
        }
    }

    /**
     *
     */
    private void sendTCP() {
        out.print( "H " );   // Heartbeat / begin message indicator
        if( contentOut.getMotorsEnabled() ) out.print( "E " );  // enable motors
        if( contentOut.motorTesting ) {
            for( int i = 0; i < contentOut.motorValues.length; i++ ) {
                out.printf( "M%02d_%04d ", i, contentOut.motorValues[i] );
                if( contentOut.getMotorsEnabled() ) out.print( "E " );  // enable motors (redundant)
            }
        }
        if( contentOut.controls ) {
            for( int i = 0; i < contentOut.controlValues.length; i++ ) {
                out.printf( "C%02d_%04d ", i, contentOut.controlValues[i] );
                if( contentOut.getMotorsEnabled() ) out.print( "E " );  // enable motors (redundant)
            }
        }
        out.println();
    }

    /**
     *
     */
    private void readTCP() {
        try {
            if( in.ready() ) {
                String messageIn = in.readLine();
                Scanner temp = new Scanner( messageIn );
                boolean motorsEnabled = false;

                contentIn.motorTesting = false;
                contentIn.controls = false;
                contentIn.orientation = false;

                while( temp.hasNext() ) {
                    String next = temp.next();
                    if( next.indexOf( "H" ) == 0 ) {
                        System.out.print( "\nH " );
                    }
                    else if( next.indexOf( "E" ) == 0 ) {
                        motorsEnabled = true;
                        System.out.print( "E " );
                    }
                    else if( next.indexOf( "M" ) == 0 ) {
                        contentIn.motorTesting = true;
                        //int motorNum = Integer.parseInt( next.substring( 1 ) );
                        //int motorVal = Integer.parseInt( next.substring( 3, next.length() ) );
                        //contentIn.motorValues[ motorNum ] = motorVal;
                        System.out.print( "M " );
                    }
                    else if( next.indexOf( "C" ) == 0 ) {
                        contentIn.controls = true;
                        //int controlNum = Integer.parseInt( next.substring( 1 ) );
                        //int controlVal = Integer.parseInt( next.substring( 3, next.length() ) );
                        //contentIn.controlValues[ controlNum ] = controlVal;
                        System.out.print( "C " );
                    }
                    else if( next.indexOf( "O" ) == 0 ) {
                        contentIn.orientation = true;
                        System.out.print( "O " );
                    }
                }
                contentIn.setMotorsEnabled( motorsEnabled );
                contentIn.fireActionPerformed( new ActionEvent(contentIn, ActionEvent.ACTION_PERFORMED, "" ) );
            }
        }
        catch( IOException ex ) {}
    }

    /**
     * @param enabled - true to 
     */
    private void setGui( Boolean connected ) {
        if( connected ) connectButton.setText( "disconnect" );
        else connectButton.setText( "connect" );
        hostAddr.setEnabled( !connected );
        hostPort.setEnabled( !connected );
    }

    /**
     * sets everything in the panel enabled/disabled
     * @param state - desired enable status (true = enabled, false = disabled)
     */
    public void setEnabled( boolean state ) {
        hostAddr.setEnabled( state );
        hostPort.setEnabled( state );
        connectButton.setEnabled( state );
    }

    ///////////////////////////////////////

    /**
     * sends message over tcp
     */
    public void actionPerformed( ActionEvent evt ) {
        if( evt.getSource() == connectButton ) {
            if( evt.getActionCommand().equals( "disconnect" ) ) {
                disconnectTcp( false );
            }
            else {
                connectTcp( hostAddr.getText(), Integer.parseInt( hostPort.getText() ) );
            }
        }
        else if( connectButton.getText().equals("disconnect") ){
            sendTCP();
            readTCP();
        }
    }

    ////////////////////////////////////////

    /**
     * adds actionlistener to subscribd listener list
     * @param al - actionlistener to add
     */
    public void addActionListener( ActionListener al ) {
        listeners.add( al );
    }

    /**
     * removes actionlistener from subscribed listener list
     * @param al - actionlistener to remove
     */
    public void removeActionListener( ActionListener al ) {
        listeners.remove( al );
    }

    /**
     * sends an actionevent to subscribed listeners
     * @param ae - actionevent to use as parameter to all subscribed listeners
     */
    public void fireActionPerformed( ActionEvent ae ) {
        for( ActionListener al : listeners ) {
            al.actionPerformed( ae );
        }
    }

    /**
     *
     */
    public void fireActionPerformed() {
        fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "" ) );
    }
}




