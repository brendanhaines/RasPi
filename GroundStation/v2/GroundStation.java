import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 *
 */
class GroundStation implements ActionListener {

    /** main window to hold everything */
    private JFrame mainWindow;
    /** window to hold emergency stop button */
    private EStopWindow eStopWindow = new EStopWindow( false );

    /** connection options section of window */
    private ConnectionPanel connectPanel;

    /** tabbed pane to hold stuff */
    private JTabbedPane tabbedPane;
    /** motor testing panel */
    private MotorTestPanel motorTestPanel;
    /** displays visual representation of copter orientation */
    private Display3d orientationPanel;
    /** displays visual representation of control positions */
    private DisplayController controllerPanel;

    /** outgoing message content options */
    public MessageContent sendContent = new MessageContent();
    /** incoming message content */
    public MessageContent receiveContent = new MessageContent();
    
    public GroundStation() {
        mainWindow = new JFrame( "PiCopter Ground Station" );
        mainWindow.setResizable( false );
        mainWindow.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
        mainWindow.getContentPane().setLayout( new BoxLayout( mainWindow.getContentPane(), BoxLayout.PAGE_AXIS ) );

        connectPanel = new ConnectionPanel( sendContent, receiveContent );
        connectPanel.setEnabled( true );
        mainWindow.getContentPane().add( connectPanel );

        JButton fcButton = new JButton( "arm" );
        mainWindow.getContentPane().add( fcButton );

        tabbedPane = new JTabbedPane();
        mainWindow.getContentPane().add( tabbedPane );

        controllerPanel = new DisplayController( receiveContent );
        tabbedPane.add( controllerPanel, "Controller" );

        orientationPanel = new Display3d( receiveContent );
        tabbedPane.add( orientationPanel, "Orientation" );

        motorTestPanel = new MotorTestPanel( sendContent );
        motorTestPanel.setEnabled( false );
        tabbedPane.add( motorTestPanel, "Motor Test" );

        connectPanel.addActionListener( this );
        sendContent.addActionListener( this );

        mainWindow.pack();
        mainWindow.setVisible( true );

        sendContent.orientation = true;
    }

    public void disableMotors() {
        connectPanel.setEnabled( true );
        eStopWindow.dispose();
    }

    public void enableMotors() {
        connectPanel.setEnabled( false );
        eStopWindow = new EStopWindow();
        eStopWindow.addActionListener( this );
    }

    public void actionPerformed( ActionEvent evt ) {
        if( evt.getSource() == eStopWindow ) {
            sendContent.setMotorsEnabled( false );
        }
        if( evt.getSource() == connectPanel ) {
            if( evt.getActionCommand().equals( "CONNECT" ) ) {
                mainWindow.setDefaultCloseOperation( JFrame.DO_NOTHING_ON_CLOSE );
                motorTestPanel.setEnabled( true );
            }
            else if( evt.getActionCommand().equals( "DISCONNECT" ) ) {
                mainWindow.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
                motorTestPanel.setEnabled( false );
            }
        }
        else if( evt.getActionCommand().equals( "ENABLE_MOTORS" ) ) {
            enableMotors();
        }
        else if( evt.getActionCommand().equals( "DISABLE_MOTORS" ) ) {
            disableMotors();
        }
    }

    public static void main( String[] args ) {
        GroundStation station = new GroundStation();
    }
}