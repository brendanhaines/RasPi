import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 *
 */
class EStopWindow extends JFrame implements ActionListener {
    /** list to hold subscribed actionlisteners */
    private ArrayList<ActionListener> listeners = new ArrayList<ActionListener>();
    
    /** stop sign image */
    //private Image sign = new Image( "./stopSign.png" );

    /** Emergency stop button */
    private JButton disableButton;

    /**
     *
     */
    public EStopWindow() {
        super("Emergency Stop");
        setDefaultCloseOperation( JFrame.DO_NOTHING_ON_CLOSE );
        setAlwaysOnTop( true );
        setLocation( 600, 100 );

        //disableButton = new JButton( "DISABLE" );
        disableButton = new JButton( new ImageIcon( (new ImageIcon( "./stopSign.png" )).getImage().getScaledInstance( 200, 200, Image.SCALE_DEFAULT) ) );
        disableButton.addActionListener( this );
        add( disableButton );

        setEnabled( true );
        pack();
        setVisible( true );
    }

    /**
     *
     */
    public void actionPerformed( ActionEvent evt ) {
        if( evt.getSource() == disableButton ) {
            fireActionPerformed();
        }
    }

    /////////////////////////////////////

    /**
     *
     */
    public void setEnabled( boolean enable ) {
        disableButton.setEnabled( enable );
    }

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
        fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "CLICKED" ) );
    }
}