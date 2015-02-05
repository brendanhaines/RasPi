// graphics
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.awt.image.*;
import javax.swing.border.*;

// communication
import java.lang.*;
import java.io.*;
import java.net.*;

/**
 * Replaces GroundStation.java
 * Written by Brendan Haines
 */
class GroundStation implements ActionListener, Runnable {

    //////////////////
    ///// FIELDS /////
    //////////////////

    /** string to hold the connection type */
    private String connectionType = "";
    /** boolean to hold connected/disconnected status */
    private boolean connectionStatus = false;
    /** TCP/IP socket for communication */
    private Socket tcpSocket;
    /** input buffer handling for tcpSocket */
    private BufferedReader tcpIn;
    /** output handler for tcpSocket */
    private PrintWriter tcpOut;
    /** thread to listen to tcpSocket */
    private Thread tcpListener;

    // Main GUI components
    /** window to hold GUI */
    private JFrame mainFrame;
    /** panel to fill mainFrame */
    private JPanel mainPanel;
    
    /** panel to hold connection options */
    private JPanel connectionPanel;
        /** connection options panel */
        private JPanel connectionOptionsPanel;
            /** connection type selection box */
            private JComboBox connectionTypeComboBox;
            /** card layout container for different connection types */
            private JPanel connectionTypePanel;
                /** JPanel to hold TCP/IP settings */
                private JPanel tcpPanel;
                    /** label for host address field */
                    private JLabel tcpHostAddressFieldLabel;
                    /** text field for host address */
                    private JTextField tcpHostAddressField;
                    /** label for port field */
                    private JLabel tcpHostPortFieldLabel;
                    /** text field for port of socket */
                    private JTextField tcpHostPortField;
        /** connect/disconnect button */
        private JButton connectButton;
    
    /** tabbed pane to hold many features */
    private JTabbedPane tabbedPane;
        /** command line interface tab */
        private JPanel cliPanel;
            /** panel to hold display options */
            private JPanel cliOptionsPanel;
                /** checkbox to display confirmation/replies from pi */
                private JCheckBox cliShowReceivedCheckBox;
            /** scrolling pane to hold command area */
            private JScrollPane cliCommandScrollPane;
                /** text area to display commands */
                private JTextArea cliCommandArea;
            /** panel to hold input stuff */
            private JPanel cliInputPanel;
                /** text field to enter commands */
                private JTextField cliInputField;
                /** send command button */
                private JButton cliInputButton;
        /** control panel */
        private JPanel controlPanel;
            /** arm/disarm flight controller button */
            private JButton armDisarmButton;
            /** calibrate IMU button */
            private JButton calibrateImuButton;
        /** motor testing tab */
        private JPanel motorTestPanel;
            /** placeholder */
            private JLabel motorTestPlaceholder;

    ////////////////////////
    ///// CONSTRUCTORS /////
    ////////////////////////

    public GroundStation() {
        setupWindow();
    }

    ///////////////////
    ///// METHODS /////
    ///////////////////

    /**
     * sets up GUI
     */
    private void setupWindow() {
        mainFrame = new JFrame( "PiCopter Ground Station" );
        mainFrame.setResizable( true );
        mainFrame.setDefaultCloseOperation( JFrame.DISPOSE_ON_CLOSE );

        mainPanel = new JPanel();
        mainPanel.setLayout( new BorderLayout() );

        setupConnectionPanel();
        setupTabbedPane();

        mainPanel.add( connectionPanel, BorderLayout.NORTH );
        mainPanel.add( tabbedPane, BorderLayout.CENTER );

        mainFrame.add( mainPanel );

        mainFrame.pack();
        mainFrame.setVisible( true );
    }

    /**
     * sets up the connection panel
     */
    private void setupConnectionPanel() {
        connectionPanel = new JPanel( new BorderLayout() );
        connectionPanel.setBorder( BorderFactory.createTitledBorder( "connection options" ) );
        connectionOptionsPanel = new JPanel( new FlowLayout() );

        connectionTypeComboBox = new JComboBox();
        connectionTypeComboBox.addItem( "TCP/IP" );
        connectionTypeComboBox.setSelectedIndex( 0 );
        connectionOptionsPanel.add( connectionTypeComboBox );

        connectionTypePanel = new JPanel( new CardLayout() );

        tcpPanel = new JPanel( new FlowLayout() );
        
        tcpHostAddressFieldLabel = new JLabel( "Host Address:" );
        tcpHostAddressField = new JTextField();
        tcpHostAddressField.setColumns( 10 );
        tcpHostAddressField.setText( "192.168.42.1" );
        tcpHostPortFieldLabel = new JLabel( "Port:" );
        tcpHostPortField = new JTextField();
        tcpHostPortField.setColumns( 6 );
        tcpHostPortField.setText( "51717" );
        
        tcpPanel.add( tcpHostAddressFieldLabel );
        tcpPanel.add( tcpHostAddressField );
        tcpPanel.add( tcpHostPortFieldLabel );
        tcpPanel.add( tcpHostPortField );

        connectionTypePanel.add( tcpPanel, "TCP/IP" );

        connectionOptionsPanel.add( connectionTypePanel );
        connectionPanel.add( connectionOptionsPanel, BorderLayout.NORTH );

        connectButton = new JButton( "connect" );
        connectButton.addActionListener( this );
        connectionPanel.add( connectButton, BorderLayout.SOUTH );
    }

    /**
     * sets up tabs
     */
    private void setupTabbedPane() {
        tabbedPane = new JTabbedPane();

        setupCliTab();
        setupControlTab();
        setupMotorTestTab();

        tabbedPane.add( cliPanel, "CLI", 0 );
        tabbedPane.add( controlPanel, "Control", 1 );
        tabbedPane.add( motorTestPanel, "Motor Testing", 2 );
        tabbedPane.setEnabledAt( 1, false );
        tabbedPane.setEnabledAt( 2, false );
    }

    /**
     * sets up command line interface tab
     */
    private void setupCliTab() {
        cliPanel = new JPanel();
        cliPanel.setLayout( new BorderLayout() );

        cliOptionsPanel = new JPanel();
        cliOptionsPanel.setLayout( new FlowLayout( FlowLayout.LEFT ) );

        cliShowReceivedCheckBox = new JCheckBox( "show receive" );
        cliShowReceivedCheckBox.setSelected( true );
        cliShowReceivedCheckBox.addActionListener( this );
        cliOptionsPanel.add( cliShowReceivedCheckBox );

        cliCommandArea = new JTextArea();
        cliCommandArea.setEditable( false );
        cliCommandScrollPane = new JScrollPane( cliCommandArea );
        cliCommandScrollPane.setPreferredSize( new Dimension( 400, 200 ) );
        new SmartScroller( cliCommandScrollPane );
        
        cliInputPanel = new JPanel();
        cliInputPanel.setLayout( new BorderLayout() );
        cliInputField = new JTextField();
        cliInputField.addActionListener( this );
        cliInputButton = new JButton( "send" );
        cliInputButton.addActionListener( this );
        cliInputPanel.add( cliInputField, BorderLayout.CENTER );
        cliInputPanel.add( cliInputButton, BorderLayout.EAST );

        cliPanel.add( cliOptionsPanel, BorderLayout.NORTH );
        cliPanel.add( cliCommandScrollPane, BorderLayout.CENTER );
        cliPanel.add( cliInputPanel, BorderLayout.SOUTH );
    }

    /**
     * sets up control panel tab
     */
    private void setupControlTab() {
        controlPanel = new JPanel( new FlowLayout( FlowLayout.LEFT ) );

        armDisarmButton = new JButton( "arm" );
        armDisarmButton.addActionListener( this );

        calibrateImuButton = new JButton( "calibrate IMU" );
        calibrateImuButton.addActionListener( this );

        controlPanel.add( armDisarmButton);
        controlPanel.add( calibrateImuButton);
    }

    /**
     * sets up motor testing tab
     */
    private void setupMotorTestTab() {
        motorTestPanel = new JPanel();
        motorTestPanel.setLayout( new BorderLayout() );
        motorTestPlaceholder = new JLabel( "Motor Test Tab Placeholder" );
        motorTestPanel.add( motorTestPlaceholder, BorderLayout.NORTH );
    }

    /**
     * enables/disables all components in a container
     * @param container - the container to enable/disable components of
     * @param enable - boolean value for .setEnabled()
     */
    private void enableComponents( Container container, boolean enable ) {
        Component[] components = container.getComponents();
        for( Component component : components ) {
            component.setEnabled( enable );
            if (component instanceof Container) {
                enableComponents( (Container) component, enable );
            }
        }
    }

    /**
     * actionPerformed method to handle ActionEvents from all buttons and GUI interaction
     */
    public void actionPerformed( ActionEvent evt ) {

        // command line interface
        if( evt.getSource() == cliInputButton || evt.getSource() == cliInputField ) {
            String inputText = cliInputField.getText();
            cliInputField.setText( "" );
            if( inputText.equals( "" ) ) return;  // if cliInput field is empty, do nothing
            cliCommandArea.append( inputText + "\n" );

            if( inputText.indexOf( "CONNECT" ) == 0 ) {
                cliCommandArea.append( "--ERROR - connection with command line interface not supported\n");
            }
            else if( inputText.indexOf( "DISCONNECT") == 0 ) {
                disconnect();
            }
            else if( inputText.indexOf( "SEND " ) == 0 ) {
                String sendText = inputText.substring( 5, inputText.length() );
                cliCommandArea.append( "--SENDING: " + sendText + "\n" );
                tcpOut.println( sendText );
            }
            else if( inputText.indexOf( "CLEAR" ) == 0 ) {
                cliCommandArea.setText( "" );
            }

            else {
                
            }
        }

        // connect button
        else if( evt.getSource() == connectButton && evt.getActionCommand().equals( "connect" ) ) {
            if( connectionTypeComboBox.getSelectedItem().equals( "TCP/IP" ) ) {
                connectionType = "TCP/IP";
                if( tcpHostAddressField.getText().equals( "" ) || tcpHostPortField.getText().equals( "" ) ) {
                    System.out.println( "must enter an address & port for TCP/IP" );
                }
                else if( connectTCP( tcpHostAddressField.getText(), Integer.parseInt( tcpHostPortField.getText() ) ) ) {
                    startTcpListener();
                }
                else {
                    System.out.println( "failed to connect to TCP/IP server" );
                }
            }
            else {
                System.out.println( "idk how you did that, but this protocol isn't supported...connectionTypeComboBox conains \"" + connectionTypeComboBox.getSelectedItem() + "\"" );
            }
        }

        // disconnect button
        else if( evt.getSource() == connectButton && evt.getActionCommand().equals( "disconnect" ) ) {
            disconnect();
        }

        // arm button
        else if( evt.getSource().equals( armDisarmButton ) && evt.getActionCommand().equals( "arm" ) ){
            cliCommandArea.append( "--ARMING..." );
            tcpOut.println( "ARM_CONTROLLER" );
            armDisarmButton.setText( "disarm" );
            cliCommandArea.append( "DONE\n" );
        }

        // disarm button
        else if( evt.getSource().equals( armDisarmButton ) && evt.getActionCommand().equals( "disarm" ) ) {
            cliCommandArea.append( "--DISARMING..." );
            tcpOut.println( "DISARM_CONTROLLER" );
            armDisarmButton.setText( "arm" );
            cliCommandArea.append( "DONE\n" );
        }
    }

    /**
     * connects over TCP/IP protocol
     * @param hostAddress - the ip address of the server host
     * @param hostPort - the port of the socket at the host
     * @return true on success, false on failure
     */
    private boolean connectTCP( String hostAddress, int hostPort ) {
        cliCommandArea.append( "--CONNECTING WITH TCP..." );
        
        try {
            tcpSocket = new Socket( hostAddress, hostPort );
            tcpIn = new BufferedReader( new InputStreamReader( tcpSocket.getInputStream() ) );
            tcpOut = new PrintWriter( tcpSocket.getOutputStream(), true );
            startTcpListener();
        }
        catch( UnknownHostException ex ) {
            cliCommandArea.append( "UnknownHostException\n" );
            return false;
        }
        catch( IOException ex ) {
            cliCommandArea.append( "IOException\n" );
            return false;
        }

        setGuiConnected();
        connectionStatus = true;
        cliCommandArea.append( "DONE\n" );
        return true;
    }

    /**
     *
     */
    private void startTcpListener() {
        tcpListener = new Thread( this, "tcpListenerThread" );
        tcpListener.start();
    }

    /**
     * the method used by the TCP listening thread
     */
    public void run() {
        while( connectionStatus ) {
            try {
                if( tcpIn.ready() ) {
                    cliRxAppend( "--READING: " + tcpIn.readLine() + "\n" );
                }
            }
            catch( Exception ex ) {}
        }
    }

    /**
     * will add the message to cli view if show receive checkbox is checked
     */
    public void cliRxAppend( String message ) {
        if( cliShowReceivedCheckBox.isSelected() ) {
            cliCommandArea.append( message );
        }
    }

    /**
     *
     */
    private void setGuiConnected() {
        connectButton.setText( "disconnect" );
        tabbedPane.setEnabledAt( 1, true ); // control panel
        tabbedPane.setEnabledAt( 2, true ); // motor testing panel
        enableComponents( connectionOptionsPanel, false );
        mainFrame.setDefaultCloseOperation( JFrame.DO_NOTHING_ON_CLOSE );
    }

    /**
     *
     */
    private void setGuiDisconnected() {
        connectButton.setText( "connect" );
        tabbedPane.setSelectedIndex( 0 );
        tabbedPane.setEnabledAt( 1, false );    // control panel
        tabbedPane.setEnabledAt( 2, false );    // motor testing panel
        enableComponents( connectionOptionsPanel, true );
        mainFrame.setDefaultCloseOperation( JFrame.DISPOSE_ON_CLOSE );
    }

    /**
     * disconnects from whatever connection has been set up with Raspberry Pi
     */
    private void disconnect() {
        cliCommandArea.append( "--DISCONNECTING...");
        
        tcpOut.println( "DISCONNECT" );

        try {
            tcpIn.close();
            tcpOut.close();
        }
        catch( IOException ex ) {
            cliCommandArea.append( "IOException\n" );
            return;
        }

        setGuiDisconnected();
        connectionStatus = false;
        cliCommandArea.append( "DONE\n" );
    }

    /**
     * main begins the ground station
     */
    public static void main( String[] args ) {
        GroundStation groundStation = new GroundStation();
    }
}



