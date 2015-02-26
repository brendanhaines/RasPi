import java.util.*;
import java.awt.event.*;

/**
 *
 */
class MessageContent {
    /** list to hold subscribed actionlisteners */
    private ArrayList<ActionListener> listeners = new ArrayList<ActionListener>();

    private boolean motorsEnabled = false;

    public boolean motorTesting = false;
    public int[] motorValues = { 0, 0, 0, 0 };

    public boolean controls = false;
    public int[] controlValues = { 0, 0, 0, 0, 0, 0 };

    public boolean orientation = false;
    public double[] orientValues = { 1, 0, 0, 0 };  // w, x, y, z

    /**
     * sets motorsEnabled variable and triggers fireActionPerformed
     * @param enable - boolean value to set motorsEnabled to
     */
    public void setMotorsEnabled( Boolean enable ) {
    	motorsEnabled = enable;
    	if( enable ) {
            fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "ENABLE_MOTORS" ) );
        }
        else {
            fireActionPerformed( new ActionEvent( this, ActionEvent.ACTION_PERFORMED, "DISABLE_MOTORS" ) );
        }
    }

    /**
     * @return motorsEnabled status
     */
    public Boolean getMotorsEnabled() {
    	return motorsEnabled;
    }

    /////////////////////////////////////////////

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
}